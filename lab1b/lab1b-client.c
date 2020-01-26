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


/*
 * Uses getopt_long() to process command line arguments.
 * In this case, only --shell is a valid argument.
 * Returns 1 if --shell is provided and 0 otherwise.
 */
char* process_command_line(int argc, char** argv, int* args) {
    char c;
    int option_index = 0;
    int port_ok = 0;
    char* port_name;
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
                fprintf(stderr, "Invalid argument. Only valid argument is --shell\n");
                exit(1);
            case 'p':
                port_ok = 1;
                port_name = optarg;
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

    // If mandatory --port argument no specified
    if (!port_ok) {
        fprintf(stderr, "--port argument not specified. \n");
        exit(1);
    }
    args[0] = atoi(port_name);
    return log_filename;
}

/*
 * A write function that greatly aids in reducing repeated code.
 */
void write_check(int fd, void* buf, int bytes) {
    char* path = (fd == 1) ? "stdout" : "shell";
    if (write(fd, buf, bytes) < 0) {
        fprintf(stderr, "Failed to write to %s: %s\r\n", path, strerror(errno));
        exit(1);
    }
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

void initialize_terminal(int term_id, struct termios* old_tio, struct termios* new_tio) {
    // Copy old terminal settings for later restore.
    if (tcgetattr(term_id, old_tio) < 0 || tcgetattr(term_id, new_tio) < 0) {
        fprintf(stderr, "Failed to get terminal attributes: %s\n", strerror(errno));
        exit(1);
    }
    // Set new terminal settings to no echo and non-canonical.
    new_tio->c_iflag = ISTRIP;
    new_tio->c_oflag = 0;
    new_tio->c_lflag = 0;

    // Change terminal settings.
    if (tcsetattr(term_id, TCSANOW, new_tio) < 0) {
        fprintf(stderr, "Failed to set terminal attributes: %s\n", strerror(errno));
        exit(1);
    }
}

void reset_terminal(int term_id, const struct termios* settings) {
    // Restore terminal settings.
    if (tcsetattr(term_id, TCSANOW, settings) < 0) {
        fprintf(stderr, "Failed to restore terminal settings: %s\r\n", strerror(errno));
        exit(1);
    }
    fprintf(stdout, "Successfully restored terminal settings.\n");
}

void communicate_server(int server_fd) {
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
    char buf[255];

    while (1) {
        if (poll(poll_list, 2, 0) < 0) {
            fprintf(stderr, "Polling has failed: %s\r\n", strerror(errno));
            exit(1);
        }
        // Monitor keyboard input read ready.
        if (poll_list[0].revents & POLLIN) {
            if ((count = read(kin, &buf, 255)) < 0) {
                fprintf(stderr, "Reading from stdin failed: %s\r\n", strerror(errno));
                exit(1);
            }
            for (int i = 0; i < count; i++) {
                switch (buf[i]) {
                    case '\3':
                        write_check(kout, "^C\r\n", 4);
                        escape = 1;
                        break;
                    case '\4':
                        write_check(kout, "^D\r\n", 4);
                        escape = 1;
                        break;
                    case '\r':
                    case '\n':
                        write_check(kout, "\r\n", 2);
                        write_check(server_fd, "\n", 1);
                        continue;
                }
                write_check(kout, buf+i, 1);
                write_check(server_fd, buf+i, 1);
                if (escape) {
                    break;
                }
            }
        }

        // Monitor shell read ready.
        if (poll_list[1].revents & POLLIN) {
            if ((count = read(server_fd, &buf, 255)) < 0) {
                fprintf(stderr, "Reading from server failed: %s\r\n", strerror(errno));
                exit(1);
            }
            if (count == 0) {
                close(server_fd);
                fprintf(stdout, "first works\r\n");
                break;
            }
            for (int i = 0; i < count; i++) {
                if (buf[i] == '\n')  {
                    write_check(kout, "\r\n", 2);
                    continue;
                }
                write_check(kout, buf+i, 1);
            }
        }

        // Monitor server connection.
        if (poll_list[1].revents & (POLLHUP | POLLERR)) {
            fprintf(stdout, "second works\r\n");
            break;
        }
    }
}


int main(int argc, char** argv) {

    // args by index: 0 --> port no, 1 --> compress flag, 2 --> log flag
    int args[3] = {0};
    char* log_filename = process_command_line(argc, argv, args);

    struct termios old_tio, new_tio;
    int term_id = 0;

    // Initialize terminal.
    initialize_terminal(term_id, &old_tio, &new_tio);

    // Start up client.
    int server_fd = initialize_client(args[0]);

    // Communicate to server.
    communicate_server(server_fd);

    // Rest terminal.
    reset_terminal(term_id, &old_tio);

    exit(0);
}