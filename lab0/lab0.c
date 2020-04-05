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
            {"input",    required_argument, 0, 'i'},
            {"output",   required_argument, 0, 'o'},
            {"segfault", no_argument,       0, 's'},
            {"catch",    no_argument,       0, 'c'},
            {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {

        switch(c) {
            case '?':
                fprintf(stderr, "Invalid argument. Valid arguments are: "
                                "--input=filename, --output=filename, "
                                "--segfault, --catch\n");
                exit(1);
            case 'i':
                // Input option with filename argument: --input=filename
                flags[0] = 1;
                input_filename = (char*)malloc(strlen(optarg)*sizeof(char));
                strcpy(input_filename, optarg);
                break;
            case 'o':
                // Output option with filename argument: --output=filename
                flags[1] = 1;
                output_filename = (char*)malloc(strlen(optarg)*sizeof(char));
                strcpy(output_filename, optarg);
                break;
            case 's':
                // Segfault option: --segfault
                flags[2] = 1;
                break;
            case 'c':
                // Catch option: --catch
                flags[3] = 1;
                break;
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
        if (err == -1) {
            fprintf(stderr, "Reading from %s failed: %s\n", input_filename, strerror(errno));
            exit(2);
        }
        write(1, &buf, 1);
    }
    free(input_filename);

    exit(0);
}
