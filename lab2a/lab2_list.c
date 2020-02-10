#include "SortedList.h"

int num_threads = 1;
int num_iters = 1;
int opt_yield = 0;
int opt_sync = 0; // 1:m 2:s 3: c
pthread_mutex_t m_lock_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_lock_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_lock_3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_lock_4 = PTHREAD_MUTEX_INITIALIZER;
volatile int s_lock_1 = 0;
volatile int s_lock_2 = 0;
volatile int s_lock_3 = 0;
volatile int s_lock_4 = 0;
char run_type[20] = "list-";
SortedList_t* list;
SortedListElement_t** elements;
int thread_exit = 0;

void catch_segfault() {
    fprintf(stderr, "Segfault detected.\n");
    exit(1);
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
                fprintf(stderr, "Invalid argument. Valid arguments include --port=PORT_NO"
                                ", --log=FILENAME, --compress. Port argument is mandatory.\n");
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
        for (int i = 0; i < num_iters; i++) {
            SortedList_insert(list, *(elements+((i+offset)*sizeof(char*))));
        }
        if (SortedList_length(list) == -1) {
            thread_exit = 1;
            return NULL;
        }
        for (int i = 0; i < num_iters; i++) {
            if (SortedList_lookup(list, (*(elements+((i+offset)*sizeof(char*))))->key) == NULL) {
                thread_exit = 1;
                return NULL;
            }
            if (SortedList_delete(*(elements+((i+offset)*sizeof(char*)))) == 1) {
                thread_exit = 1;
                return NULL;
            }
        }
    }
    else if (opt_sync == 1) {
        pthread_mutex_lock(&m_lock_1);
        for (int i = 0; i < num_iters; i++) {
            SortedList_insert(list, *(elements+((i+offset)*sizeof(char*))));
        }
//        pthread_mutex_unlock(&m_lock_1);
//        pthread_mutex_lock(&m_lock_2);
        if (SortedList_length(list) == -1) {
            thread_exit = 1;
            return NULL;
        }
//        pthread_mutex_unlock(&m_lock_2);
//        pthread_mutex_lock(&m_lock_3);
        for (int i = 0; i < num_iters; i++) {
            if (SortedList_lookup(list, (*(elements+((i+offset)*sizeof(char*))))->key) == NULL) {
                thread_exit = 1;
                return NULL;
            }
            if (SortedList_delete(*(elements+((i+offset)*sizeof(char*)))) == 1) {
                thread_exit = 1;
                return NULL;
            }
        }
        pthread_mutex_unlock(&m_lock_1);
    }
    else {
        for (int i = 0; i < num_iters; i++) {
            SortedList_insert(list, *(elements+((i+offset)*sizeof(char*))));
        }
        if (SortedList_length(list) == -1) {
            thread_exit = 1;
            return NULL;
        }
        for (int i = 0; i < num_iters; i++) {
            if (SortedList_lookup(list, (*(elements+((i+offset)*sizeof(char*))))->key) == NULL) {
                thread_exit = 1;
                return NULL;
            }
            if (SortedList_delete(*(elements+((i+offset)*sizeof(char*)))) == 1) {
                thread_exit = 1;
                return NULL;
            }
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

    elements = malloc(num_ops * sizeof(char*));

    char* rand_keys[num_ops];
    int offsets[num_threads];
    for (int i = 0; i < num_ops; i++) {
        *(elements+(i*sizeof(char*))) = malloc(sizeof(SortedListElement_t));
        rand_keys[i] = malloc(2*sizeof(char));
        rand_keys[i][0] = 'a' + (random() % 26);
        rand_keys[i][1] = '\0';
        (*(elements+(i*sizeof(char*))))->key = rand_keys[i];
    }
    for (int i = 0; i < num_threads; i++) {
        offsets[i] = i * num_iters;
    }

    // Start timer.
    if (clock_gettime(CLOCK_REALTIME, &start) < 0) {
        fprintf(stderr, "Failed to retrieve time: %s\n", strerror(errno));
        exit(1);
    }

    // Create threads.
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, &thread_list_ops, (void*)&offsets[i]);
    }

    // Wait for all threads to complete.
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    if (thread_exit) {
        fprintf(stderr, "Doubly linked list was corrupted.\n");
        exit(2);
    }

    if (clock_gettime(CLOCK_REALTIME, &finish) < 0) {
        fprintf(stderr, "Failed to retrieve time: %s\n", strerror(errno));
        exit(1);
    }

    long total_time_ns = finish.tv_nsec - start.tv_nsec;
    int total_ops = num_threads * num_iters * 3;
    int length;
    if ((length = SortedList_length(list)) != 0) {
        fprintf(stderr, "Doubly linked list length was not zero at exit.\n");
        exit(2);
    }
    printf("Total length is %d\n", length);

    printf("%s,%d,%d,%d,%d,%ld,%ld\n", run_type, num_threads, num_iters, 1, total_ops,
            total_time_ns, total_time_ns/total_ops);

    free(list);
    for (int i = 0; i < num_ops; i++) {
        free(rand_keys[i]);
    }
//    for (int i = 0; i < num_ops; i++) {
//        free(*(elements + (i * sizeof(char *))));
//    }
    free(elements);
    exit(0);
}