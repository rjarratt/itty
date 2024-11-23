#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <wiringPi.h>

#define BUFFER_SIZE 1400

int process(int relayPinNumber, int powerOnWaitTimeSeconds, int powerOffWaitTimeSeconds);
int copy_data(int relayPinNumber, int powerOnWaitTimeSeconds, int powerOffWaitTimeSeconds);
int poweron(int relayPinNumber, int powerOnWaitTimeSeconds);
int poweroff(int relayPinNumber);
int map_relay_number_to_pin_number(int relay);

int relay_to_pin_map[] = { 4, 17, 27, 22 };

#define NUM_RELAYS (sizeof(relay_to_pin_map)/sizeof(int))

int main(int argc,char *argv[])
{
    int result = 0;
    int relay = -1;
    int powerOnWaitTimeSeconds= 5;
    int powerOffWaitTimeSeconds = 30;

    int option;
    while ((option = getopt(argc, argv, "d:w:r:")) != -1)
    {
        switch (option)
        {
        case 'd':
            powerOnWaitTimeSeconds = atoi(optarg);
            break;

        case 'w':
            powerOffWaitTimeSeconds = atoi(optarg);
            break;

        case 'r':
            relay = atoi(optarg);
            break;
        }
    }

    //fprintf(stderr, "d=%d, w=%d, r=%d\n", powerOnWaitTimeSeconds, powerOffWaitTimeSeconds, relay);

    if (powerOnWaitTimeSeconds <= 0 || powerOffWaitTimeSeconds <= 0 || relay <= 0)
    {
        fprintf(stderr, "Usage: pcntl [-d n] [-w n] -r n\n");
        fprintf(stderr, "  -d n The power on delay in seconds (default=5). If the power is off it powers on the device and waits n seconds before sending the first character. This is designed to give the equipment time to start.\n");
        fprintf(stderr, "  -w n The power off delay in seconds (default=30). After the last data has been received it waits n seconds for more data and if there isn't any it switches off the device.\n");
        fprintf(stderr, "  -r n The number of the relay on the BC Robotics Relay HAT.\n");
    }
    else
    {
        int pin = map_relay_number_to_pin_number(relay);
        if (pin < 0)
        {
            fprintf(stderr, "Invalid relay number must be in the range 1-%d\n", NUM_RELAYS);
        }
        else
        {
            result = process(pin, powerOnWaitTimeSeconds, powerOffWaitTimeSeconds);
        }
    }

    return result;
}

int process(int relayPinNumber, int powerOnWaitTimeSeconds, int powerOffWaitTimeSeconds)
{
    wiringPiSetupGpio();
    pinMode(relayPinNumber, OUTPUT);
    int status = fcntl(fileno(stdin), F_SETFL, O_NONBLOCK);
    if (status == -1)
    {
        fprintf(stderr, "Failed to set non-blocking i/o\n");
    }
    else
    {
        status = copy_data(relayPinNumber, powerOnWaitTimeSeconds, powerOffWaitTimeSeconds);
    }

    return status;
}

int copy_data(int relayPinNumber, int powerOnWaitTimeSeconds, int powerOffWaitTimeSeconds)
{
    char buf[BUFFER_SIZE];
    int receivingData = 0;
    time_t timeLastDataReceived;

    while(!feof(stdin))
    {
        int bytes = read(fileno(stdin), buf, BUFFER_SIZE);
        if (bytes  == -1)
        {
            if (errno == EAGAIN)
            {
                time_t now = time(NULL);
                double timeSinceLastData = difftime(now, timeLastDataReceived);
                if (receivingData && timeSinceLastData > powerOffWaitTimeSeconds)
                {
                    poweroff(relayPinNumber);
                    receivingData = 0;
                }

                sleep(1);
            }
        }
        else
        {
            if (!receivingData)
            {
                poweron(relayPinNumber, powerOnWaitTimeSeconds);
            }
 
            receivingData = 1;
            timeLastDataReceived = time(NULL);
            fwrite(buf, 1, bytes, stdout);
        }
    }
}

int poweron(int relayPinNumber, int powerOnWaitTimeSeconds)
{
    digitalWrite(relayPinNumber, HIGH);
    sleep(powerOnWaitTimeSeconds);
}

int poweroff(int relayPinNumber)
{
    digitalWrite(relayPinNumber, LOW);
}

int map_relay_number_to_pin_number(int relay)
{
    int pin = -1;
    if (relay >= 1 && relay <= NUM_RELAYS)
    {
        pin = relay_to_pin_map[relay - 1];
    }

    return pin;
}
