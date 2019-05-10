/*****************************************************************************
** �ļ����ƣ�sram.h   (оƬ23LC1024)
** ��    �ܣ�
** �޸���־��
******************************************************************************/
#ifndef __SRAM_H
#define __SRAM_H

#include "stm32f10x.h"

/*****************************************************************************
** �ܽŶ���
*****************************************************************************/

#define    SRAM_Port_RCC           RCC_APB2Periph_GPIOB
#define    SRAM_Port               GPIOB
#define    SRAM_SCK_Pin            GPIO_Pin_13
#define    SRAM_SI_Pin             GPIO_Pin_15
#define    SRAM_SO_Pin             GPIO_Pin_14
#define    SRAM_CS_Pin             GPIO_Pin_12

#define    SRAM_SCK_HIGH           GPIO_SetBits(SRAM_Port, SRAM_SCK_Pin)           //����SRAMʱ����
#define    SRAM_SCK_LOW            GPIO_ResetBits(SRAM_Port, SRAM_SCK_Pin)           //����SRAMʱ����

#define    SRAM_SI_HIGH            GPIO_SetBits(SRAM_Port, SRAM_SI_Pin)           //SI_SRAMΪ�����
#define    SRAM_SI_LOW             GPIO_ResetBits(SRAM_Port, SRAM_SI_Pin)           //SI_SRAMΪ�����

#define    SRAM_CS_Enable          GPIO_SetBits(SRAM_Port, SRAM_CS_Pin)           //��SRAMƬѡʹ��
#define    SRAM_CS_Disable         GPIO_ResetBits(SRAM_Port, SRAM_CS_Pin)             //�ر�SRAMƬѡʹ��

#define    SRAM_SO_Read            GPIO_ReadInputDataBit(SRAM_Port, SRAM_SO_Pin)


/*******************************************************************************
** ��������
*******************************************************************************/
void SRAM_GPIO_Init(void);
void SRAM_WriteOneByte(u8 DataBuffer);
u8 SRAM_ReadOneByte(void);
void SRAM_Write_Adr(u8 *Adr);
void SRAM_Write_Data(u8 *pBuffer,u8 Page_Num,u32 WriteBytesNum);
void SRAM_Read_Data(u8 *pBuffer,u8 Page_Num ,u32 ReadBytesNum);

#endif

