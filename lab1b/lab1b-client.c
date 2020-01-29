// NAME: Andrew Choi
// EMAIL: asjchoi@ucla.edu
// ID: 205348339

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
#include <sys/socket.h>
#include <netinet/in.h>
#include "zlib.h"

#define CHUNK 1024

/*
 * Uses getopt_long() to process command line arguments.
 * In this case, only --shell is a valid argument.
 * Returns 1 if --shell is provided and 0 otherwise.
 */
char* process_command_line(int argc, char** argv, int* args) {
    char c;
    int option_index = 0;
    char* log_filename;
    static struct option long_options[] = {
            {"port",     required_argument, 0, 'p'},
            {"compress", no_argument,       0, 'c'},
            {"log",      required_argument, 0, 'l'},
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
                break;
            case 'l':
                log_filename = (char*)malloc(sizeof(char) * strlen(optarg)+1);
                if (log_filename != NULL) {
                    strcpy(log_filename, optarg);
                }
                args[2] = 1;
        }
    }
    // Check for any other unwanted command line arguments
    if (optind < argc) {
        fprintf(stderr, "Unrecognized command line argument: %s\n", argv[optind++]);
        exit(1);
    }

    // Check to see that port argument was provided.
    if (!args[0]) {
        fprintf(stderr, "--port argument must be provided. \n");
        exit(1);
    }
    return log_filename;
}

/*
 * A write function that greatly aids in reducing repeated code.
 */
void write_check(int fd, void* buf, int bytes, char* path) {
    if (write(fd, buf, bytes) < 0) {
        fprintf(stderr, "Failed to write to %s: %s\r\n", path, strerror(errno));
        exit(1);
    }
}

void initialize_terminal(struct termios* old_tio, struct termios* new_tio) {
    // Copy old terminal settings for later restore.
    if (tcgetattr(0, old_tio) < 0 || tcgetattr(0, new_tio) < 0) {
        fprintf(stderr, "Failed to get terminal attributes: %s\n", strerror(errno));
        exit(1);
    }
    // Set new terminal settings to no echo and non-canonical.
    new_tio->c_iflag = ISTRIP;
    new_tio->c_oflag = 0;
    new_tio->c_lflag = 0;

    // Change terminal settings.
    if (tcsetattr(0, TCSANOW, new_tio) < 0) {
        fprintf(stderr, "Failed to set terminal attributes: %s\n", strerror(errno));
        exit(1);
    }
}

void reset_terminal(const struct termios* settings) {
    // Restore terminal settings.
    if (tcsetattr(0, TCSANOW, settings) < 0) {
        fprintf(stderr, "Failed to restore terminal settings: %s\r\n", strerror(errno));
        exit(1);
    }
    fprintf(stdout, "Successfully restored terminal settings.\n");
}


int initialize_client(int port_no) {
    // Initialize TCP socket.
    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Client failed to initialize socket: %s\r\n", strerror(errno));
        exit(1);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port_no);

    // Connect to server.
    fprintf(stdout, "Client attempting to connect to server.\r\n");
    if (connect(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        fprintf(stderr, "Client failed to connect to server: %s\r\n", strerror(errno));
        exit(1);
    }
    fprintf(stdout, "Client successfully connected to server.\r\n");
    return server_fd;
}

void log_to_file(int log_fd, int count, char* buf, char flag) {
    char tmp_num[10];
    sprintf(tmp_num, "%d", count);
    if (flag == 'r')
        write_check(log_fd, "RECEIVED ", 9, "log file");
    else
        write_check(log_fd, "SENT ", 5, "log file");
    write_check(log_fd, tmp_num, strlen(tmp_num), "log file");
    write_check(log_fd, " bytes: ", 8, "log file");
    write_check(log_fd, buf, count, "log file");
    write_check(log_fd, "\n", 1, "log file");
}

void communicate_server(int server_fd, int* args, char* log_filename) {
    // Setup polling between stdin (keyboard) and socket (server).
    struct pollfd poll_list[2];
    poll_list[0].fd = 0; // stdin
    poll_list[0].events = POLLIN;
    poll_list[1].fd = server_fd;
    poll_list[1].events = POLLIN;

    // File descriptors
    int kin = 0;
    int kout = 1;

    int escape = 0;
    int count;
    char buf[512];

    // Setup log file if option enabled.
    int log_fd;
    if (args[2]) {
        if ((log_fd = creat(log_filename, 0666)) < 0) {
            fprintf(stderr, "Failed to initialize log file: %s\r\n", strerror(errno));
            exit(1);
        }
        free(log_filename);
    }

    if (!args[1]) {
        // No compression.
        while (1) {
            if (poll(poll_list, 2, 0) < 0) {
                fprintf(stderr, "Polling has failed: %s\r\n", strerror(errno));
                exit(1);
            }
            // Monitor keyboard input read ready.
            if (poll_list[0].revents & POLLIN) {
                if ((count = read(kin, &buf, 512)) < 0) {
                    fprintf(stderr, "Reading from stdin failed: %s\r\n", strerror(errno));
                    exit(1);
                }
                if (args[2]) {
                    log_to_file(log_fd, count, buf, 's');
                }
                for (int i = 0; i < count; i++) {
                    switch (buf[i]) {
                        case '\3':
                            write_check(kout, "^C\r\n", 4, "stdout");
                            escape = 1;
                            break;
                        case '\4':
                            write_check(kout, "^D\r\n", 4, "stdout");
                            escape = 1;
                            break;
                        case '\r':
                        case '\n':
                            write_check(kout, "\r\n", 2, "stdout");
                            write_check(server_fd, "\n", 1, "socket");
                            continue;
                    }
                    write_check(kout, buf+i, 1, "stdout");
                    write_check(server_fd, buf+i, 1, "socket");
                    if (escape) {
                        break;
                    }
                }
            }

            // Monitor shell read ready.
            if (poll_list[1].revents & POLLIN) {
                if ((count = read(server_fd, &buf, 512)) < 0) {
                    fprintf(stderr, "Reading from server failed: %s\r\n", strerror(errno));
                    exit(1);
                }
                if (count == 0) {
                    close(server_fd);
                    break;
                }
                if (args[2]) {
                    log_to_file(log_fd, count, buf, 'r');
                }
                for (int i = 0; i < count; i++) {
                    if (buf[i] == '\n')  {
                        write_check(kout, "\r\n", 2, "stdout");
                        continue;
                    }
                    write_check(kout, buf+i, 1, "stdout");
                }
            }

            // Monitor server connection.
            if (poll_list[1].revents & (POLLHUP | POLLERR)) {
                close(server_fd);
                break;
            }
        }
    }
    else {
        // Compression.

        // Initialize zlib stream.
        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;

        int have, ret;
        unsigned char compressed[CHUNK];
        unsigned char translated[CHUNK];
        int compressed_data_size;

        while (1) {
            if (poll(poll_list, 2, 0) < 0) {
                fprintf(stderr, "Polling has failed: %s\r\n", strerror(errno));
                exit(1);
            }
            // Monitor keyboard input read ready.
            if (poll_list[0].revents & POLLIN) {
                if ((count = read(kin, &buf, 512)) < 0) {
                    fprintf(stderr, "Reading from stdin failed: %s\r\n", strerror(errno));
                    exit(1);
                }
                for (int i = 0; i < count; i++) {
                    switch (buf[i]) {
                        case '\3':
                            write_check(kout, "^C\r\n", 4, "stdout");
                            escape = 1;
                            break;
                        case '\4':
                            write_check(kout, "^D\r\n", 4, "stdout");
                            escape = 1;
                            break;
                        case '\r':
                        case '\n':
                            write_check(kout, "\r\n", 2, "stdout");
                            buf[i] = '\n';
                            continue;
                    }
                    write_check(kout, buf+i, 1, "stdout");
                    if (escape) {
                        break;
                    }
                }
                if (deflateInit(&strm, Z_DEFAULT_COMPRESSION) != Z_OK) {
                    fprintf(stderr, "Failed to initialize compression settings.\r\n");
                    exit(1);
                }
                // Set compression parameters and pointers.
                strm.avail_in = count;
                strm.next_in = (unsigned char*)buf;
                strm.avail_out = CHUNK;
                strm.next_out = compressed;

                ret = deflate(&strm, Z_SYNC_FLUSH);
                if (ret != Z_OK && ret != Z_STREAM_END) {
                    fprintf(stderr, "Experienced error during compression.\r\n");
                    exit(1);
                }
                have = CHUNK - strm.avail_out;

                // Send header info containing data size for when messages stack in read buffer.
                compressed_data_size = htonl(have);
                write_check(server_fd, &compressed_data_size, sizeof(int), "socket");
                write_check(server_fd, compressed, have, "socket");

                // Log if argument is specified.
                if (args[2]) {
                    log_to_file(log_fd, have, (char*)compressed, 's');
                }
                deflateEnd(&strm);
            }

            // Monitor shell read ready.
            if (poll_list[1].revents & POLLIN) {
                if (read(server_fd, &compressed_data_size, sizeof(int)) < 0) {
                    fprintf(stderr, "Reading from server failed: %s\r\n", strerror(errno));
                    exit(1);
                }
                if ((count = read(server_fd, &buf, ntohl(compressed_data_size))) < 0) {
                    fprintf(stderr, "Reading from server failed: %s\r\n", strerror(errno));
                    exit(1);
                }
                if (count == 0) {
                    close(server_fd);
                    break;
                }
                if (inflateInit(&strm) != Z_OK) {
                    fprintf(stderr, "Failed to initialize decompression settings.\r\n");
                    exit(1);
                }
                // Set decompression parameters and pointers.
                strm.avail_in = count;
                strm.next_in = (unsigned char*)buf;
                strm.avail_out = CHUNK;
                strm.next_out = translated;

                ret = inflate(&strm, Z_SYNC_FLUSH);
                if (ret != Z_OK && ret != Z_STREAM_END) {
                    fprintf(stderr, "Experienced error during decompression.\r\n");
                    exit(1);
                }
                have = CHUNK - strm.avail_out;

                for (int i = 0; i < have; i++) {
                    if (translated[i] == '\n')  {
                        write_check(kout, "\r\n", 2, "stdout");
                        continue;
                    }
                    write_check(kout, translated+i, 1, "stdout");
                }
                // Log if argument is specified.
                if (args[2]) {
                    log_to_file(log_fd, count, buf, 'r');
                }
                inflateEnd(&strm);
            }
            // Monitor server connection.
            if (poll_list[1].revents & (POLLHUP | POLLERR)) {
                close(server_fd);
                break;
            }
        }
    }
}


int main(int argc, char** argv) {

    // args by index: 0 --> port no, 1 --> compress flag, 2 --> log flag
    int args[3] = {0};
    char* log_filename = process_command_line(argc, argv, args);

    struct termios old_tio, new_tio;

    // Initialize terminal.
    initialize_terminal(&old_tio, &new_tio);

    // Start up client.
    int server_fd = initialize_client(args[0]);

    // Communicate to server.
    communicate_server(server_fd, args, log_filename);

    // Reset terminal.
    reset_terminal(&old_tio);

    exit(0);
}