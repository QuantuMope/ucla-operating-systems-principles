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


int main(int argc, char** argv) {

    char c;
    int option_index = 0;
    int shell_option = 0;
    static struct option long_options[] = {
            {"shell", no_argument, 0, 's'},
            {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        switch (c) {
            case '?':
                fprintf(stderr, "Invalid argument. Only valid argument is --shell\n");
                exit(1);
            case 's':
                shell_option = 1;
                break;
        }
    }
    // Check for any other unwanted command line arguments
    if (optind < argc) {
        fprintf(stderr, "Unrecognized command line argument: %s\n", argv[optind++]);
        exit(1);
    }

    struct termios old_tio, new_tio;
    int ifd = 0; // stdin
    int ofd = 1; // stdout

    // Copy old terminal settings for later restore.
    if (tcgetattr(ifd, &old_tio) < 0 ||
        tcgetattr(ifd, &new_tio) < 0){
        fprintf(stderr, "Failed to get terminal attributes: %s\n",
                strerror(errno));
        exit(1);
    }

    // Set new terminal settings.
    new_tio.c_iflag = ISTRIP;
    new_tio.c_oflag = 0; // no processing
    new_tio.c_lflag = 0; // no processing

    // Change terminal settings.
    if (tcsetattr(ifd, TCSANOW, &new_tio) < 0) {
        fprintf(stderr, "Failed to set terminal attributes: %s\n",
                strerror(errno));
        exit(1);
    }

    int pfd1[2];  // read 3, write 4
    int pfd2[2];  // read 5, write 6
    pid_t pid;
    char* exec_arg[] = {NULL};
    if (shell_option) {
        pipe(pfd1);
        pipe(pfd2);
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Creating child process failed. \n");
            exit(1);
        }
        // Child process. Setup pipe.
        else if (pid == 0) {
            close(pfd1[1]);  // close fd 4 (write)
            close(pfd2[0]);  // close fd 5 (read)
            close(0);
            dup(pfd1[0]); // make fd 3 (read) new stdin
            close(pfd1[0]);
            close(1);
            close(2);
            dup(pfd2[1]); // make fd 6 (write) new stdout
            dup(pfd2[1]); // make fd 6 (write) new stderr
            close(pfd2[1]);

            if (execv("/bin/bash", exec_arg) < 0) {
                fprintf(stderr, "Failed to execute shell.\n");
                exit(1);
            }
        }
        // Parent process. Setup pipe.
        else {
            close(pfd1[0]); // close fd 3 (read)
            close(pfd2[1]); // close fd 6 (write)
            // writing to fd 4 will write to shell
            // reading from fd 5 is shell output
        }
    }

    struct pollfd poll_list[2];
    int ret;
    if (shell_option) {
        poll_list[0].fd = 0; // poll stdin
        poll_list[0].events = POLLIN;
        poll_list[1].fd = 5; // poll shell output
        poll_list[1].events = POLLIN;
    }

    // Shell-option
    int count, status;
    char buf[255];
    char cr_lf[] = "\r\n";
    char lf[] = "\n";
    if (shell_option) {
        while (1) {
            if ((ret = poll(poll_list, 2, 0)) < 0) {
                fprintf(stderr, "Polling has failed: %s\n", strerror(errno));
                exit(1);
            }

            if (poll_list[0].revents & POLLIN) {
                if ((count = read(ifd, &buf, 255)) != 0) {
                    if (count == -1) {
                        fprintf(stderr, "Reading from stdin failed: %s\n",
                                strerror(errno));
                        exit(1);
                    }
                    for (int i = 0; i < count; i++) {
                        if (buf[i] == '\003') {
                            if (kill(pid, SIGINT) < 0) {
                                fprintf(stderr, "Failed to kill shell process: %s\n",
                                        strerror(errno));
                            }
                        }
                        if (buf[i] == '\004') {
                            close(4); // close pipe to shell
                            fprintf(stdout, "Received Ctrl+D.\r\n");
                        }
                        if (buf[i] == '\r' || buf[0] == '\n') {
                            write(ofd, &cr_lf, 2);
                            write(4, &lf, 1);
                            continue;
                        }
                        write(ofd, &buf, count);
                        write(4, &buf, count);
                    }
                }
            }
            if (poll_list[1].revents & POLLIN) {
                if ((count = read(5, &buf, 255)) != 0) {
                    if (count == -1) {
                        fprintf(stderr, "Reading from stdin failed: %s\n",
                                strerror(errno));
                        exit(1);
                    }
                    for (int i = 0; i < count; i++) {
                        if (buf[i] == '\n') {
                            write(ofd, &cr_lf, 2);
                            continue;
                        }
                        write(ofd, buf+i, 1);
                    }
                }
            }
            if (poll_list[0].revents & POLLHUP) {
                fprintf(stderr, "POLLHUP received.");
                break;
            }
            if (poll_list[0].revents & POLLERR) {
                fprintf(stderr, "POLLERR received.");
                break;
            }
            if (poll_list[1].revents & POLLHUP ||
                poll_list[1].revents & POLLERR) {
                if (waitpid(pid, &status, 0) < 0) {
                    fprintf(stderr, "waitpid failed: %s\r\n", strerror(errno));
                    exit(1);
                }
//                int sig = status & 0x007f;
//                int sta = (status >> 8) & 0xff00;
                int sig = WTERMSIG(status);
                int sta = WEXITSTATUS(status);
                fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\r\n",
                        sig, sta);
                break;
            }
//            if (poll_list[1].revents & POLLERR) {
//                fprintf(stderr, "POLLERR received.");
//                break;
//            }
        }
    }
    // Non-shell option
    else {
        while ((count = read(ifd, &buf, 255)) != 0) {
            if (count == -1) {
                fprintf(stderr, "Reading from stdin failed: %s\r\n", strerror(errno));
                exit(2);
            }
            if (buf[0] == '\004') {
                fprintf(stdout, "Received Ctrl+D.\r\n");
                break;
            }
            if (buf[0] == '\003') {
                fprintf(stdout, "Received Ctrl+C.\r\n");
                break;
            }
            if (buf[0] == '\r' || buf[0] == '\n') {
                write(ofd, &cr_lf, 2);
                continue;
            }
            write(ofd, &buf, count);
        }
    }

    // Restore terminal settings.
    if (tcsetattr(ifd, TCSANOW, &old_tio) < 0) {
        fprintf(stderr, "Failed to restore terminal settings: %s\n",
                strerror(errno));
    }
    fprintf(stdout, "Successfully restored terminal settings.\n");

    exit(0);
}