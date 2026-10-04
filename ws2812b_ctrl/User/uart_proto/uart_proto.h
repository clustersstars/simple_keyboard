#ifndef UART_PROTO_H_
#define UART_PROTO_H_
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "tool_lib.h"
//#define
#define UART_DATA_HEADER 224U //0b11100000
#define UART_DATA_TYPE   24U  //0b00011000

typedef enum {
    DATA_TYPE_ERROR    = 0U,
    DATA_TYPE_POSITION = 1U,
    DATA_TYPE_COMMAND  = 2U
}UART_Data_TypeDef;

typedef struct {
    uint8_t data_header;
    uint8_t data_len;
    uint8_t *data;
}UART_Data_Def;

typedef enum {
    MASTER_MODE = 0U,
    SLAVE_MODE
}UART_Mode_TypeDef;

typedef struct {
    UART_HandleTypeDef *uart;
    UART_Data_Def udata;
    uint8_t temp_buffer_len;
    uint8_t *temp_buffer;
    uint8_t *receive_buffer;
    uint8_t RE;
}UART_ManagerTypeDef;

extern uint8_t temp_buffer[100];
extern uint8_t receive_buffer[100];
extern UART_ManagerTypeDef uart_manager;

void UART_Manager_Init(UART_HandleTypeDef *uart);
void BufMove(const uint8_t *Arr,uint8_t size);
bool UART_Data_Transfer(UART_Data_TypeDef data_type,uint8_t *data,uint8_t data_num);
void ReceiveBufferParse(void (*Command_HandlingCallback)(uint8_t),void (*Position_HandlingCallback)(uint8_t));
__weak void Command_HandlingCallback(uint8_t command);
__weak void Position_HandlingCallback(uint8_t position);
#endif
