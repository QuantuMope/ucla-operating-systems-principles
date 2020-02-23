#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <sys/poll.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <mraa/aio.h>
#include <mraa/gpio.h>

sig_atomic_t volatile run_flag = 1;
const int B  = 4275;
const int R0 = 100000;

void button_interrupt() {
    time_t raw_time;
    struct tm* curr_time;
    time(&raw_time);
    curr_time = localtime(&raw_time);
    printf("%d:%d:%d SHUTDOWN\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec);
    run_flag = 0;
}

void key_interrupt(int sig) {
    if (sig == SIGINT)
        run_flag = 0;
}

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
    int log_fd = NULL;
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
                if ((log_fd = creat(optarg, 0666)) < 0) {
                    fprintf(stderr, "Failed to create/open file %s: %s\n", optarg, strerror(errno));
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

    signal(SIGINT, key_interrupt);

    // Initialize sensors
    mraa_aio_context temp_sensor;
    mraa_gpio_context button;
    temp_sensor = mraa_aio_init(1);
    button = mraa_gpio_init(60);
    mraa_gpio_dir(button, MRAA_GPIO_IN);
    mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &button_interrupt, NULL);
    if (temp_sensor == NULL) {
        printf("Error initializing temperature sensor\n");
        exit(1);
    }

    struct pollfd poll_list;
    poll_list.fd = 0;
    poll_list.events = POLLIN;

    time_t raw_time;
    struct tm* curr_time;

    char read_buf[512] = {0};
    char command_buf[512] = {0};
    int offset = 0;
    int length = 0;
    int count;

    float temp;
    while (run_flag) {
        if (poll(&poll_list, 1, 0) < 0) {
            fprintf(stderr, "Polling has failed: %s\n", strerror(errno));
            exit(1);
        }

        if (poll_list.revents & POLLIN) {
            if ((count = read(0, read_buf, 512)) < 0) {
                fprintf(stderr, "Reading from stdin failed: %s\n", strerror(errno));
                exit(1);
            }
            for (int i = 0; i < count; i++) {
                if (read_buf[i] == '\n') {
                    command_buf[length+i] = '\0';

                }
                command_buf[length+i] = read_buf[i];
            }
            strcpy(command_buf+length, read_buf);
            length += offset;
        }

        time(&raw_time);
        curr_time = localtime(&raw_time);

        temp = convert_read_to_temp(mraa_aio_read(temp_sensor), scale);
        printf("%d:%d:%d %0.1f\n", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec, temp);
        sleep(period);
    }

    mraa_aio_close(temp_sensor);
    mraa_gpio_close(button);

    exit(0);
}
