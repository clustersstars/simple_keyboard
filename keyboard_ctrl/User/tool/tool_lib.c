#include "tool_lib.h"

//标记数组
bool MarkList_Init(MarkList_InitTypeDef* mark_structure,uint8_t mark_size,uint8_t* mark_list){
    mark_structure->mark_size = mark_size;
    if(mark_list == NULL || mark_size == 0){
        return false;
    }
    mark_structure->mark_list = mark_list;
    return true;
}

//添加元素到list
void MarkList_Add(MarkList_InitTypeDef* mark_structure,uint8_t elem){
    mark_structure->mark_list[elem] = 1;
}

bool MarkList_IsExist(MarkList_InitTypeDef* mark_structure,uint8_t elem){
    if(mark_structure->mark_list[elem] == 0){
        return false;
    }
    return true;
}

//标记数组清空
void MarkList_MarkClear(MarkList_InitTypeDef* mark_structure){
    memset(mark_structure->mark_list,0,mark_structure->mark_size);
}




//环形数组
bool RingList_Init(RingList_InitTypeDef* ring_structure,uint8_t list_size,uint8_t* list){
    if(list == NULL || list_size == 0){
        return false;
    }
    ring_structure->list_size          = list_size;
    ring_structure->list_read_pointer  = 0;
    ring_structure->list_write_pointer = 0;
    ring_structure->list_read_temp_pointer = 0;
    ring_structure->ring_list          = list;
    return true;
}

bool RingList_Put(RingList_InitTypeDef* ring_structure,uint8_t element){
    //判断数组是否已满
    if (RingList_IsFull(ring_structure)) {
        return false;
    }
    //入队列
    ring_structure->ring_list[ring_structure->list_write_pointer] = element;
    ring_structure->list_write_pointer = (ring_structure->list_write_pointer+1)%ring_structure->list_size;
    return true;
}

int16_t RingList_Pop(RingList_InitTypeDef* ring_structure){
    uint8_t element = 0;
    //判断数组是否为空
    if (RingList_IsEmpty(ring_structure)) {
        return -1;
    }
    //
    element = ring_structure->ring_list[ring_structure->list_read_pointer];
    ring_structure->list_read_pointer = (ring_structure->list_read_pointer+1)%ring_structure->list_size;
    return element;
}

void RingList_Update_TempReadPointer(RingList_InitTypeDef* ring_structure) {
    ring_structure->list_read_temp_pointer = ring_structure->list_read_pointer;
}

void RingList_TraceBack_ReadPointer(RingList_InitTypeDef* ring_structure) {
    ring_structure->list_read_pointer = ring_structure->list_read_temp_pointer;
}

bool RingList_IsFull(const RingList_InitTypeDef *ring_structure) {
    if((ring_structure->list_write_pointer+1)%ring_structure->list_size != ring_structure->list_read_temp_pointer){
        return false;
    }
    return true;
}

bool RingList_IsEmpty(const RingList_InitTypeDef *ring_structure) {
    if (ring_structure->list_read_pointer != ring_structure->list_write_pointer) {
        return false;
    }
    return true;
}

void RingList_ListClear(RingList_InitTypeDef *ring_structure){
    memset(ring_structure->ring_list,0,ring_structure->list_size);
    ring_structure->list_read_pointer = 0;
    ring_structure->list_write_pointer = 0;
    ring_structure->list_read_temp_pointer = 0;
}




