#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

int main(void)
{
    CURL *curl = curl_easy_init();
    if (curl == NULL)
    {
        printf("ERROR: could not initialize cURL.");
        return EXIT_FAILURE;
    }
    CURLcode res;
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://wttr.in/Moscow?format=j1");
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    printf("Hello, wttr! %d\n", res);
    return EXIT_SUCCESS;
}