#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

#define BUFFER_SIZE 1400

int process(int powerOnWaitTimeSeconds, int powerOffWaitTimeSeconds);
int copy_data(int powerOnWaitTimeSeconds, int powerOffWaitTimeSeconds);
int poweron(int powerOnWaitTimeSeconds);
int poweroff(void);

int main(int argc,char *argv[])
{
    int result = 0;
    if (argc != 1)
    {
        fprintf(stderr, "Usage: pcntl\n");
    }
    else
    {
           result = process(5, 5);
    }

    return result;
}

int process(int powerOnWaitTimeSeconds, int powerOffWaitTimeSeconds)
{
    int status = fcntl(fileno(stdin), F_SETFL, O_NONBLOCK);
    if (status == -1)
    {
        fprintf(stderr, "Failed to set non-blocking i/o\n");
    }
    else
    {
        status = copy_data(powerOnWaitTimeSeconds, powerOffWaitTimeSeconds);
    }

    return status;
}

int copy_data(int powerOnWaitTimeSeconds, int powerOffWaitTimeSeconds)
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
                    poweroff();
                    receivingData = 0;
                }

                sleep(1);
            }
        }
        else
        {
            if (!receivingData)
            {
                poweron(powerOnWaitTimeSeconds);
            }
 
            receivingData = 1;
            timeLastDataReceived = time(NULL);
            fwrite(buf, 1, bytes, stdout);
        }
    }
}

int poweron(int powerOnWaitTimeSeconds)
{
    fprintf(stderr, "Powering on\n");
    sleep(powerOnWaitTimeSeconds);
}

int poweroff(void)
{
    fprintf(stderr, "Powering off\n");
}
