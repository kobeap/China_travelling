#ifndef __openmv_h__
#define __openmv_h__
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
extern uint8_t color;
void open_mv(void);
void close_mv(void);
void mv_init(uint32_t bound);
#endif
