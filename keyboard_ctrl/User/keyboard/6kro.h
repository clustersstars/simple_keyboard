#ifndef _6KRO_H_
#define _6KRO_H_
#include <stdint.h>
#include "tool_lib.h"
#define _6KRO_REPORT_BUFFER_SIZE 8            //六键标准键盘报表字节长度
#define _6KRO_BUFFER_SIZE 6

//六键缓冲结构体 
typedef struct{                            
  uint8_t buffer_size;
  uint8_t buffer_top;
  volatile uint8_t modifier_key;
  uint8_t *buffer;
}_6KRO_Buffer;

typedef struct _6KRO_BufferManage{
  // volatile uint8_t function_key_flag;
  // volatile uint8_t function_key;
  // volatile uint8_t check;
  // volatile uint8_t temp_check;
  // HashMark_InitTypeDef *hash_structure;
  _6KRO_Buffer        *_6kro_instant_buffer;
  void (*push)(struct _6KRO_BufferManage* buffer_manage,uint8_t key);
  void (*clearBuffer)(_6KRO_Buffer* _6kro_buffer);
}_6KRO_BufferManage;

//六键无冲键盘 双缓冲报表
typedef struct _6KRO_Report{
  uint8_t* _6kro_report_channel1;   //缓冲通道1
  uint8_t* _6kro_report_channel2;   //缓冲通道2
  volatile uint8_t channel_selected;   //通道选择
  void (*fillReport)(_6KRO_BufferManage* buffer_manage,struct _6KRO_Report* report);
}_6KRO_Report;     


bool _6KRO_BufferManage_Init(_6KRO_BufferManage* buffer_manage);
bool _6KRO_Report_Init(_6KRO_Report* report);
#endif
