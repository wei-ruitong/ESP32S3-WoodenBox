#ifndef _SD_CARD_H
#define _SD_CARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_err.h"



esp_err_t sd_init();



#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif

