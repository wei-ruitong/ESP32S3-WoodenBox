#include "mysntp.h"
#include "ui.h"
static const char *SNTP_TAG = "SNTP";
unsigned char week[7] = {'日', '一', '二', '三', '四', '五', '六'};
char str[2];
static void initialize_sntp(void)
{
    ESP_LOGI(SNTP_TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_setservername(1, "ntp.aliyun.com"); // set the secondary NTP server (will be used only if SNTP_MAX_SERVERS > 1)
    sntp_setservername(2, "210.72.145.44");
    sntp_init();
    ESP_LOGI(SNTP_TAG, "List of configured NTP servers:");
    for (uint8_t i = 0; i < SNTP_MAX_SERVERS; ++i)
    {
        if (sntp_getservername(i))
        {
            ESP_LOGI(SNTP_TAG, "server %d: %s", i, sntp_getservername(i));
        }
        else
        {
            // we have either IPv4 or IPv6 address, let's print it
            char buff[INET6_ADDRSTRLEN];
            ip_addr_t const *ip = sntp_getserver(i);
            if (ipaddr_ntoa_r(ip, buff, INET6_ADDRSTRLEN) != NULL)
                ESP_LOGI(SNTP_TAG, "server %d: %s", i, buff);
        }
    }
}
static void obtain_time(void)
{
    initialize_sntp();
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 15;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count)
    {
        ESP_LOGI(SNTP_TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    sntp_stop();
    time(&now);
    localtime_r(&now, &timeinfo);
}
void get_net_time(void)
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2016 - 1900))
    {
        ESP_LOGI(SNTP_TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
        obtain_time();
        // update 'now' variable with current time
        time(&now);
    }
    setenv("TZ", "CST-8", 1);
    tzset();
    localtime_r(&now, &timeinfo);
    int min = timeinfo.tm_min;
    int hour = timeinfo.tm_hour;
    int wday = timeinfo.tm_wday;

    memset(str, 0, 2);
    if (hour / 10 == 0)
    {
        str[0] = '0';
        str[1] = hour;
    }
    else
    {
        sprintf(str, "%d", hour);
        lv_label_set_text(ui_Label11, str);
        memset(str, 0, 2);
    }
    if (min / 10 == 0)
    {
        str[0] = '0';
        str[1] = min;
    }
    else
    {
        sprintf(str, "%d", min);
    }
    lv_label_set_text(ui_Label12, str);
    // lv_label_set_text(ui_Label17, week[wday]);
}
void getTime(void *parm)
{
    for (;;)
    {
        get_net_time();
        vTaskDelay(pdMS_TO_TICKS(1000 * 30));
    }
}