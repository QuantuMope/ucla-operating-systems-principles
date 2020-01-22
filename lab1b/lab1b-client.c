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


/*
 * Uses getopt_long() to process command line arguments.
 * In this case, only --shell is a valid argument.
 * Returns 1 if --shell is provided and 0 otherwise.
 */
int process_command_line(int argc, char** argv) {
    char c;
    int option_index = 0;
    int port_ok = 0;
    char* port_name;
    char* log_filename;
    static struct option long_options[] = {
            {"port", required_argument, 0, 'p'},
            {"log",  required_argument, 0, 'l'},
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
            case 'l':
                log_filename = optarg;
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
    return atoi(port_name);
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
    int sock_fd, new_sock_fd;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Client failed to initialize socket: %s\n", strerror(errno));
        exit(1);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port_no);

    // Connect to server.
    if (connect(sock_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        fprintf(stderr, "Client failed to connect to server: %s\n", strerror(errno));
        exit(1);
    }

    return sock_fd;
}


int main(int argc, char** argv) {

    // Check for --shell argument
    int port_no = process_command_line(argc, argv);

    struct termios old_tio, new_tio;
    int term_id = 0;

    // Copy old terminal settings for later restore.
    if (tcgetattr(term_id, &old_tio) < 0 ||
        tcgetattr(term_id, &new_tio) < 0){
        fprintf(stderr, "Failed to get terminal attributes: %s\n", strerror(errno));
        exit(1);
    }

    // Set new terminal settings to no echo and non-canonical.
    new_tio.c_iflag = ISTRIP;
    new_tio.c_oflag = 0;
    new_tio.c_lflag = 0;

    // Change terminal settings.
    if (tcsetattr(term_id, TCSANOW, &new_tio) < 0) {
        fprintf(stderr, "Failed to set terminal attributes: %s\n", strerror(errno));
        exit(1);
    }

    int sock_fd = initialize_client(port_no);

    // Start program.
    shell_process();

    // Restore terminal settings.
    if (tcsetattr(term_id, TCSANOW, &old_tio) < 0) {
        fprintf(stderr, "Failed to restore terminal settings: %s\r\n", strerror(errno));
    }
    fprintf(stdout, "Successfully restored terminal settings.\n");

    exit(0);
}