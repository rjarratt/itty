#include <stdio.h>
#include <curl/curl.h>

int process(char *url);
void read_data(CURLM *multi);

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
    CURL *curl;
    CURLM *multi;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl)
    {
        multi = curl_multi_init();
        if (multi)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_multi_add_handle(multi, curl);

            read_data(multi);

            curl_multi_remove_handle(multi, curl);
            curl_easy_cleanup(curl);
            curl_multi_cleanup(curl);
            curl_global_cleanup();
        }
        else
        {
            fprintf(stderr, "Cannot initialise CurlM\n");
        }
    }
    else
    {
        fprintf(stderr, "Cannot initialise Curl\n");
    }

    return 0;
}

void read_data(CURLM *multi)
{
    int still_running = 1;
    do
    {
        CURLMcode mc = curl_multi_perform(multi, &still_running);
 
        if(!mc)
        /* wait for activity, timeout or "nothing" */
        mc = curl_multi_poll(multi, NULL, 0, 1000, NULL);
 
        if (mc)
        {
            fprintf(stderr, "curl_multi_poll() failed, code %d.\n", (int)mc);
            break;
        }
 
    }
    while(still_running);
}
