#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

struct Memory {
    char *response;
    size_t size;
};

size_t write_callback(void *data, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *temp = realloc(mem->response, mem->size + total_size + 1);
    if (temp == NULL) {
        printf("Failed to allocate memory\n");
        return 0;
    }
    mem->response = temp;

    memcpy(&(mem->response[mem->size]), data, total_size);
    mem->size += total_size;
    mem->response[mem->size] = '\0';

    return total_size;
}

char *fetch_weather_data(const char *url) {
    CURL *curl;
    CURLcode res;
    struct Memory chunk = {0};

    curl = curl_easy_init();
    if (!curl) {
        printf("Failed to initialize CURL\n");
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        printf("CURL error: %s\n", curl_easy_strerror(res));
        free(chunk.response);
        chunk.response = NULL;
    }

    curl_easy_cleanup(curl);
    return chunk.response;
}

void parse_weather_data(const char *data) {
    cJSON *json = cJSON_Parse(data);
    if (!json) {
        printf("Error parsing JSON\n");
        return;
    }

    cJSON *main = cJSON_GetObjectItemCaseSensitive(json, "main");
    cJSON *weather_array = cJSON_GetObjectItemCaseSensitive(json, "weather");
    cJSON *weather = cJSON_GetArrayItem(weather_array, 0);

    if (main && weather) {
        double temp = cJSON_GetObjectItemCaseSensitive(main, "temp")->valuedouble;
        int humidity = cJSON_GetObjectItemCaseSensitive(main, "humidity")->valueint;
        const char *condition = cJSON_GetObjectItemCaseSensitive(weather, "description")->valuestring;

        printf("Temperature: %.2f°C\n", temp - 273.15); 
        printf("Humidity: %d%%\n", humidity);
        printf("Condition: %s\n", condition);
    } else {
        printf("Error extracting data\n");
    }

    cJSON_Delete(json);
}
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <city_name>\n", argv[0]);
        return 1;
    }

    const char *api_key = "431c05e678dcb7141aa7909fb86477be";
    char url[256];
    snprintf(url, sizeof(url), "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s", argv[1], api_key);

    char *response = fetch_weather_data(url);
    if (response) {
        parse_weather_data(response);
        free(response);
    } else {
        printf("Failed to fetch weather data\n");
    }

    return 0;
}
