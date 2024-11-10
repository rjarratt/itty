#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/wait.h>

int process(char *url);
void read_data(CURLM *multi, CURL *curl, char *url);
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);

int main(int argc,char *argv[])
{
    int result = 0;
    if (argc != 2)
    {
        fprintf(stderr, "Usage: itty <url>\n");
    }
    else
    {
           result = process(argv[1]);
    }

    return result;
}

int process(char *url)
{
    CURLM *multi;
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl)
    {
        multi = curl_multi_init();
        if (multi)
        {
            while (1)
            {
                read_data(multi, curl, url);
            }
        }
        else
        {
            fprintf(stderr, "Cannot initialise CurlM\n");
        }

        curl_multi_cleanup(curl);
    }
    else
    {
        fprintf(stderr, "Cannot initialise Curl\n");
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}

void read_data(CURLM *multi, CURL *curl, char *url)
{
    int still_running = 1;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_multi_add_handle(multi, curl);

    do
    {
        CURLMcode mc = curl_multi_perform(multi, &still_running);
        if (!mc)
        {
            /* wait for activity, timeout or "nothing" */
            mc = curl_multi_poll(multi, NULL, 0, 1000, NULL);
        }
 
        if (mc)
        {
            fprintf(stderr, "curl_multi_poll() failed, code %d.\n", (int)mc);
            break;
        }
 
    }
    while(still_running);

    curl_multi_remove_handle(multi, curl);
}

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    int result = fwrite(buffer, size, nmemb, stdout);
    return result;
}

