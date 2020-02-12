// NAME: Andrew Choi
// EMAIL: asjchoi@ucla.edu
// ID: 205348339

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>

int num_threads = 1;
int num_iters = 1;
int opt_yield = 0;
int opt_sync = 0; // 1:m 2:s 3: c
long long counter = 0;
pthread_mutex_t m_lock = PTHREAD_MUTEX_INITIALIZER;
volatile int s_lock = 0;
char run_type[20] = "add";

void process_command_line(int argc, char** argv) {
    char c;
    int option_index = 0;
    static struct option long_options[] = {
            {"threads",    required_argument, 0, 't'},
            {"iterations", required_argument, 0, 'i'},
            {"yield",      no_argument      , 0, 'y'},
            {"sync",       required_argument, 0, 's'},
            {0, 0, 0, 0}
    };
    while ((c = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        switch (c) {
            case '?':
                fprintf(stderr, "Invalid argument. Valid arguments include --t=# --i=#"
                                " --y --s=[m|s|c]\n");
                exit(1);
            case 't':
                num_threads = atoi(optarg);
                break;
            case 'i':
                num_iters = atoi(optarg);
                break;
            case 'y':
                opt_yield = 1;
                strcpy(run_type+3, "-yield");
                break;
            case 's':
                if (strcmp(optarg, "m") == 0)
                    opt_sync = 1;
                else if (strcmp(optarg, "s") == 0)
                    opt_sync = 2;
                else if (strcmp(optarg, "c") == 0)
                    opt_sync = 3;
                else {
                    fprintf(stderr, "Invalid argument provided to --sync.\n");
                    exit(1);
                }
        }
    }
    // Check for any other unwanted command line arguments
    if (optind < argc) {
        fprintf(stderr, "Unrecognized command line argument: %s\n", argv[optind++]);
        exit(1);
    }
    int offset = (opt_yield) ? 9 : 3;
    switch (opt_sync) {
        case 0:
            strcpy(run_type+offset, "-none");
            break;
        case 1:
            strcpy(run_type+offset, "-m");
            break;
        case 2:
            strcpy(run_type+offset, "-s");
            break;
        case 3:
            strcpy(run_type+offset, "-c");
    }
}

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (opt_yield)
        sched_yield();
    *pointer = sum;
}

void* thread_add() {
    if (opt_sync == 0) {
        for (int i = 0; i < num_iters; i++) {
            add(&counter, 1);
        }
        for (int i = 0; i < num_iters; i++) {
            add(&counter, -1);
        }
    }
    else if (opt_sync == 1) {
        for (int i = 0; i < num_iters; i++) {
            pthread_mutex_lock(&m_lock);
            add(&counter, 1);
            pthread_mutex_unlock(&m_lock);
        }
        for (int i = 0; i < num_iters; i++) {
            pthread_mutex_lock(&m_lock);
            add(&counter, -1);
            pthread_mutex_unlock(&m_lock);
        }
    }
    else if (opt_sync == 2) {
        for (int i = 0; i < num_iters; i++) {
            while (__sync_lock_test_and_set(&s_lock, 1));
            add(&counter, 1);
            __sync_lock_release(&s_lock);
        }
        for (int i = 0; i < num_iters; i++) {
            while (__sync_lock_test_and_set(&s_lock, 1));
            add(&counter, -1);
            __sync_lock_release(&s_lock);
        }
    }
    else {
        long long old, new;
        int val = 1;
        for (int i = 0; i < 2*num_iters; i++) {
            while (1) {
                old = counter;
                new = old + val;
                if (opt_yield)
                    sched_yield();
                if (__sync_val_compare_and_swap(&counter, old, new) == old)
                    break;
            }
            if (i == (num_iters - 1))
                val = -1;
        }
    }
    return NULL;
}


int main(int argc, char** argv) {
    // Process command line arguments.
    process_command_line(argc, argv);

    struct timespec start, finish;

    pthread_t threads[num_threads];

    if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) {
        fprintf(stderr, "Failed to retrieve time: %s\n", strerror(errno));
        exit(1);
    }

    // Create threads.
    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, &thread_add, NULL) != 0) {
            fprintf(stderr, "Creating thread number %d failed.\n", i);
            exit(1);
        }
    }

    // Wait for all threads to complete.
    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Failed to join thread number %d.\n", i);
            exit(1);
        }
    }

    if (clock_gettime(CLOCK_MONOTONIC, &finish) < 0) {
        fprintf(stderr, "Failed to retrieve time: %s\n", strerror(errno));
        exit(1);
    }

    long long unsigned int total_time_ns = 1000000000 * (finish.tv_sec - start.tv_sec) + finish.tv_nsec - start.tv_nsec;
    long long unsigned int total_ops = num_threads * num_iters * 2;

    printf("%s,%d,%d,%lld,%lld,%lld,%lld\n", run_type, num_threads, num_iters, total_ops,
            total_time_ns, total_time_ns/total_ops, counter);
    if (counter != 0)
        exit(2);
    exit(0);
}