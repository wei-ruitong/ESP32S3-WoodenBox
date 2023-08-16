#ifndef __MY_SNTP_H
#define __MY_SNTP_H

#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_sntp.h"


#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 48
#endif

void get_net_time(void);
void getTime(void * parm);

#endif // !__MY_SNTP_H

