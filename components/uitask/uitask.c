#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "ui.h"
#include "lv_port_indev.h"
#include "lv_port_disp.h"
// #include "lv_demos.h"
#include "uitask.h"
#include "lv_port_fs.h"
#define TAG "demo"
void lv_tick_task(void *arg)
{
    lv_tick_inc(1);
}
void guiTask(void *pvParameter)
{
    lv_init();
    vTaskDelay(10/portTICK_PERIOD_MS);
    // lvgl驱动初始化
    lvgl_driver_init();
    // 文件系统初始化
    fatfs_init();
    // 屏幕显示初始化
    display_init();
    // 输入设备初始化---触摸屏
    indev_init();
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1000));
    ui_init();
    for (;;)
    {
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


