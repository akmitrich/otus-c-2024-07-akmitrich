#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct JseHandler
{
    char *buffer;
    size_t size;
};

static size_t json_write_to_buffer(void *data, size_t size, size_t nmemb, void *clientp);

int main(void)
{
    struct JseHandler json_handler = {0};
    CURLcode res;
    CURL *curl = curl_easy_init();
    if (curl == NULL)
    {
        printf("FATAL: could not initialize cURL.");
        return EXIT_FAILURE;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://wttr.in/Moscow?format=j1");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, json_write_to_buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&json_handler);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        printf("ERROR: could not curl easy perform, %s\n", curl_easy_strerror(res));
    curl_easy_cleanup(curl);

    printf("OK: %d\n%s\n", res, json_handler.buffer);
    return EXIT_SUCCESS;
}

size_t json_write_to_buffer(void *data, size_t size, size_t nmemb, void *clientp)
{
    size_t arrived_size = size * nmemb;
    struct JseHandler *handler = (struct JseHandler *)clientp;
    size_t old_size = handler->size;
    size_t new_jse_size = old_size + arrived_size;
    void *ptr = realloc(handler->buffer, new_jse_size + 1);
    if (ptr == NULL)
    {
        printf("FATAL: out of memory.\n");
        return 0;
    }
    handler->buffer = ptr;

    memcpy(&(handler->buffer[old_size]), data, arrived_size);
    handler->buffer[new_jse_size] = '\0';
    handler->size = new_jse_size;

    return arrived_size;
}