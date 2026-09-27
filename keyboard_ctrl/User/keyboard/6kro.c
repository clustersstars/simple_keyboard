#include "6kro.h"

static void push(_6KRO_BufferManage* buffer_manage,uint8_t key);
static void clearBuffer(_6KRO_Buffer* _6kro_buffer);
static void fillReport(_6KRO_BufferManage* buffer_manage,_6KRO_Report* report);

bool _6KRO_BufferManage_Init(_6KRO_BufferManage* buffer_manage){
    if(buffer_manage == NULL){
        return false;
    }
    // buffer_manage->function_key_flag = 0;
    // buffer_manage->function_key      = 0;
    // buffer_manage->check             = 0;
    // buffer_manage->temp_check        = 0;
    // buffer_manage->hash_structure                     = hash_structure;
    static _6KRO_Buffer _6kro_instant_buffer          = {0};
    buffer_manage->_6kro_instant_buffer               = &_6kro_instant_buffer;
    buffer_manage->_6kro_instant_buffer->buffer_size  = 6;
    buffer_manage->_6kro_instant_buffer->buffer_top   = 0;
    buffer_manage->_6kro_instant_buffer->modifier_key = 0;
    static uint8_t static_instant_buffer[6]           = {0};
    buffer_manage->_6kro_instant_buffer->buffer       = static_instant_buffer;
    buffer_manage->push        = push;
    buffer_manage->clearBuffer = clearBuffer;
    return true;
}

bool _6KRO_Report_Init(_6KRO_Report* report){
    if(report == NULL){
        return false;
    }
    static uint8_t static_buffer1[_6KRO_REPORT_BUFFER_SIZE] = {0};
    static uint8_t static_buffer2[_6KRO_REPORT_BUFFER_SIZE] = {0};
    report->_6kro_report_channel1 = static_buffer1;
    report->_6kro_report_channel2 = static_buffer2;
    report->channel_selected = 0;
    report->fillReport = fillReport;
    return true;
}

void clearBuffer(_6KRO_Buffer* _6kro_buffer){
    _6kro_buffer->buffer_top   = 0;
    _6kro_buffer->modifier_key = 0;
    // buffer_manage->function_key_flag = 0;
    // buffer_manage->function_key      = 0;
    memset(_6kro_buffer->buffer,0,_6kro_buffer->buffer_size);
}

void push(_6KRO_BufferManage* buffer_manage,uint8_t key){
    // if(key == 0xE8){
    //     buffer_manage->function_key_flag = 1;
    //     return;
    // }
    // if(HashMark_Add(buffer_manage->hash_structure,key)){
    if(key >= 0xE0 && key <= 0xE7){
        buffer_manage->_6kro_instant_buffer->modifier_key |= (1 << (key-0xE0));
        return;
    }else{
        if(buffer_manage->_6kro_instant_buffer->buffer_top < _6KRO_BUFFER_SIZE){
            buffer_manage->_6kro_instant_buffer->buffer[buffer_manage->_6kro_instant_buffer->buffer_top++] = key;
        }
    }
    // }
    // if(buffer_manage->function_key_flag){
    //     buffer_manage->function_key = key;
    // }


}

void fillReport(_6KRO_BufferManage* buffer_manage,_6KRO_Report* report){
    if(report->channel_selected){
        report->_6kro_report_channel1[0] = buffer_manage->_6kro_instant_buffer->modifier_key;
        memcpy(report->_6kro_report_channel1+2,buffer_manage->_6kro_instant_buffer->buffer,_6KRO_BUFFER_SIZE);
        memset(report->_6kro_report_channel2,0,_6KRO_REPORT_BUFFER_SIZE);
    }else{
        report->_6kro_report_channel2[0] = buffer_manage->_6kro_instant_buffer->modifier_key;
        memcpy(report->_6kro_report_channel2+2,buffer_manage->_6kro_instant_buffer->buffer,_6KRO_BUFFER_SIZE);
        memset(report->_6kro_report_channel1,0,_6KRO_REPORT_BUFFER_SIZE);
    }
    report->channel_selected = !report->channel_selected;
}
