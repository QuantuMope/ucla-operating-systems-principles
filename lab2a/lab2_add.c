#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

void process_command_line(int argc, char** argv, int* arg_values) {
    static struct option long_options[] = {
            {"threads",    required_argument, 0, 't'},
            {"iterations", required_argument, 0, 'i'},
            {0, 0, 0, 0}
    };
    while ((c = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        switch (c) {
            case '?':
                fprintf(stderr, "Invalid argument. Valid arguments include --port=PORT_NO"
                                ", --log=FILENAME, --compress. Port argument is mandatory.\n");
                exit(1);
            case 't':
                arg_values[0] = atoi(optarg);
                break;
            case 'i':
                arg_values[1] = atoi(optarg);
        }
    }
    // Check for any other unwanted command line arguments
    if (optind < argc) {
        fprintf(stderr, "Unrecognized command line argument: %s\n", argv[optind++]);
        exit(1);
    }
}

void *add(long long* pointer, long long value) {
    long long sum = *pointer + value;
    *pointer = sum;
}


int main(int argc, char** argv) {
    // Process command line arguments.
    int cmd_args[2] = {1};
    process_command_line(argc, argv, cmd_args);
    int num_threads, num_iters = cmd_args[0], cmd_args[1];

    struct timespec start, finish;

    long long counter = 0;
    pthread_t threads[num_threads];
    if (clock_gettime(CLOCK_REALTIME, &start) < 0) {
        fprintf(stderr, "Failed to retrieve time: %s\n", strerror(errno));
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, add);
    }
    // Wait for all threads to complete.
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    if (clock_gettime(CLOCK_REALTIME, &finish) < 0) {
        fprintf(stderr, "Failed to retrieve time: %s\n", strerror(errno));
    }
    long seconds = finish.tv_sec - start.tv_sec;
    long ns = finish.tv_nsec - start.tv_nsec;
//    fprinf(stdout, "add-none,%d,%d,%d,%d,%d", num_threads, num_iters);

    exit(0);
}