#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON/cJSON.h"

struct JseHandler
{
    char *buffer;
    size_t size;
};

static size_t json_write_to_buffer(void *data, size_t size, size_t nmemb, void *clientp);
static int print_weather(const char *location);
static void json_print_info(const char *json);

int main(int argc, const char *argv[])
{
    const char *program = argv[0];
    printf("%s -- request current weather for ", program);
    if (argc != 2)
    {
        printf("given LOCATION\n\n");
        printf("Usage: %s LOCATION\n", program);
        printf("LOCATION -- reporting point to request weather for.\n");
        return EXIT_FAILURE;
    }
    const char *location = argv[1];
    printf("%s\n", location);

    return print_weather(location);
}

#define LOCATION_MAX_LEN 1024

int print_weather(const char *location)
{
    static const char *base_url = "https://wttr.in/";
    static const char *fmt_json = "?format=j1";
    if (strlen(location) > LOCATION_MAX_LEN - strlen(base_url) - strlen(fmt_json))
    {
        printf("ERROR: location name %s is too long.\n", location);
        return EXIT_FAILURE;
    }
    struct JseHandler json_handler = {0};
    char url[LOCATION_MAX_LEN];
    CURLcode res;
    CURL *curl = curl_easy_init();
    if (curl == NULL)
    {
        printf("FATAL: could not initialize cURL.");
        return EXIT_FAILURE;
    }
    sprintf(url, "%s%s%s", base_url, location, fmt_json);
    printf("Send request to %s\n", url);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, json_write_to_buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&json_handler);

    res = curl_easy_perform(curl);
    if (res == CURLE_OK)
    {
        printf("OK: %d\n\n", res);
        json_print_info(json_handler.buffer);
    }
    else
        printf("ERROR: could not curl easy perform, %s\n", curl_easy_strerror(res));
    curl_easy_cleanup(curl);

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

void json_print_info(const char *json_str)
{
    cJSON *json = cJSON_Parse(json_str);
    if (json == NULL)
    {
        printf("ERROR: could not parse JSON.\n");
        return;
    }
    cJSON *current_condition;
    if ((current_condition = cJSON_GetObjectItem(json, "current_condition")))
    {
        current_condition = cJSON_GetArrayItem(current_condition, 0);
        cJSON *obs_tm = cJSON_GetObjectItem(current_condition, "localObsDateTime");
        const char *observation_time = cJSON_GetStringValue(obs_tm);
        printf("Observation time: %s\n", observation_time);
        cJSON *w_desc = cJSON_GetObjectItem(current_condition, "weatherDesc");
        w_desc = cJSON_GetArrayItem(w_desc, 0);
        w_desc = cJSON_GetObjectItem(w_desc, "value");
        const char *weather_decription = cJSON_GetStringValue(w_desc);
        cJSON *temp = cJSON_GetObjectItem(current_condition, "temp_C");
        const char *temp_C = cJSON_GetStringValue(temp);
        printf("Temperature: %s\u00b0C, %s\n", temp_C, weather_decription);
        cJSON *feels = cJSON_GetObjectItem(current_condition, "FeelsLikeC");
        const char *feels_like_C = cJSON_GetStringValue(feels);
        printf("Feels like: %s\u00b0C\n", feels_like_C);
        cJSON *wind_desc = cJSON_GetObjectItem(current_condition, "winddir16Point");
        const char *wind_description = cJSON_GetStringValue(wind_desc);
        cJSON *wind_dir = cJSON_GetObjectItem(current_condition, "winddirDegree");
        const char *wind_direction = cJSON_GetStringValue(wind_dir);
        printf("Wind direction: %s\u00b0, %s\n", wind_direction, wind_description);
        cJSON *wind_sp = cJSON_GetObjectItem(current_condition, "windspeedKmph");
        const char *wind_speed = cJSON_GetStringValue(wind_sp);
        printf("Wind speed: %s km/h\n", wind_speed);
    }
    cJSON *nearest_area;
    if ((nearest_area = cJSON_GetObjectItem(json, "nearest_area")))
    {
        nearest_area = cJSON_GetArrayItem(nearest_area, 0);
        cJSON *area_name = cJSON_GetObjectItem(nearest_area, "areaName");
        area_name = cJSON_GetArrayItem(area_name, 0);
        area_name = cJSON_GetObjectItem(area_name, "value");
        const char *town = area_name->valuestring;
        cJSON *country = cJSON_GetObjectItem(nearest_area, "country");
        country = cJSON_GetArrayItem(country, 0);
        country = cJSON_GetObjectItem(country, "value");
        const char *at = country->valuestring;
        printf("Location: %s, %s\n", town, at);
    }
}