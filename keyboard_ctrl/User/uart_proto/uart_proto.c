#include "uart_proto.h"
#include "stdio.h"
#include "string.h"
#include "tool_lib.h"
// //变量声明
uint8_t temp_buffer[100] = {0};
uint8_t receive_buffer[100] = {0};
UART_ManagerTypeDef uart_manager;

static RingList_InitTypeDef ring_structure;
static void UART_Send_Data(void);

void UART_Manager_Init(UART_HandleTypeDef *uart) {
  uart_manager.uart = uart;
  uart_manager.udata.data_header = 0;
  uart_manager.udata.data = NULL;
  uart_manager.udata.data_len = 0;
  uart_manager.temp_buffer_len = 0;
  uart_manager.temp_buffer = temp_buffer;
  uart_manager.receive_buffer = receive_buffer;
  uart_manager.RE = 0;
  if (!RingList_Init(&ring_structure,100,receive_buffer)) {
    //初始化失败
  }
}

void ReceiveBufferParse(void (*Command_HandlingCallback)(uint8_t),void (*Position_HandlingCallback)(uint8_t)){
  uint8_t data_type = 0;
  uint8_t data = 0;
  while (!RingList_IsEmpty(&ring_structure)) {
    //获取元素
    data = (uint8_t)RingList_Pop(&ring_structure);
    //判断元素是指令还是数据
    if ((data & UART_DATA_HEADER) == UART_DATA_HEADER) {
      if ((data & UART_DATA_TYPE) >> 3 == DATA_TYPE_ERROR) {
        RingList_Update_TempReadPointer(&ring_structure);
      }else {
        data_type = (data & UART_DATA_TYPE) >> 3;
      }
    }else {
      if (data_type == DATA_TYPE_POSITION) {
        Position_HandlingCallback(data);
      }else if (data_type == DATA_TYPE_COMMAND) {
        Command_HandlingCallback(data);
      }else {
        //类型错误
      }
      //更新temp_read_pointer
      RingList_Update_TempReadPointer(&ring_structure);
      //清除数据类型
      data_type = 0;
    }
  }
  if (data_type != 0) {
    //数据不完整,数据回溯
    RingList_TraceBack_ReadPointer(&ring_structure);
  }
}


void BufMove(const uint8_t* Arr,uint8_t size) {
  for (uint8_t i=0;i<size;i++) {
    if (Arr[i] != '\0') {
      RingList_Put(&ring_structure,Arr[i]);
    }
  }
}

bool UART_Data_Transfer(UART_Data_TypeDef data_type,uint8_t *data,uint8_t data_num) {
  if (data_type == DATA_TYPE_ERROR || data_num == 0 || data == NULL) {
    return false;
  }
  if (uart_manager.uart->gState != HAL_UART_STATE_READY) {
    return false;
  }
  uart_manager.udata.data_header = UART_DATA_HEADER | (data_type << 3);
  uart_manager.udata.data = data;
  uart_manager.udata.data_len = data_num;
  UART_Send_Data();
  return true;
}

void UART_Send_Data(void) {
  uint8_t data[3] = {0};
  data[0] = uart_manager.udata.data_header;
  data[2] = '\0';
  for (uint8_t i=0;i<uart_manager.udata.data_len;i++) {
    data[1] = uart_manager.udata.data[i];
    HAL_UART_Transmit(uart_manager.uart,data,3,1000);
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
    if(huart == uart_manager.uart){
        if(uart_manager.uart->ErrorCode == HAL_UART_ERROR_ORE){
            __HAL_UART_CLEAR_OREFLAG(uart_manager.uart);
            HAL_UARTEx_ReceiveToIdle_IT(uart_manager.uart,temp_buffer,100);
        }
    }
}

__weak void Command_HandlingCallback(uint8_t command __attribute__((unused))) {

}
__weak void Position_HandlingCallback(uint8_t position __attribute__((unused))) {

}
