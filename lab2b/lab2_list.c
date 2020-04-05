#include "SortedList.h"

#define BIL 1000000000ULL


int num_threads = 1;
int num_iters = 1;
int num_lists = 1;
int opt_yield = 0;
int opt_sync = 0; // 1:m 2:s
int thread_exit = 0;
char run_type[20] = "list-";
long long unsigned int* thread_lock_times;

pthread_mutex_t* m_locks;
volatile int* s_locks;
SortedList_t* headers;
SortedListElement_t* elements;

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
            {"lists",      required_argument, 0, 'l'},
            {0, 0, 0, 0}
    };
    while ((c = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        switch (c) {
            case '?':
                fprintf(stderr, "Invalid argument. Valid arguments include --t=# --i=#"
                                " --l=# --y=[idl] --s=[m|s]\n");
                exit(1);
            case 't':
                if ((num_threads = atoi(optarg)) == 0)
                    fprintf(stderr, "Invalid argument provided to --threads.\n");
                break;
            case 'i':
                if ((num_iters = atoi(optarg)) == 0)
                    fprintf(stderr, "Invalid argument provided to --iterations.\n");
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
                if ((int)strlen(optarg) != yield_opt_count)
                    fprintf(stderr, "Invalid argument provided to --yield.\n");
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
                break;
            case 'l':
                if ((num_lists = atoi(optarg)) == 0)
                    fprintf(stderr, "Invalid argument provided to --lists.\n");
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

// Get time function for code brevity
void get_time(struct timespec* record) {
    if (clock_gettime(CLOCK_MONOTONIC, record) < 0) {
        fprintf(stderr, "Failed to retrieve time: %s\n", strerror(errno));
        exit(1);
    }
}

// Locks desired mutex lock and returns the time spent waiting in ns
long long unsigned int lock_and_time(pthread_mutex_t* m_lock) {
    struct timespec start, finish;
    get_time(&start);
    pthread_mutex_lock(m_lock);
    get_time(&finish);
    return BIL * (finish.tv_sec - start.tv_sec) + finish.tv_nsec - start.tv_nsec;
}

// Locks desired spin lock and returns the time spent waiting in ns
long long unsigned int spin_lock_and_time(volatile int* s_lock) {
    struct timespec start, finish;
    get_time(&start);
    while (__sync_lock_test_and_set(s_lock, 1));
    get_time(&finish);
    return BIL * (finish.tv_sec - start.tv_sec) + finish.tv_nsec - start.tv_nsec;
}

// Thread safe get length function for partitioned lists
int get_length(int id) {
    int len = 0;
    int length = 0;
    for (int i = 0; i < num_lists; i++) {
        if (opt_sync == 1)
            thread_lock_times[id] += lock_and_time(m_locks+i);
        else if (opt_sync == 2)
            thread_lock_times[id] += spin_lock_and_time(s_locks+i);
        if ((len = SortedList_length(headers+i)) == -1) {
            thread_exit = 1;
            return -1;
        }
        if (opt_sync == 1)
            pthread_mutex_unlock(m_locks+i);
        else if (opt_sync == 2)
            __sync_lock_release(s_locks+i);
        length += len;
    }
    return length;
}

void* thread_list_ops(void* input) {
    int begin = *(int*)input;
    int end = begin + num_iters;
    int id = begin / num_iters;
    int hash;
    // Unprotected
    if (opt_sync == 0) {
        for (int i = begin; i < end; i++) {
            hash = *(elements[i].key) % num_lists;
            if (thread_exit) return NULL;
            SortedList_insert(headers+hash, elements+i);
        }
        get_length(id);
        for (int i = begin; i < end; i++) {
            if (thread_exit) return NULL;
            if (SortedList_lookup(headers+hash, elements[i].key) == NULL) {
                thread_exit = 1;
                return NULL;
            }
        }
        for (int i = begin; i < end; i++) {
            if (thread_exit) return NULL;
            if (SortedList_delete(elements+i) == 1) {
                thread_exit = 1;
                return NULL;
            }
        }
    }
    // Mutex-synchronized
    else if (opt_sync == 1) {
        for (int i = begin; i < end; i++) {
            hash = *(elements[i].key) % num_lists;
            if (thread_exit) return NULL;
            thread_lock_times[id] += lock_and_time(m_locks+hash);
            SortedList_insert(headers+hash, elements+i);
            pthread_mutex_unlock(m_locks+hash);
        }
        get_length(id);
        for (int i = begin; i < end; i++) {
            hash = *(elements[i].key) % num_lists;
            if (thread_exit) return NULL;
            thread_lock_times[id] += lock_and_time(m_locks+hash);
            if (SortedList_lookup(headers+hash, elements[i].key) == NULL) {
                thread_exit = 1;
                return NULL;
            }
            pthread_mutex_unlock(m_locks+hash);
        }
        for (int i = begin; i < end; i++) {
            hash = *(elements[i].key) % num_lists;
            if (thread_exit) return NULL;
            thread_lock_times[id] += lock_and_time(m_locks+hash);
            if (SortedList_delete(elements+i) == 1) {
                thread_exit = 1;
                return NULL;
            }
            pthread_mutex_unlock(m_locks+hash);
        }
    }
    // Spin-lock synchronized
    else {
        for (int i = begin; i < end; i++) {
            hash = *(elements[i].key) % num_lists;
            if (thread_exit) return NULL;
            thread_lock_times[id] += spin_lock_and_time(s_locks+hash);
            SortedList_insert(headers+hash, elements+i);
            __sync_lock_release(s_locks+hash);
        }
        get_length(id);
        for (int i = begin; i < end; i++) {
            hash = *(elements[i].key) % num_lists;
            if (thread_exit) return NULL;
            thread_lock_times[id] += spin_lock_and_time(s_locks+hash);
            if (SortedList_lookup(headers+hash, elements[i].key) == NULL) {
                thread_exit = 1;
                return NULL;
            }
            __sync_lock_release(s_locks+hash);
        }
        for (int i = begin; i < end; i++) {
            hash = *(elements[i].key) % num_lists;
            if (thread_exit) return NULL;
            thread_lock_times[id] += spin_lock_and_time(s_locks+hash);
            if (SortedList_delete(elements+i) == 1) {
                thread_exit = 1;
                return NULL;
            }
            __sync_lock_release(s_locks+hash);
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

    // Initialize locks (does both for simplicity)
    s_locks = malloc(num_lists * sizeof(volatile int));
    m_locks = malloc(num_lists * sizeof(pthread_mutex_t));

    // Initialize per thread lock times to zero and offsets
    thread_lock_times = malloc(num_threads*sizeof(long long unsigned int));
    int offsets[num_threads];
    for (int i = 0; i < num_threads; i++) {
        thread_lock_times[i] = 0;
        offsets[i] = i * num_iters;
    }

    // Initialize list heads and threads
    pthread_t threads[num_threads];
    headers = malloc(num_lists * sizeof(SortedList_t));
    for (int i = 0; i < num_lists; i++) {
        headers[i].prev = headers+i;
        headers[i].next = headers+i;
        headers[i].key = NULL;
        pthread_mutex_init(m_locks+i, NULL);
        s_locks[i] = 0;
    }

    // Initialize list elements
    elements = malloc(num_ops * sizeof(SortedListElement_t));
    char* rand_keys[num_ops];
    for (int i = 0; i < num_ops; i++) {
        rand_keys[i] = malloc(2*sizeof(char));
        rand_keys[i][0] = 'a' + (rand() % 26);
        rand_keys[i][1] = '\0';
        elements[i].key = rand_keys[i];
    }

    // Start timer
    get_time(&start);

    // Create threads
    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(threads+i, NULL, &thread_list_ops, (void*)(offsets+i)) != 0) {
            fprintf(stderr, "Creating thread number %d failed.\n", i);
            exit(1);
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Failed to join thread number %d.\n", i);
            exit(1);
        }
    }

    // Check for list corruption
    if (thread_exit) {
        fprintf(stderr, "Doubly linked list was corrupted.\n");
        exit(2);
    }

    // Finish timer
    get_time(&finish);

    // Calculate program completion time
    long long unsigned int total_time_ns = BIL * (finish.tv_sec - start.tv_sec) + finish.tv_nsec - start.tv_nsec;
    long long unsigned int total_ops = num_threads * num_iters * 3;

    // Calculate average lock wait time
    long long unsigned int avg_lock_time = 0;
    if (opt_sync) {
        for (int i = 0; i < num_threads; i++) {
            avg_lock_time += thread_lock_times[i];
        }
        avg_lock_time /= total_ops;
    }

    // Check to see that list length is zero
    int length = 0;
    for (int i = 0; i < num_lists; i++)
        length += SortedList_length(headers+i);
    if (length != 0) {
        fprintf(stderr, "Doubly linked list length was not zero at exit.\n");
        exit(2);
    }

    // Print output
    printf("%s,%d,%d,%d,%lld,%lld,%lld,%lld\n", run_type, num_threads, num_iters, num_lists,
            total_ops, total_time_ns, total_time_ns/total_ops, avg_lock_time);

    free(headers);
    free(elements);
    for (int i = 0; i < num_ops; i++) {
        free(rand_keys[i]);
    }
    free(thread_lock_times);

    exit(0);
}