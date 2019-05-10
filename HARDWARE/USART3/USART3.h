#ifndef __USART3_H
#define __USART3_H

#include "stm32f10x.h"
#include <stdio.h>

#define  USART3_RX_LEN 2048  //֧�����2K�Ĵ���д��

extern uint16_t USART3_RX_STA;  //���ݽ��ձ�־λ
extern uint8_t USART3_RX_BUF[USART3_RX_LEN];  //�������ݻ�������

void uart3_init(u32 baud);
uint16_t crc16(char* puchMsg,uint8_t usDataLen);

#endif


