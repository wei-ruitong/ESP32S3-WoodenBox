/* ESP HTTP Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_tls.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "ui.h"
#define MAX_HTTP_OUTPUT_BUFFER 1200
static const char *TAG = "HTTP_CLIENT";

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer; // Buffer to store response of http request from event handler
    static int output_len;      // Stores number of bytes read
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        /*
         *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
         *  However, event handler can also be used in case chunked encoding is used.
         */
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            // If user_data buffer is configured, copy the response into the buffer
            if (evt->user_data)
            {
                memcpy(evt->user_data + output_len, evt->data, evt->data_len);
            }
            else
            {
                if (output_buffer == NULL)
                {
                    output_buffer = (char *)malloc(esp_http_client_get_content_length(evt->client));
                    output_len = 0;
                    if (output_buffer == NULL)
                    {
                        ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                        return ESP_FAIL;
                    }
                }
                memcpy(output_buffer + output_len, evt->data, evt->data_len);
            }
            output_len += evt->data_len;
        }

        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        if (output_buffer != NULL)
        {
            // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
            // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        int mbedtls_err = 0;
        esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
        if (err != 0)
        {
            ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
            ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
        }
        if (output_buffer != NULL)
        {
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    }
    return ESP_OK;
}

static void http_rest_with_url(void)
{
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
    /**
     * NOTE: All the configuration parameters for http_client must be spefied either in URL or as host and path parameters.
     * If host and path parameters are not set, query parameter will be ignored. In such cases,
     * query parameter should be specified in URL.
     *
     * If URL as well as host and path parameters are specified, values of host and path will be considered.
     */
    esp_http_client_config_t config = {
        .url = "http://api.seniverse.com/v3/weather/daily.json?key=Sn0_6sq7sqVlYVVVL&location=Dongguan&language=zh-Hans&unit=c&start=0&days=3",
        // .host = "httpbin.org",
        // .path = "/get",
        // .query = "esp",
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer, // Pass address of local buffer to get response
        // .disable_auto_redirect = true,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        // ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
        //         esp_http_client_get_status_code(client),
        //         esp_http_client_get_content_length(client));
        //    printf("%s\r\n", cJSON_Print(cJSON_Parse(local_response_buffer)));
        cJSON *cjson = NULL;
        cJSON *results = NULL;
        cJSON *daily = NULL;
        cJSON *daily_item = NULL;
        cJSON *text_day = NULL;
        cJSON *code_day = NULL;
        cJSON *high = NULL;
        cJSON *low = NULL;
        cJSON *wind_direction = NULL;
        cJSON *wind_scale = NULL;
        cJSON *humidity = NULL;
        cJSON *date = NULL;
        cjson = cJSON_Parse(local_response_buffer);
        if (cjson == NULL)
        {
            printf("JSON convert failed\r\n");
            return;
        }
        // printf("%s\r\n", cJSON_Print(cjson));
        results = cJSON_GetObjectItem(cjson, "results");
        daily = cJSON_GetObjectItem(cJSON_GetArrayItem(results, 0), "daily");
        uint8_t i;
        char filename[32];
        for (i = 0; i < 3; i++)
        {
            memset(filename, 0, 32);
            daily_item = cJSON_GetArrayItem(daily, i);
            date = cJSON_GetObjectItem(daily_item, "date");
            text_day = cJSON_GetObjectItem(daily_item, "text_day");
            code_day = cJSON_GetObjectItem(daily_item, "code_day");
            high = cJSON_GetObjectItem(daily_item, "high");
            low = cJSON_GetObjectItem(daily_item, "low");
            wind_direction = cJSON_GetObjectItem(daily_item, "wind_direction");
            wind_scale = cJSON_GetObjectItem(daily_item, "wind_scale");
            humidity = cJSON_GetObjectItem(daily_item, "humidity");
            printf("%s   %s\r\n", text_day->valuestring, high->valuestring);
            strcat(filename, "S:pictures/big/");
            strcat(filename, code_day->valuestring);
            strcat(filename, ".bin");
            printf("%s\r\n", filename);
            if (i == 0)
            {
                lv_img_set_src(ui_ImgButton5, filename);
                lv_img_set_src(ui_Image5, filename);

                lv_label_set_text(ui_Label1, date->valuestring);
                lv_label_set_text(ui_Label13, date->valuestring);
                lv_label_set_text(ui_Label2, text_day->valuestring);
                lv_label_set_text(ui_Label18, text_day->valuestring);
                lv_label_set_text(ui_Label3, wind_direction->valuestring);
                memset(filename, 0, 32);
                strcat(filename, wind_scale->valuestring);
                strcat(filename, "级");
                lv_label_set_text(ui_Label6, filename);
                memset(filename, 0, 32);
                strcat(filename, humidity->valuestring);
                strcat(filename, "%");
                lv_label_set_text(ui_Label8, filename);
                memset(filename, 0, 32);
                strcat(filename, high->valuestring);
                strcat(filename, "℃↑");
                lv_label_set_text(ui_Label7, filename);
                memset(filename, 0, 32);
                strcat(filename, low->valuestring);
                strcat(filename, "℃↓");
                lv_label_set_text(ui_Label9, filename);
            }
        }
    }
    else
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

void getWeather(void *pvParameters)
{
    for (;;)
    {
        http_rest_with_url();
        vTaskDelay(5*60*1000/portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
