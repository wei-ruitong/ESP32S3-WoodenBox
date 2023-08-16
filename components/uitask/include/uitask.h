#ifndef _TASK_H
#define _TASK_H

#ifdef __cplusplus
extern "C" {
#endif


void lv_tick_task(void *arg);
void guiTask(void *pvParameter);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif

