#include "keyboard.h"

#include "main.h"
#include "tool_lib.h"
#include "uart_proto.h"
//extern引用
extern TIM_HandleTypeDef htim4;

//常量声明
static const uint8_t keyMap[KEYBOARDROW][KEYBOARDCOL] = {
  //ROW1
  {KB_ESC,0x01,KB_F1,KB_F2,KB_F3,KB_F4,0x01,KB_F5,KB_F6,KB_F7,KB_F8,KB_F9,KB_F10,KB_F11,KB_F12,KB_DELETE},
  //ROW2
  {KB_TILDE,KB_1,KB_2,KB_3,KB_4,KB_5,KB_6,KB_7,KB_8,KB_9,KB_0,KB_MIUS,KB_EQUAL,0x01,KB_BACKSPACE,KB_PAGEUP},
  //ROW3
  {KB_TAB,0x01,KB_Q,KB_W,KB_E,KB_R,KB_T,KB_Y,KB_U,KB_I,KB_O,KB_P,KB_LSQUBRACKET,KB_RSQUBRACKET,KB_BACKSLASH,KB_PAGEDOWN},
  //ROW4
  {KB_CAPSLOCK,0x01,KB_A,KB_S,KB_D,KB_F,KB_G,KB_H,KB_J,KB_K,KB_L,KB_SEMICOLON,KB_APOSTROPHE,KB_ENTER,0x01,KB_HOME},
  //ROW5
  {0x01,Left_Shift,KB_Z,KB_X,KB_C,KB_V,KB_B,KB_N,KB_M,KB_COMMA,KB_FULLSTOP,KB_SLASH,0x01,Right_Shift,KB_UARROW,KB_END},
  //ROW6
  {Left_Ctrl,Left_GUI,0x01,Left_Alt,0x01,0x01,KB_SPACE,0x01,0x01,0x01,Right_Alt,Right_Ctrl,Fn,KB_LARROW,KB_DARROW,KB_RARROW},
};
static const uint8_t keyPosMap[KEYBOARDROW][KEYBOARDCOL] = {
  //ROW1
  {COL0,0,COL1,COL2,COL3,COL4,0,COL5,COL6,COL7,COL8,COL9,COL10,COL11,COL12,COL13},
  //ROW2
  {COL0,COL1,COL2,COL3,COL4,COL5,COL6,COL7,COL8,COL9,COL10,COL11,0,COL12,COL13},
  //ROW3
  {COL0,0,COL1,COL2,COL3,COL4,COL5,COL6,COL7,COL8,COL9,COL10,COL11,COL12,COL13,COL14},
  //ROW4
  {COL0,0,COL1,COL2,COL3,COL4,COL5,COL6,COL7,COL8,COL9,COL10,COL11,COL12,0,COL13},
  //ROW5
  {0,COL0,COL1,COL2,COL3,COL4,COL5,COL6,COL7,COL8,COL9,COL10,0,COL11,COL12,COL13},
  //ROW6
  {COL0,COL1,0,COL2,0,0,COL3,0,0,0,COL4,COL5,COL6,COL7,COL8,COL9},
};

static const uint16_t ROW_GPIO_PIN[KEYBOARDROW] = {ROW0_Pin,ROW1_Pin,ROW2_Pin,ROW3_Pin,ROW4_Pin,ROW5_Pin};
static volatile GPIO_TypeDef* const ROW_GPIOx[KEYBOARDROW] = {ROW0_GPIO_Port,ROW1_GPIO_Port,ROW2_GPIO_Port,ROW3_GPIO_Port,ROW4_GPIO_Port,ROW5_GPIO_Port};
static volatile GPIO_TypeDef* const COL_GPIOx[KEYBOARDCOL] = {
  GPIOA,GPIOA,GPIOA,GPIOA,GPIOA,GPIOA,GPIOA,GPIOA,GPIOC,GPIOC,GPIOC,GPIOC,GPIOC,GPIOB,GPIOB,GPIOB
};

#ifdef KEYBOARD_RGB

static const uint8_t rgb_commands[13] = {
  RGB_COLOR_BLUE,
  RGB_COLOR_CYAN,
  RGB_COLOR_ORANGE,
  RGB_COLOR_PINK,
  RGB_COLOR_RED,
  RGB_COLOR_VIOLET,
  RGB_COLOR_WHITE,
  RGB_SWITCH_OPEN,
  RGB_MODE_STATIC,
  RGB_MODE_PRESS,
  RGB_MODE_PRESS_RANDOM,
  RGB_MODE_LOVE,
  RGB_SWITCH_CLOSE
};

static const RGB_CommandTypeDef rgb_types[13] = {
  COLOR_TYPE,
  COLOR_TYPE,
  COLOR_TYPE,
  COLOR_TYPE,
  COLOR_TYPE,
  COLOR_TYPE,
  COLOR_TYPE,
  SWITCH_TYPE,
  MODE_TYPE,
  MODE_TYPE,
  MODE_TYPE,
  MODE_TYPE,
  SWITCH_TYPE
};

#endif

//变量声明


Keyboard_TypeManage             keyboard_type_manage;
static BufferManage             buffer_manage;
static ReportManage             report_manage;
#ifdef KEYBOARD_RGB
static ModeManage               mode_manage;
static PosManage                position_manage;
#endif

TIM_HandleTypeDef *keyboard_tim_1ms = &htim4;   

//函数声明
static bool BufferManage_Init(Keyboard_TypeManage* keyboard_type_manage,BufferManage *buffer_manage);
static bool ReportManage_Init(Keyboard_TypeManage* keyboard_type_manage,ReportManage *report_manage);

#ifdef KEYBOARD_RGB
static bool ModeManage_Init(Keyboard_TypeManage* keyboard_type_manage,ModeManage* mode_manage);
static bool PositionManage_Init(Keyboard_TypeManage* keyboard_type_manage,PosManage* position_manage);
static void PosList_Push(uint8_t position);
static void PosList_Clear(PosManage* position_manage);
static int16_t verifyCommand(uint8_t command);
static void pushCommandToTempMode(uint8_t command);
static void popCommandToTempMode(RGB_CommandTypeDef command_type);
static void pushCommandToWaitResponseMode(RGB_CommandTypeDef command_type,uint8_t command);
static void popCommandToWaitResponseMode(RGB_CommandTypeDef command_type);
#endif

static void scanKeyboardRow(void);//行扫描
static bool checkCompare(BufferManage* buffer_manage);
static void updateReport(Keyboard_TypeManage* keyboard_type_manage);
static void push(Keyboard_TypeManage* keyboard_type_manage,uint8_t key,uint8_t position);
static void clear(Keyboard_TypeManage* keyboard_type_manage);
static void clearBuffer(BufferManage* buffer_manage);


void KeyboardInit(void){
  keyboard_type_manage.SCAN_ROW = 0;
  keyboard_type_manage.threshold = K_MAX_THRESHOLD;
  keyboard_type_manage.KeyboardState = KEYBOARD_IDLE;
  BufferManage_Init(&keyboard_type_manage,&buffer_manage);//初始化按键缓冲
  ReportManage_Init(&keyboard_type_manage,&report_manage);//初始化USB报告
  #ifdef KEYBOARD_RGB
  ModeManage_Init(&keyboard_type_manage,&mode_manage);//初始化rgb模式
  PositionManage_Init(&keyboard_type_manage,&position_manage);//初始化按键位置存储
  #endif
  HAL_TIM_Base_Start_IT(keyboard_tim_1ms);
}

bool BufferManage_Init(Keyboard_TypeManage *type_manage,BufferManage *buffer_manage){
  if(type_manage == NULL || buffer_manage == NULL){
    return false;
  }
  buffer_manage->check             = 0;
  buffer_manage->temp_check        = 0;
  buffer_manage->function_key_flag = 0;
  buffer_manage->function_key      = 0;
  if(!NKRO_BufferManage_Init(&buffer_manage->nkro_buffer_manage)){
    return false;
  }
  type_manage->buffer_manage = buffer_manage;
  return true;
}
bool ReportManage_Init(Keyboard_TypeManage* keyboard_type_manage,ReportManage *report_manage){
  if(keyboard_type_manage == NULL || report_manage == NULL){
    return false;
  }
  if(!NKRO_Report_Init(&report_manage->nkro_report)){
    return false;
  }
  report_manage->usb_report    = report_manage->nkro_report.nkro_report_channel1;
  keyboard_type_manage->report_manage = report_manage;
  return true;
}

#ifdef KEYBOARD_RGB
bool ModeManage_Init(Keyboard_TypeManage* keyboard_type_manage,ModeManage* mode_manage){
  if(keyboard_type_manage == NULL || mode_manage == NULL){
    return false;
  }
  mode_manage->rgb_mode[0] = RGB_SWITCH_CLOSE;//初始化rgb_switch
  mode_manage->rgb_mode[1] = RGB_COLOR_ORANGE;//初始化rgb_color
  mode_manage->rgb_mode[2] = RGB_MODE_STATIC;//初始化rgb_mode
  mode_manage->pending_quantity = 0;//初始化待处理命令数量
  keyboard_type_manage->mode_manage = mode_manage;
  return true;
}

bool PositionManage_Init(Keyboard_TypeManage* keyboard_type_manage,PosManage* position_manage){
  if(keyboard_type_manage == NULL || position_manage == NULL){
    return false;
  }
  position_manage->pos_list.pos_top = 0;
  memset((uint8_t*)position_manage->pos_list.list,0,sizeof(position_manage->pos_list.list));
  position_manage->pos_update_flag = 0;
  static uint8_t ring_list[K_POSITION_LIST_SIZE] = {0};
  RingList_Init(&position_manage->ring_structure,K_POSITION_LIST_SIZE,ring_list);
  keyboard_type_manage->position_manage = position_manage;
  return true;
}
#endif

void keyboardStart(void){
  //键盘空闲
  if(keyboard_type_manage.KeyboardState == KEYBOARD_IDLE){
  }
  //扫描准备
  if(keyboard_type_manage.KeyboardState == KEYBOARD_READY_SCAN){
    setKeyboardRowPin(GPIO_PIN_SET);
    keyboard_type_manage.KeyboardState = KEYBOARD_READY_FINISH;
  }
  //准备完成 -> 开始扫描
  if(keyboard_type_manage.KeyboardState == KEYBOARD_READY_FINISH){
    keyboard_type_manage.KeyboardState = KEYBOARD_SCAN_RUNNING;
    scanKeyboardRow();
    keyboard_type_manage.KeyboardState = KEYBOARD_SCAN_OVER;
  }
  //扫描结束
  if(keyboard_type_manage.KeyboardState == KEYBOARD_SCAN_OVER){
    //判断按键是否变化
    if(checkCompare(keyboard_type_manage.buffer_manage)){
      if(keyboard_type_manage.threshold > 0){
        keyboard_type_manage.threshold--;
      }
    }else{
      //重赋新校验值
      keyboard_type_manage.buffer_manage->check = keyboard_type_manage.buffer_manage->temp_check;
      //重置阈值
      keyboard_type_manage.threshold = K_MAX_THRESHOLD;
      #ifdef KEYBOARD_RGB
      //位置更新标志置位
      keyboard_type_manage.position_manage->pos_update_flag = 1;
      #endif
    }
    if(keyboard_type_manage.threshold == 0){
      //更新报表
      updateReport(&keyboard_type_manage);
      if(keyboard_type_manage.buffer_manage->check == 0){//判断键盘是否进入空闲状态
        keyboard_type_manage.KeyboardState = KEYBOARD_IDLE;
        //恢复行的初始状态(低电平)
        setKeyboardRowPin(GPIO_PIN_RESET);
        return;
      }
      #ifdef KEYBOARD_RGB
      if(keyboard_type_manage.position_manage->pos_update_flag){//将pos_list的值转移到ring_list
        for(uint8_t i=0;i<keyboard_type_manage.position_manage->pos_list.pos_top;i++){
          if(RingList_Put(&keyboard_type_manage.position_manage->ring_structure,keyboard_type_manage.position_manage->pos_list.list[i]) == false){
            break;
          }
        }
        keyboard_type_manage.position_manage->pos_update_flag = 0;
      }
      #endif
    }
    //清理数据缓冲
    clear(&keyboard_type_manage);
    keyboard_type_manage.KeyboardState = KEYBOARD_READY_FINISH;
  }
}

void setKeyboardRowPin(GPIO_PinState PinState){//将键盘行全部置1,或置0
  for(uint8_t row=0;row<KEYBOARDROW;row++){
    HAL_GPIO_WritePin((GPIO_TypeDef*)ROW_GPIOx[row],ROW_GPIO_PIN[row],PinState);
  }
}
void scanKeyboardRow(void){
  keyboard_type_manage.SCAN_ROW = 0;
  for(uint8_t row=0;row<KEYBOARDROW;row++){
    HAL_GPIO_WritePin((GPIO_TypeDef*)ROW_GPIOx[row],ROW_GPIO_PIN[row],GPIO_PIN_RESET); //将该行拉低
    for(uint8_t col=0;col<KEYBOARDCOL;col++){
      if((COL_GPIOx[col]->IDR & (uint32_t)(1 << col)) == (uint32_t)GPIO_PIN_RESET){ //判断该列是否为低电平
        push(&keyboard_type_manage,keyMap[keyboard_type_manage.SCAN_ROW][col],keyboard_type_manage.SCAN_ROW*16+keyPosMap[keyboard_type_manage.SCAN_ROW][col]);
      }
    }
    HAL_GPIO_WritePin((GPIO_TypeDef*)ROW_GPIOx[row],ROW_GPIO_PIN[row],GPIO_PIN_SET);   //恢复拉高
    keyboard_type_manage.SCAN_ROW = (keyboard_type_manage.SCAN_ROW+1) % KEYBOARDROW;//更新扫描行
  }
}

void push(Keyboard_TypeManage* keyboard_type_manage,uint8_t key,uint8_t position){
  if(key == KB_ERROR) {
    return;
  }
  if(key == Fn){
    keyboard_type_manage->buffer_manage->function_key_flag = 1;
    return;
  }
  keyboard_type_manage->buffer_manage->function_key = key;
  keyboard_type_manage->buffer_manage->temp_check ^= key;
  keyboard_type_manage->buffer_manage->nkro_buffer_manage.push(&keyboard_type_manage->buffer_manage->nkro_buffer_manage,key);
  #ifdef KEYBOARD_RGB
  PosList_Push(position);//添加按键位置入栈
  #endif
}

void clear(Keyboard_TypeManage* keyboard_type_manage){
  clearBuffer(keyboard_type_manage->buffer_manage);
  #ifdef KEYBOARD_RGB
  PosList_Clear(keyboard_type_manage->position_manage);
  #endif
}

void updateReport(Keyboard_TypeManage* keyboard_type_manage){
  if(keyboard_type_manage->buffer_manage->function_key_flag && keyboard_type_manage->buffer_manage->function_key != 0){
    #ifdef KEYBOARD_RGB
    pushCommandToTempMode(keyboard_type_manage->buffer_manage->function_key);
    #endif
    return;
  }
  keyboard_type_manage->report_manage->usb_report = keyboard_type_manage->report_manage->nkro_report.fillReport(&keyboard_type_manage->buffer_manage->nkro_buffer_manage,&keyboard_type_manage->report_manage->nkro_report);
}

void clearBuffer(BufferManage* buffer_manage){
  buffer_manage->function_key_flag = 0;
  buffer_manage->function_key      = 0;
  buffer_manage->temp_check        = 0;
  buffer_manage->nkro_buffer_manage.clearBuffer(buffer_manage->nkro_buffer_manage.nkro_instant_buffer);
}

bool checkCompare(BufferManage* buffer_manage){
  if(buffer_manage->check != buffer_manage->temp_check){
    return false;
  }
  return true;
}

#ifdef KEYBOARD_RGB
void pushCommandToTempMode(uint8_t command) {
  int type_index = 0;
  if((type_index = verifyCommand(command)) == -1){
    return;
  }
  if (keyboard_type_manage.mode_manage->temp_mode[rgb_types[type_index]] == command) {
    //该指令已在temp_mode中
    return;
  }
  if (keyboard_type_manage.mode_manage->rgb_mode[rgb_types[type_index]] == command) {
    //该指令已激活
    return;
  }
  keyboard_type_manage.mode_manage->temp_mode[rgb_types[type_index]] = command;
  keyboard_type_manage.mode_manage->pending_quantity++;
}

void popCommandToTempMode(RGB_CommandTypeDef command_type) {
  keyboard_type_manage.mode_manage->temp_mode[command_type] = 0;
}

void pushCommandToWaitResponseMode(RGB_CommandTypeDef command_type,uint8_t command) {
  keyboard_type_manage.mode_manage->wait_response_mode[command_type].rgb_command = command;
  keyboard_type_manage.mode_manage->wait_response_mode[command_type].repeat_transmit_count = 0;
  keyboard_type_manage.mode_manage->wait_response_mode[command_type].time = HAL_GetTick();
}

void popCommandToWaitResponseMode(RGB_CommandTypeDef command_type) {
  keyboard_type_manage.mode_manage->wait_response_mode[command_type].rgb_command = 0;
  keyboard_type_manage.mode_manage->wait_response_mode[command_type].repeat_transmit_count = 0;
  keyboard_type_manage.mode_manage->wait_response_mode[command_type].time = 0;
}

void PosList_Push(uint8_t position){
  if(keyboard_type_manage.position_manage->pos_list.pos_top >= K_POSITION_LIST_SIZE){
    return;
  }
  keyboard_type_manage.position_manage->pos_list.list[keyboard_type_manage.position_manage->pos_list.pos_top] = position;
  keyboard_type_manage.position_manage->pos_list.pos_top++;
}

void PosList_Clear(PosManage *position_manage){
  position_manage->pos_list.pos_top = 0;
  memset((uint8_t*)position_manage->pos_list.list,0,sizeof(position_manage->pos_list));
}

//验证指令是否存在
int16_t verifyCommand(uint8_t command){
  int16_t left = 0;
  int16_t right = sizeof(rgb_commands)/sizeof(uint8_t) - 1;
  int16_t mid = 0;
  while (left <= right) {
        mid = left + (right - left) / 2;
        if (rgb_commands[mid] == command) {
            return mid;
        } else if (rgb_commands[mid] < command) {
            // 中间值 < 目标值，去右半部分查找
            left = mid + 1;
        } else {
            // 中间值 > 目标值，去左半部分查
            right = mid - 1;
        }
    }
    // 循环结束未找到
    return -1;
}

void transmitCommand(void){
  if(keyboard_type_manage.mode_manage->pending_quantity > 0){
    for(uint8_t i=0;i<3;i++){
      if(keyboard_type_manage.mode_manage->temp_mode[i] != 0){
        //发送命令
        UART_Data_Transfer(DATA_TYPE_COMMAND,&keyboard_type_manage.mode_manage->temp_mode[i],1);
        //
        pushCommandToWaitResponseMode(i,keyboard_type_manage.mode_manage->temp_mode[i]);
        //
        popCommandToTempMode(i);
      }
    }
    keyboard_type_manage.mode_manage->pending_quantity--;
  }
}

void transmitPosition(void){
  uint8_t data = 0;
  while(!RingList_IsEmpty(&keyboard_type_manage.position_manage->ring_structure)) {
    //取数据
    data = (uint8_t)RingList_Pop(&keyboard_type_manage.position_manage->ring_structure);
    //更新temp_read_pointer;
    RingList_Update_TempReadPointer(&keyboard_type_manage.position_manage->ring_structure);
    UART_Data_Transfer(DATA_TYPE_POSITION,&data,1);
  }
}

void commandTimeoutHandler(void) {
  for(uint8_t i=0;i<3;i++) {
    if (keyboard_type_manage.mode_manage->wait_response_mode[i].rgb_command != 0 && HAL_GetTick() - keyboard_type_manage.mode_manage->wait_response_mode[i].time > COMMAND_TIMEOUT) {
      if (keyboard_type_manage.mode_manage->wait_response_mode[i].repeat_transmit_count <= COMMAND_SEND_REPEAT_COUNT-1) {
        //重发
        UART_Data_Transfer(DATA_TYPE_COMMAND,&keyboard_type_manage.mode_manage->wait_response_mode[i].rgb_command,1);
        keyboard_type_manage.mode_manage->wait_response_mode[i].time = HAL_GetTick();
        keyboard_type_manage.mode_manage->wait_response_mode[i].repeat_transmit_count++;
      }else {
        //清除
        popCommandToWaitResponseMode(i);
        keyboard_type_manage.mode_manage->wait_response_mode[i].repeat_transmit_count = 0;
      }
    }
  }
}

void Command_HandlingCallback(uint8_t command) {
  //处理指令
  int type_index = 0;
  if((type_index = verifyCommand(command)) == -1){
    return;
  }
  if (keyboard_type_manage.mode_manage->wait_response_mode[rgb_types[type_index]].rgb_command == command) {
    //清除wait_response_mode[type_index]
    popCommandToWaitResponseMode(rgb_types[type_index]);
    //
    keyboard_type_manage.mode_manage->rgb_mode[rgb_types[type_index]] = command;
  }
}

void Position_HandlingCallback(uint8_t position) {
  //处理位置
}
#endif
