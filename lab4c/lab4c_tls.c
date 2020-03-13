// NAME: Andrew Choi
// EMAIL: asjchoi@ucla.edu
// ID: 205348339

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <mraa/aio.h>
#include <mraa/gpio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


int run_flag = 1;
const int B  = 4275;
const int R0 = 100000;
FILE* log_file = NULL;

// Shutdown code. Enabled from OFF command
void turn_off() {
    time_t raw_time;
    struct tm* curr_time;
    time(&raw_time);
    curr_time = localtime(&raw_time);
    fprintf(log_file, "%02d:%02d:%02d SHUTDOWN\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec);
    run_flag = 0;
}

void key_interrupt(int sig) {
    if (sig == SIGINT)
        run_flag = 0;
}

// Convert sensor reading into (C or F) temperature reading
float convert_read_to_temp(int read, char scale) {
    float R = 1023.0/read-1.0;
    R = R0 * R;
    float temp = 1.0 / (log(R/R0)/B+1/298.15)-273.15;
    return (scale == 'F') ? 1.8 * temp + 32 : temp;
}

int initialize_client(int port_no, char* host) {
    // Initialize TCP socket.
    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Client failed to initialize socket: %s\n", strerror(errno));
        exit(2);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port_no);
    struct hostent *h;
    if ((h = gethostbyname(host)) == NULL) {
        fprintf(stderr, "Invalid hostname: %s\n", strerror(h_errno));
        exit(1);
    }
    memcpy((char*)&address.sin_addr.s_addr, h->h_addr, h->h_length);

    // Connect to server.
    printf("Client attempting to connect to server.\n");
    if (connect(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        fprintf(stderr, "Client failed to connect to server: %s\n", strerror(errno));
        exit(2);
    }
    printf("Client successfully connected to server.\n");
    return server_fd;
}

void ssl_write_check(SSL* ssl, const void* buf, int num) {
    if (SSL_write(ssl, buf, num) <= 0) {
        fprintf(stderr, "Writing to server with SSL encryption failed\n");
        exit(2);
    }
}


int main(int argc, char** argv) {
    // Process command line arguments
    char c;
    int option_index = 0;
    int period = 1;
    char scale = 'F';
    char* id = NULL;
    char* host = NULL;
    int port_no;
    static struct option long_options[] = {
            {"period", required_argument, 0, 'p'},
            {"scale",  required_argument, 0, 's'},
            {"log",    required_argument, 0, 'l'},
            {"id",     required_argument, 0, 'i'},
            {"host",   required_argument, 0, 'h'},
            {0, 0, 0, 0}
    };
    while ((c = getopt_long(argc, argv, "", long_options, &option_index)) != (char)-1) {
        switch (c) {
            case '?':
                fprintf(stderr, "Invalid argument. Valid arguments are --period=#"
                                " --scale=[C|F] --log=FILENAME\n");
                exit(1);
            case 'p':
                if ((period = atoi(optarg)) == 0) {
                    fprintf(stderr, "Invalid argument provided to --period\n");
                    exit(1);
                }
                break;
            case 's':
                scale = optarg[0];
                if (!(scale == 'F' || scale == 'C') || strlen(optarg) != 1) {
                    fprintf(stderr, "Invalid argument provided to --scale\n");
                    exit(1);
                }
                break;
            case 'l':
                if ((log_file = fopen(optarg, "w+")) == NULL) {
                    fprintf(stderr, "Failed to initialize file pointer: %s\n", strerror(errno));
                    exit(1);
                }
                break;
            case 'i':
                if (strlen(optarg) != 9) {
                    fprintf(stderr, "ID should be a 9 digit number\n");
                    exit(1);
                }
                id = optarg;
                break;
            case 'h':
                host = optarg;
                break;
        }
    }
    if (log_file == NULL || host == NULL || id == NULL) {
        fprintf(stderr, "One or more mandatory parameters were not provided\n"
                        "Please provide --id=9-DIGIT-#, --host=NAME/ADDRESS, --log=FILENAME\n");
        exit(1);
    }

    // Last argument should be port.
    if (optind < argc)
        port_no = atoi(argv[optind++]);
    else {
        fprintf(stderr, "Port number was not provided\n");
        exit(1);
    }

    // Check for any unwanted command line arguments.
    if (optind < argc) {
        fprintf(stderr, "Unrecognized command line argument: %s\n", argv[optind++]);
        exit(1);
    }
    // Allow for Ctrl+C keyboard interrupt
    signal(SIGINT, key_interrupt);

    // Initialize sensors
    mraa_aio_context temp_sensor;
    temp_sensor = mraa_aio_init(1);

    // Check for successful sensor connections
    if (temp_sensor == NULL) {
        fprintf(stderr, "Error initializing temperature sensor\n");
        exit(1);
    }

    // Connect to server
    int server_fd = initialize_client(port_no, host);

    // Initialize SSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    // Setup for SSL connection
    SSL_CTX *ctx;
    SSL *ssl;
    if ((ctx = SSL_CTX_new(TLSv1_client_method())) == NULL) {
        fprintf(stderr, "Failed to create SSL context\n");
        exit(2);
    }
    if ((ssl = SSL_new(ctx)) == NULL) {
        fprintf(stderr, "Failed to create SSL connection data structure.\n");
        exit(2);
    }
    if (SSL_set_fd(ssl, server_fd) == 0) {
        fprintf(stderr, "Failed to set socket file descriptor for SSL\n");
        exit(2);
    }

    // Attempt to connect
    if (SSL_connect(ssl) <= 0) {
        fprintf(stderr, "Failed to connect to server with SSL encryption\n");
        exit(2);
    }

    char ssl_buffer[128] = {'\0'};
    // Send ID
    fprintf(log_file, "ID=%s\n", id);
    sprintf(ssl_buffer, "ID=%s\n", id);
    ssl_write_check(ssl, ssl_buffer, strlen(ssl_buffer));
    memset(ssl_buffer, '\0', 128);

    // Poll the server
    struct pollfd poll_list;
    poll_list.fd = server_fd;
    poll_list.events = POLLIN;

    time_t raw_time;
    struct tm* curr_time;
    struct timespec start, finish;
    char read_buf[512] = {0};
    char command_buf[512] = {0};
    int length = 0;
    int add = 0;
    int offset = 0;
    int count;
    int generate_reports = 1;
    int i;  // for loop variable
    float temp;

    // Perform one read right away and start interval timer
    time(&raw_time);
    curr_time = localtime(&raw_time);
    temp = convert_read_to_temp(mraa_aio_read(temp_sensor), scale);
    fprintf(log_file, "%02d:%02d:%02d %0.1f\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec, temp);
    sprintf(ssl_buffer, "%02d:%02d:%02d %0.1f\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec, temp);
    ssl_write_check(ssl, ssl_buffer, strlen(ssl_buffer));
    memset(ssl_buffer, '\0', 128);
    clock_gettime(CLOCK_MONOTONIC, &start);

    while (run_flag) {
        if (poll(&poll_list, 1, 0) < 0) {
            fprintf(stderr, "Polling has failed: %s\n", strerror(errno));
            exit(2);
        }

        if (poll_list.revents & POLLIN) {
            if ((count = SSL_read(ssl, &read_buf, 512)) < 0) {
                fprintf(stderr, "Reading from server with SSL encryption failed\n");
                exit(2);
            }
            add = count;
            for (i = 0; i < count; i++) {
                if (read_buf[i] == '\n') {
                    char period_check[8] = {0};
                    char log_check[5] = {0};
                    strncpy(period_check, command_buf, 7);
                    strncpy(log_check, command_buf, 4);

                    // Check for valid commands
                    if (strcmp(command_buf, "SCALE=F") == 0) {
                        scale = 'F';
                    }
                    else if (strcmp(command_buf, "SCALE=C") == 0) {
                        scale = 'C';
                    }
                    else if (strcmp(period_check, "PERIOD=") == 0) {
                        period = atoi(command_buf+7);
                    }
                    else if (strcmp(command_buf, "STOP") == 0) {
                        generate_reports = 0;
                    }
                    else if (strcmp(command_buf, "START") == 0) {
                        generate_reports = 1;
                    }
                    else if (strcmp(log_check, "LOG ") == 0) {
                        ; // Will need for lab4c
                    }
                    else if (strcmp(command_buf, "OFF") == 0) {
                        fprintf(log_file, "OFF\n");
                        turn_off();
                        break;
                    }

                    // Log all commands valid or invalid if enabled
                    fprintf(log_file, "%s\n", command_buf);

                    // Clear and reset command buffer after a \n
                    length = 0;
                    memset(command_buf, '\0', 512);

                    // After a command, set offset for correct indexing if read buffer has multiple commands
                    offset = i + 1;

                    // Reduce the length by the completed command
                    add = count - (i + 1);
                }
                // Add from read buffer to the command buffer
                command_buf[length+i-offset] = read_buf[i];
            }
            // Add the length of the incomplete command if it exists
            length += add;
            offset = add = 0;
        }

        // Avoid reporting if shutdown enabled
        if (!run_flag)
            break;

        // Measure reporting intervals using clock_gettime() rather than using sleep()
        // This allows the program to immediately process and respond to sent commands
        clock_gettime(CLOCK_MONOTONIC, &finish);
        if (generate_reports && ((int)(finish.tv_sec - start.tv_sec) >= period)) {
            time(&raw_time);
            curr_time = localtime(&raw_time);
            temp = convert_read_to_temp(mraa_aio_read(temp_sensor), scale);
            fprintf(log_file, "%02d:%02d:%02d %0.1f\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec, temp);
            sprintf(ssl_buffer, "%02d:%02d:%02d %0.1f\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec, temp);
            ssl_write_check(ssl, ssl_buffer, strlen(ssl_buffer));
            memset(ssl_buffer, '\0', 128);
            clock_gettime(CLOCK_MONOTONIC, &start);
        }
    }

    // Close sensors
    if (mraa_aio_close(temp_sensor) != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to close temperature sensor\n");
        exit(2);
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);

    exit(0);
}
