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


/*
 * Uses getopt_long() to process command line arguments.
 * In this case, only --shell is a valid argument.
 * Returns 1 if --shell is provided and 0 otherwise.
 */
int process_command_line(int argc, char** argv) {
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
    return shell_option;
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

// --shell
void shell_process() {
    // Initiate pipes from process to shell.
    int pfd1[2];  // read 3, write 4
    int pfd2[2];  // read 5, write 6
    if (pipe(pfd1) < 0 || pipe(pfd2) < 0) {
        fprintf(stderr, "Creating pipes failed: %s\r\n", strerror(errno));
        exit(1);
    }

    // Initialize child process.
    pid_t pid;
    if ((pid = fork()) < 0) {
        fprintf(stderr, "Forking child process failed: %s\r\n", strerror(errno));
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
            fprintf(stderr, "Failed to execute shell: %s\r\n", strerror(errno));
            exit(1);
        }
    }
    // Parent process. Setup proper pipe connections.
    else {
        close(pfd1[0]); // close fd 3 (read)
        close(pfd2[1]); // close fd 6 (write)
    }

    // Set up polling for keyboard and shell input.
    struct pollfd poll_list[2];
    poll_list[0].fd = 0; // poll stdin
    poll_list[0].events = POLLIN;
    poll_list[1].fd = 5; // poll shell output
    poll_list[1].events = POLLIN;

    // File descriptors.
    int kin  = 0;
    int kout = 1;
    int sin  = 4; // writing to fd 4 will write to shell
    int sout = 5; // reading from fd 5 is shell output

    int count, status;
    int escape = 0;
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
                        if (kill(pid, SIGINT) < 0) {
                            fprintf(stderr, "Failed to kill shell process: %s\r\n", strerror(errno));
                            exit(1);
                        }
                        escape = 1;
                        break;
                    case '\4':
                        write_check(kout, "^D\r\n", 4);
                        close(sin); // close pipe to shell
                        escape = 1;
                        break;
                    case '\r':
                    case '\n':
                        write_check(kout, "\r\n", 2);
                        write_check(sin, "\n", 1);
                        continue;
                }
                if (escape)
                    break;
                write_check(kout, buf+i, 1);
                write_check(sin, buf+i, 1);
            }
        }

        // Monitor shell read ready.
        if (poll_list[1].revents & POLLIN) {
            if ((count = read(sout, &buf, 255)) < 0) {
                fprintf(stderr, "Reading from shell failed: %s\r\n", strerror(errno));
                exit(1);
            }
            for (int i = 0; i < count; i++) {
                if (buf[i] == '\n') {
                    write_check(kout, "\r\n", 2);
                    continue;
                }
                write_check(kout, buf+i, 1);
            }
        }

        // Monitor shell termination states.
        if (poll_list[1].revents & POLLHUP ||
            poll_list[1].revents & POLLERR) {
            if (waitpid(pid, &status, 0) < 0) {
                fprintf(stderr, "Failed to obtain shell exit status: %s\r\n", strerror(errno));
                exit(1);
            }
            fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\r\n", WTERMSIG(status), WEXITSTATUS(status));
            break;
        }
    }
}

void non_shell_process() {
    int count;
    char buf[255];

    int ifd = 0;
    int ofd = 1;
    int escape = 0;

    while (1) {
        if ((count = read(ifd, &buf, 255)) < 0) {
            fprintf(stderr, "Reading from stdin failed: %s\r\n", strerror(errno));
            exit(1);
        }
        for (int i = 0; i < count; i++) {
            if (buf[i] == '\3') {
                write_check(ofd, "^C\r\n", 4);
                escape = 1;
                break;
            }
            if (buf[i] == '\4') {
                write_check(ofd, "^D\r\n", 4);
                escape = 1;
                break;
            }
            if (buf[i] == '\r' || buf[i] == '\n') {
                write_check(ofd, "\r\n", 2);
                continue;
            }
            write_check(ofd, buf+i, 1);
        }
        if (escape)
            break;
    }
}

int main(int argc, char** argv) {

    // Check for --shell argument
    int shell = process_command_line(argc, argv);

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

    // Start program.
    if (shell) {
        shell_process();
    }
    else {
        non_shell_process();
    }

    // Restore terminal settings.
    if (tcsetattr(term_id, TCSANOW, &old_tio) < 0) {
        fprintf(stderr, "Failed to restore terminal settings: %s\r\n", strerror(errno));
    }
    fprintf(stdout, "Successfully restored terminal settings.\n");

    exit(0);
}