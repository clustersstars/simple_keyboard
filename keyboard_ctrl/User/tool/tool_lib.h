#ifndef TOOL_LIB_H_
#define TOOL_LIB_H_
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef enum{
    OK = (uint8_t)0,
    EEROR,
    BUSY,
    TIMEOUT
}StatusTypeDef;

typedef enum{
    Equal = (uint8_t)0,
    No_Equal
}CompareStatusTypeDef;

//创建一个标记数组初始化
typedef struct{
    volatile uint8_t mark_size;//标记数组大小
    uint8_t *mark_list;//标记数组指针
}MarkList_InitTypeDef;

//创建一个环形数组
typedef struct{
    volatile uint8_t list_size;//数组的大小 //实际只能装list_size-1
    volatile uint8_t list_read_pointer;
    volatile uint8_t list_write_pointer;
    volatile uint8_t list_read_temp_pointer;
    uint8_t *ring_list;//数组指针
}RingList_InitTypeDef;

//extern
bool MarkList_Init(MarkList_InitTypeDef* mark_structure,uint8_t mark_size,uint8_t* mark_list);
void MarkList_Add(MarkList_InitTypeDef* mark_structure,uint8_t elem);
bool MarkList_IsExist(MarkList_InitTypeDef* mark_structure,uint8_t elem);
void MarkList_MarkClear(MarkList_InitTypeDef* mark_structure);

bool RingList_Init(RingList_InitTypeDef* ring_structure,uint8_t list_size,uint8_t* list);
bool RingList_Put(RingList_InitTypeDef* ring_structure,uint8_t element);
int16_t RingList_Pop(RingList_InitTypeDef* ring_structure);
void RingList_Update_TempReadPointer(RingList_InitTypeDef* ring_structure);
void RingList_TraceBack_ReadPointer(RingList_InitTypeDef* ring_structure);
bool RingList_IsFull(const RingList_InitTypeDef* ring_structure);
bool RingList_IsEmpty(const RingList_InitTypeDef* ring_structure);
void RingList_ListClear(RingList_InitTypeDef* ring_structure);
#endif
