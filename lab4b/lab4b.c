#include <stdio.h>
#include <stdlib.h>
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

int run_flag = 1;
const int B  = 4275;
const int R0 = 100000;
FILE* log_file = NULL;

// Shutdown code. Enabled from button and OFF command
void button_and_off_interrupt() {
    time_t raw_time;
    struct tm* curr_time;
    time(&raw_time);
    curr_time = localtime(&raw_time);
    printf("%02d:%02d:%02d SHUTDOWN\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec);
    if (log_file != NULL) {
        fprintf(log_file, "%02d:%02d:%02d SHUTDOWN\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec);
    }
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


int main(int argc, char**argv) {

    // Process command line arguments
    char c;
    int option_index = 0;
    int period = 1;
    char scale = 'F';
    static struct option long_options[] = {
        {"period", required_argument, 0, 'p'},
        {"scale",  required_argument, 0, 's'},
        {"log",    required_argument, 0, 'l'},
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
        }
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
    mraa_gpio_context button;
    temp_sensor = mraa_aio_init(1);
    button = mraa_gpio_init(60);

    // Check for successful sensor connections
    if (button == NULL) {
        fprintf(stderr, "Error initializing button\n");
        exit(1);
    }
    if (temp_sensor == NULL) {
        fprintf(stderr, "Error initializing temperature sensor\n");
        exit(1);
    }

    // Set button as a gpio input and check for rising edge signal
    mraa_gpio_dir(button, MRAA_GPIO_IN);
    mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &button_and_off_interrupt, NULL);

    // Poll standard input
    struct pollfd poll_list;
    poll_list.fd = 0;
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
    printf("%02d:%02d:%02d %0.1f\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec, temp);
    if (log_file != NULL)
        fprintf(log_file, "%02d:%02d:%02d %0.1f\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec, temp);
    clock_gettime(CLOCK_MONOTONIC, &start);

    while (run_flag) {
        if (poll(&poll_list, 1, 0) < 0) {
            fprintf(stderr, "Polling has failed: %s\n", strerror(errno));
            exit(1);
        }

        if (poll_list.revents & POLLIN) {
            if ((count = read(0, &read_buf, 512)) < 0) {
                fprintf(stderr, "Reading from stdin failed: %s\n", strerror(errno));
                exit(1);
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
                        if (log_file != NULL)
                            fprintf(log_file, "OFF\n");
                        button_and_off_interrupt();
                        break;
                    }

                    // Log all commands valid or invalid if enabled
                    if (log_file != NULL) {
                        fprintf(log_file, command_buf);
                        fprintf(log_file, "\n");
                    }

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
            printf("%02d:%02d:%02d %0.1f\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec, temp);
            if (log_file != NULL)
                fprintf(log_file, "%02d:%02d:%02d %0.1f\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec, temp);
            clock_gettime(CLOCK_MONOTONIC, &start);
        }
    }

    // Close sensors
    if (mraa_aio_close(temp_sensor) != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to close temperature sensor\n");
        exit(1);
    }
    if (mraa_gpio_close(button) != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to close button\n");
        exit(1);
    }

    exit(0);
}
