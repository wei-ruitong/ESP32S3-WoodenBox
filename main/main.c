#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "weather.h"
#include "mysntp.h"
#include "uitask.h"
#include "uitask.h"
#include "wifi.h"

extern SemaphoreHandle_t xSemaphore;
void app_main()
{
    connect_wifi();
    if(xSemaphoreTake( xSemaphore, ( TickType_t ) 10000 ) == pdTRUE ){
        xTaskCreatePinnedToCore(guiTask, "gui", 1024*8, NULL, 0, NULL, 1);
    }else{
        printf("wifi connect failed  in 10000 ticks\n");
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(getWeather, "getWeather", 4096*3, NULL, 0, NULL, 1);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(getTime, "getTime",1024*4, NULL, 0, NULL, 1);  
}
