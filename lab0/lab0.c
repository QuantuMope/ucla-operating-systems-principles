// NAME: Andrew Choi
// EMAIL: asjchoi@ucla.edu
// ID: 205348339

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

void handle_sigsegv()
{
    fprintf(stderr, "Caught and received SIGSEGV.\n");
    exit(4);
}

void cause_segfault()
{
    char* seg_fault_force = NULL;
    *seg_fault_force = 'x';
}

int main(int argc, char** argv)
{
    int c;
    int ifd;
    int ofd;
    int option_index = 0;
    char* input_filename;
    char* output_filename;
    int flags[4] = {0};  // argument flags

    static struct option long_options[] = {
            {"input",    required_argument, 0, 0},
            {"output",   required_argument, 0, 0},
            {"segfault", no_argument,       0, 0},
            {"catch",    no_argument,       0, 0},
            {0, 0, 0, 0}
    };

    while (1) {

        c = getopt_long(argc, argv, "", long_options, &option_index);

        if (c == '?') {
            fprintf(stderr, "Valid arguments are: "
                            "--input=filename, --output=filename, "
                            "--segfault, --catch\n");
            exit(1);
        }

        if (c == -1)
            break;

        // Input option with filename argument: --input=filename
        if (!strcmp(long_options[option_index].name, "input")) {
            flags[0] = 1;
            input_filename = (char*)malloc(strlen(optarg)*sizeof(char));
            strcpy(input_filename, optarg);
            continue;
        }

        // Output option with filename argument: --output=filename
        if (!strcmp(long_options[option_index].name, "output")) {
            flags[1] = 1;
            output_filename = (char*)malloc(strlen(optarg)*sizeof(char));
            strcpy(output_filename, optarg);
            continue;
        }

        // Segfault option: --segfault
        if (!strcmp(long_options[option_index].name, "segfault")) {
            flags[2] = 1;
            continue;
        }

        // Catch option: --catch
        if (!strcmp(long_options[option_index].name, "catch")) {
            flags[3] = 1;
            continue;
        }

    }

    // Check for any other unwanted command line arguments
    if (optind < argc) {
        fprintf(stderr, "Unrecognized command line argument: %s\n", argv[optind++]);
        exit(1);
    }

    // 1. File redirection
    // Input file redirection
    if (flags[0]) {
        ifd = open(input_filename, O_RDONLY);
        // Redirect stdin to file
        if (ifd >= 0) {
            close(0);
            dup(ifd);
            close(ifd);
        } else {
            fprintf(stderr, "--input failed to open %s: %s\n", input_filename, strerror(errno));
            exit(2);
        }
        free(input_filename);
    }

    // Output file redirection
    if (flags[1]) {
        ofd = creat(output_filename, 0666);
        if (ofd >= 0) {
            close(1);
            dup(ofd);
            close(ofd);
        } else {
            fprintf(stderr, "--output failed to create/write to file %s: %s\n", output_filename, strerror(errno));
            exit(3);
        }
        free(output_filename);
    }
    // 2. Register signal handler
    if (flags[3]) {
        if (signal(SIGSEGV, handle_sigsegv) == SIG_ERR)
            fprintf(stderr, "--catch failed: %s\n", strerror(errno));
    }

    // 3. Cause segfault
    if (flags[2])
        cause_segfault();

    // 4. Copy stdin to stdout if no segfault
    int err;
    char buf[1];
    while ((err = read(0, &buf, 1)) != 0) {
        if (err == -1)
            fprintf(stderr, "Reading from stdin failed: %s\n", strerror(errno));
        write(1, &buf, 1);
    }

    exit(0);
}
