#ifndef RANDOM_H_
#define RANDOM_H_
#include <stdlib.h>
#include <stdint.h>
#include "stm32f1xx_hal.h"
#define SEED_NUM 10 //种子数量
extern void RGB_Srand_Init(void);
extern int Get_RandValue(void);
extern ADC_HandleTypeDef* random_source;
#endif
