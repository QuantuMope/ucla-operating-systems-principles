// NAME: Andrew Choi
// EMAIL: asjchoi@ucla.edu
// ID: 205348339

#include "SortedList.h"

int num_threads = 1;
int num_iters = 1;
int opt_yield = 0;
int opt_sync = 0; // 1:m 2:s 3: c
pthread_mutex_t m_lock = PTHREAD_MUTEX_INITIALIZER;
volatile int s_lock = 0;
char run_type[20] = "list-";
SortedList_t* list;
SortedListElement_t* elements;
int thread_exit = 0;

void catch_segfault() {
    fprintf(stderr, "Segfault experienced due to corrupted list.\n");
    exit(2);
}

void process_command_line(int argc, char** argv) {
    char c;
    int option_index = 0;
    int offset = 5;
    int yield_opt_count = 0;
    int y_opts[3] = {0}; // i, d, l
    static struct option long_options[] = {
            {"threads",    required_argument, 0, 't'},
            {"iterations", required_argument, 0, 'i'},
            {"yield",      required_argument, 0, 'y'},
            {"sync",       required_argument, 0, 's'},
            {0, 0, 0, 0}
    };
    while ((c = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        switch (c) {
            case '?':
                fprintf(stderr, "Invalid argument. Valid arguments include --t=# --i=#"
                                " --y=[idl] --s=[m|s]\n");
                exit(1);
            case 't':
                num_threads = atoi(optarg);
                break;
            case 'i':
                num_iters = atoi(optarg);
                break;
            case 'y':
                if (strchr(optarg, 'i') != NULL) {
                    opt_yield |= 0x01;
                    yield_opt_count++;
                    y_opts[0] = 1;
                }
                if (strchr(optarg, 'd') != NULL) {
                    opt_yield |= 0x02;
                    yield_opt_count++;
                    y_opts[1] = 1;
                }
                if (strchr(optarg, 'l') != NULL) {
                    opt_yield |= 0x04;
                    yield_opt_count++;
                    y_opts[2] = 1;
                }
                if ((int)strlen(optarg) != yield_opt_count) {
                    fprintf(stderr, "Invalid argument provided to --yield.\n");
                }
                break;
            case 's':
                if (strcmp(optarg, "m") == 0)
                    opt_sync = 1;
                else if (strcmp(optarg, "s") == 0)
                    opt_sync = 2;
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
    int flag = 0;
    if (y_opts[0]) {
        strcpy(run_type+offset, "i");
        offset++;
        flag = 1;
    }
    if (y_opts[1]) {
        strcpy(run_type+offset, "d");
        offset++;
        flag = 1;
    }
    if (y_opts[2]) {
        strcpy(run_type+offset, "l");
        offset++;
        flag = 1;
    }
    if (flag == 0) {
        strcpy(run_type+offset, "none");
        offset += 4;
    }
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
    }
}

void* thread_list_ops(void* input) {
    int offset = *((int*)input);
    if (opt_sync == 0) {
        for (int i = offset; i < num_iters + offset; i++) {
            if (thread_exit) return NULL;
            SortedList_insert(list, &elements[i]);
        }
        if (SortedList_length(list) == -1) {
            thread_exit = 1;
            return NULL;
        }
        for (int i = offset; i < num_iters + offset; i++) {
            if (thread_exit) return NULL;
            if (SortedList_lookup(list, elements[i].key) == NULL) {
                thread_exit = 1;
                return NULL;
            }
        }
        for (int i = offset; i < num_iters + offset; i++) {
            if (thread_exit) return NULL;
            if (SortedList_delete(&elements[i]) == 1) {
                thread_exit = 1;
                return NULL;
            }
        }
    }
    else if (opt_sync == 1) {
        for (int i = offset; i < num_iters + offset; i++) {
            if (thread_exit) return NULL;
            pthread_mutex_lock(&m_lock);
            SortedList_insert(list, &elements[i]);
            pthread_mutex_unlock(&m_lock);
        }
        pthread_mutex_lock(&m_lock);
        if (SortedList_length(list) == -1) {
            thread_exit = 1;
            return NULL;
        }
        pthread_mutex_unlock(&m_lock);
        for (int i = offset; i < num_iters + offset; i++) {
            if (thread_exit) return NULL;
            pthread_mutex_lock(&m_lock);
            if (SortedList_lookup(list, elements[i].key) == NULL) {
                thread_exit = 1;
                return NULL;
            }
            pthread_mutex_unlock(&m_lock);
        }
        for (int i = offset; i < num_iters + offset; i++) {
            if (thread_exit) return NULL;
            pthread_mutex_lock(&m_lock);
            if (SortedList_delete(&elements[i]) == 1) {
                thread_exit = 1;
                return NULL;
            }
            pthread_mutex_unlock(&m_lock);
        }
    }
    else {
        for (int i = offset; i < num_iters + offset; i++) {
            if (thread_exit) return NULL;
            while (__sync_lock_test_and_set(&s_lock, 1));
            SortedList_insert(list, &elements[i]);
            __sync_lock_release(&s_lock);
        }
        while (__sync_lock_test_and_set(&s_lock, 1));
        if (SortedList_length(list) == -1) {
            thread_exit = 1;
            return NULL;
        }
        __sync_lock_release(&s_lock);
        for (int i = offset; i < num_iters + offset; i++) {
            if (thread_exit) return NULL;
            while (__sync_lock_test_and_set(&s_lock, 1));
            if (SortedList_lookup(list, elements[i].key) == NULL) {
                thread_exit = 1;
                return NULL;
            }
            __sync_lock_release(&s_lock);
        }
        for (int i = offset ; i < num_iters + offset; i++) {
            if (thread_exit) return NULL;
            while (__sync_lock_test_and_set(&s_lock, 1));
            if (SortedList_delete(&elements[i]) == 1) {
                thread_exit = 1;
                return NULL;
            }
            __sync_lock_release(&s_lock);
        }
    }
    return NULL;
}

int main(int argc, char** argv) {

    // Process command line arguments.
    process_command_line(argc, argv);
    signal(SIGSEGV, catch_segfault);
    int num_ops = num_threads * num_iters;

    struct timespec start, finish;

    pthread_t threads[num_threads];

    // Init head of list.
    list = malloc(sizeof(SortedList_t));
    list->prev = list;
    list->next = list;
    list->key = NULL;

    elements = malloc(num_ops * sizeof(SortedListElement_t));

    char* rand_keys[num_ops];
    int offsets[num_threads];
    for (int i = 0; i < num_ops; i++) {
        rand_keys[i] = malloc(2*sizeof(char));
        rand_keys[i][0] = 'a' + (random() % 26);
        rand_keys[i][1] = '\0';
        elements[i].key = rand_keys[i];
    }
    for (int i = 0; i < num_threads; i++) {
        offsets[i] = i * num_iters;
    }

    // Start timer.
    if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) {
        fprintf(stderr, "Failed to retrieve time: %s\n", strerror(errno));
        exit(1);
    }

    // Create threads.
    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, &thread_list_ops, (void*)&offsets[i]) != 0) {
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

    if (thread_exit) {
        fprintf(stderr, "Doubly linked list was corrupted.\n");
        exit(2);
    }

    if (clock_gettime(CLOCK_MONOTONIC, &finish) < 0) {
        fprintf(stderr, "Failed to retrieve time: %s\n", strerror(errno));
        exit(1);
    }

    long long unsigned int total_time_ns = 1000000000 * (finish.tv_sec - start.tv_sec) + finish.tv_nsec - start.tv_nsec;
    long long unsigned int total_ops = num_threads * num_iters * 3;
    if (SortedList_length(list) != 0) {
        fprintf(stderr, "Doubly linked list length was not zero at exit.\n");
        exit(2);
    }
    printf("%s,%d,%d,%d,%lld,%lld,%lld\n", run_type, num_threads, num_iters, 1, total_ops,
            total_time_ns, total_time_ns/total_ops);

    free(list);
    free(elements);
    for (int i = 0; i < num_ops; i++) {
        free(rand_keys[i]);
    }

    exit(0);
}