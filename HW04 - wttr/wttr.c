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
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            printf("ERROR: could not curl easy perform, %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
    }
    curl_version_info_data *data = curl_version_info(CURLVERSION_NOW);
    printf("OK: %d\n%s\n", res, data->protocols);
    return EXIT_SUCCESS;
}