#ifndef SUPERCAPACITOR_H
#define SUPERCAPACITOR_H

#include "can.h"
#include "typedef.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SCCM_RECEIVE_ID 0x600
#define SCCM_SEND_ID 0x601

#define Charging_ON 1
#define Charging_OFF 0

#define Power_Supply 1
#define Power_NotSupply 0

#pragma anon_unions

#define SuperCapacitorFUN_Init      \
    {                               \
        &SCCM_MsgProcess,           \
            &SCCM_SendMsg,          \
            &SuperCapacitor_Switch, \
            &Check_SuperCapacitor,  \
    }

typedef union
{
    uint8_t data[8];
    struct
    {
        float chassis_power;  /* ���̹��ʣ���λ��W */
        uint8_t chassis_buff; /* ���̹��ʻ��� */
        uint8_t cap_usable;   /* ���ݿ��Խ������ */
        uint8_t cap_cell;     /* ����ʣ�����������ָ��� */
    };
} SCCM_ReceiveData_t;

typedef union
{
    uint8_t data[8];
    struct
    {
        float charge_power;    /* ��繦�ʣ���λ��W ,��Χ 0-80W */
        uint8_t charge_enable; /* ���ʹ�� */
        uint8_t is_cap_output; /* ʹ�õ��ݹ��� */
    };
} SCCM_SendData_t;

typedef struct
{
    SCCM_ReceiveData_t ReceiveData;
    SCCM_SendData_t SendData;

    bool EnableCapacitor;
    uint8_t InfoUpdateFlag;   //��Ϣ��ȡ���±�־
    uint16_t InfoUpdateFrame; //֡��
    uint8_t OffLineFlag;      //�豸���߱�־
} Supercapacitor_t;

typedef struct
{
    void (*SCCM_MsgProcess)(Can_Export_Data_t RxMessage);
    void (*SCCM_SendMsg)(float Charge_power, uint8_t Charge_enable, uint8_t Cap_enable);
    void (*SuperCapacitor_Switch)(bool state);
    void (*Check_SuperCapacitor)(void);
} SuperCapacitorFUN_t;

extern uint8_t Cap_Tick_5MS;
extern Supercapacitor_t supercapacitor;
extern SuperCapacitorFUN_t SuperCapacitorFUN;

#endif
