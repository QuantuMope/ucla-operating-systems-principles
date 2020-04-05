#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <signal.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "zlib.h"

#define CHUNK 1024

/*
 * Uses getopt_long() to process command line arguments.
 * In this case, only --shell is a valid argument.
 * Returns 1 if --shell is provided and 0 otherwise.
 */
void process_command_line(int argc, char** argv, int* args) {
    char c;
    int option_index = 0;
    static struct option long_options[] = {
            {"port",     required_argument, 0, 'p'},
            {"compress", no_argument,       0, 'c'},
            {0, 0, 0, 0}
    };
    while ((c = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        switch (c) {
            case '?':
                fprintf(stderr, "Invalid argument. Valid arguments include --port=PORT_NO"
                                ", --log=FILENAME, --compress. Port argument is mandatory.\n");
                exit(1);
            case 'p':
                args[0] = atoi(optarg);
                break;
            case 'c':
                args[1] = 1;
        }
    }
    // Check for any other unwanted command line arguments
    if (optind < argc) {
        fprintf(stderr, "Unrecognized command line argument: %s\n", argv[optind++]);
        exit(1);
    }
    // If mandatory --port argument no specified
    if (!args[0]) {
        fprintf(stderr, "--port argument must be provided. \n");
        exit(1);
    }
}

/*
 * A write function that greatly aids in reducing repeated code.
 */
void write_check(int fd, void* buf, int bytes, char* path) {
    if (write(fd, buf, bytes) < 0) {
        fprintf(stderr, "Failed to write to %s: %s\n", path, strerror(errno));
        exit(1);
    }
}

int initialize_server(int port_no) {
    // Initialize TCP socket.
    int sock_fd, new_sock_fd;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Server failed to initialize socket: %s\n", strerror(errno));
        exit(1);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_no);

    // Setting socket option to SO_REUSEADDR allows quick reconnection after closing the socket.
    int opt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        fprintf(stderr, "Server failed to set socket options: %s\n", strerror(errno));
        exit(1);
    }
    if (bind(sock_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        fprintf(stderr, "Server failed to bind socket: %s\n", strerror(errno));
        exit(1);
    }
    fprintf(stdout, "Server waiting for client connection.\n");
    if (listen(sock_fd, 5) < 0) {
        fprintf(stderr, "Server failed to listen to socket: %s\n", strerror(errno));
        exit(1);
    }
    socklen_t cli_addr_size = sizeof(address);
    if ((new_sock_fd = accept(sock_fd, (struct sockaddr*)&address, &cli_addr_size)) < 0) {
        fprintf(stderr, "Server failed to accept socket connection: %s\n", strerror(errno));
        exit(1);
    }
    fprintf(stdout, "Server successfully connected to client.\n");
    return new_sock_fd;
}

void process_client_requests(int client_fd, int compress_option) {
    // Initiate pipes from process to shell.
    int pfd1[2];  // read 3, write 4
    int pfd2[2];  // read 5, write 6
    if (pipe(pfd1) < 0 || pipe(pfd2) < 0) {
        fprintf(stderr, "Creating pipes failed: %s\n", strerror(errno));
        exit(1);
    }

    // Initialize child process.
    pid_t pid;
    if ((pid = fork()) < 0) {
        fprintf(stderr, "Forking child process failed: %s\n", strerror(errno));
        exit(1);
    }
    // Child process. Setup proper pipe connections.
    else if (pid == 0) {
        close(pfd1[1]);  // close fd 4 (write)
        close(pfd2[0]);  // close fd 5 (read)
        close(0);
        dup(pfd1[0]);    // make fd 3 (read) new stdin
        close(pfd1[0]);
        close(1);
        close(2);
        dup(pfd2[1]);    // make fd 6 (write) new stdout
        dup(pfd2[1]);    // make fd 6 (write) new stderr
        close(pfd2[1]);

        // Execute shell.
        char *no_args[] = {"/bin/bash", NULL};
        if (execv("/bin/bash", no_args) < 0) {
            fprintf(stderr, "Failed to execute shell: %s\n", strerror(errno));
            exit(1);
        }
    }
    // Parent process. Setup proper pipe connections.
    else {
        close(pfd1[0]); // close fd 3 (read)
        close(pfd2[1]); // close fd 6 (write)
    }

    // File descriptors.
    int sin  = pfd1[1]; // writing sin will write to shell
    int sout = pfd2[0]; // reading from sout is shell output

    // Set up polling for socket and shell input
    struct pollfd poll_list[2];
    poll_list[0].fd = client_fd; // poll client
    poll_list[0].events = POLLIN;
    poll_list[1].fd = sout; // poll shell output
    poll_list[1].events = POLLIN;

    int count, status;
    int escape = 0;
    char buf[512];

    if (!compress_option) {
        while (1) {
            if (poll(poll_list, 2, 0) < 0) {
                fprintf(stderr, "Polling has failed: %s\n", strerror(errno));
                exit(1);
            }
            // Monitor client input read ready.
            if (poll_list[0].revents & POLLIN) {
                if ((count = read(client_fd, &buf, 512)) < 0) {
                    fprintf(stderr, "Reading from client failed: %s\n", strerror(errno));
                    close(sin);
                    exit(1);
                }
                for (int i = 0; i < count; i++) {
                    switch (buf[i]) {
                        case '\3':
                            if (kill(pid, SIGINT) < 0) {
                                fprintf(stderr, "Failed to kill shell process: %s\n", strerror(errno));
                                exit(1);
                            }
                            escape = 1;
                            break;
                        case '\4':
                            close(sin); // close pipe to shell
                            escape = 1;
                            break;
                    }
                    if (escape)
                        break;
                    write_check(sin, buf+i, 1, "shell");
                }
            }

            // Monitor shell read ready.
            if (poll_list[1].revents & POLLIN) {
                if ((count = read(sout, &buf, 512)) < 0) {
                    fprintf(stderr, "Reading from shell failed: %s\n", strerror(errno));
                    exit(1);
                }
                write_check(client_fd, buf, count, "socket");
            }
            // Monitor shell termination states.
            if (poll_list[1].revents & (POLLHUP | POLLERR)) {
                if (waitpid(pid, &status, 0) < 0) {
                    fprintf(stderr, "Failed to obtain shell exit status: %s\n", strerror(errno));
                    exit(1);
                }
                fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
                close(client_fd);  // close network socket
                break;
            }
        }
    }
    else {
        int have;
        unsigned char translated[CHUNK];
        unsigned char compressed[CHUNK];
        int compressed_data_size;

        // Initialize zlib stream.
        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        int ret;

        while (1) {
            if (poll(poll_list, 2, 0) < 0) {
                fprintf(stderr, "Polling has failed: %s\n", strerror(errno));
                exit(1);
            }
            // Monitor client input read ready.
            if (poll_list[0].revents & POLLIN) {
                if (read(client_fd, &compressed_data_size, sizeof(int)) < 0) {
                    fprintf(stderr, "Reading from client failed: %s\n", strerror(errno));
                    exit(1);
                }
                if ((count = read(client_fd, &buf, ntohl(compressed_data_size))) < 0) {
                    fprintf(stderr, "Reading from client failed: %s\n", strerror(errno));
                    exit(1);
                }
                if (inflateInit(&strm) != Z_OK) {
                    fprintf(stderr, "Failed to initialize decompression settings.\n");
                    exit(1);
                }
                // Set decompression parameters and pointers.
                strm.avail_in = count;
                strm.next_in = (unsigned char*)buf;
                strm.avail_out = CHUNK;
                strm.next_out = translated;

                ret = inflate(&strm, Z_SYNC_FLUSH);
                if (ret != Z_OK && ret != Z_STREAM_END) {
                    fprintf(stderr, "Experienced error during decompression.\n");
                    exit(1);
                }
                have = CHUNK - strm.avail_out;

                for (int i = 0; i < have; i++) {
                    switch (translated[i]) {
                        case '\3':
                            if (kill(pid, SIGINT) < 0) {
                                fprintf(stderr, "Failed to kill shell process: %s\n", strerror(errno));
                                exit(1);
                            }
                            escape = 1;
                            break;
                        case '\4':
                            close(sin); // close pipe to shell
                            escape = 1;
                            break;
                    }
                    if (escape)
                        break;
                    write_check(sin, translated+i, 1, "shell");
                }
                inflateEnd(&strm);
            }
            // Monitor shell read ready.
            if (poll_list[1].revents & POLLIN) {
                if ((count = read(sout, &buf, 512)) < 0) {
                    fprintf(stderr, "Reading from shell failed: %s\n", strerror(errno));
                    exit(1);
                }
                if (deflateInit(&strm, Z_DEFAULT_COMPRESSION) != Z_OK) {
                    fprintf(stderr, "Failed to initialize compression settings.\n");
                    exit(1);
                }
                // Set compression parameters and pointers.
                strm.avail_in = count;
                strm.next_in = (unsigned char *)buf;
                strm.avail_out = CHUNK;
                strm.next_out = compressed;

                ret = deflate(&strm, Z_SYNC_FLUSH);
                if (ret != Z_OK && ret != Z_STREAM_END) {
                    fprintf(stderr, "Experienced error during compression.\n");
                    exit(1);
                }
                have = CHUNK - strm.avail_out;

                // Send header info containing data size for when messages stack in read buffer.
                compressed_data_size = htonl(have);
                write_check(client_fd, &compressed_data_size, sizeof(int), "socket");
                write_check(client_fd, compressed, have, "socket");
                deflateEnd(&strm);
            }

            // Monitor shell termination states.
            if (poll_list[1].revents & (POLLHUP | POLLERR)) {
                if (waitpid(pid, &status, 0) < 0) {
                    fprintf(stderr, "Failed to obtain shell exit status: %s\n", strerror(errno));
                    exit(1);
                }
                fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
                close(client_fd);  // close network socket
                break;
            }
        }

    }
}

int main(int argc, char** argv) {

    // args by index: 0 --> port no, 1 --> compress flag
    int args[2] = {0};
    process_command_line(argc, argv, args);

    // Start up server socket and connect to client.
    int client_fd = initialize_server(args[0]);

    // Start shell process and process client requests.
    process_client_requests(client_fd, args[1]);

    exit(0);
}