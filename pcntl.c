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

int main(int argc,char *argv[])
{
    int result = 0;
    if (argc != 1)
    {
        fprintf(stderr, "Usage: pcntl\n");
    }
    else
    {
           result = process(4, 5, 5);
    }

    return result;
}

int process(int relayPinNumber, int powerOnWaitTimeSeconds, int powerOffWaitTimeSeconds)
{
    wiringPiSetupGpio();
    pinMode(4, OUTPUT);
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
