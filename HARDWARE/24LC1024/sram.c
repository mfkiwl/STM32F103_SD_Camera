/*****************************************************************************
** �ļ����ƣ�sram.c
** ��    �ܣ�
** �޸���־��
******************************************************************************/
#include "stm32f10x.h"
#include "spi.h"
#include "sram.h"
#include "delay.h"
/******************************************************************************
** �������ƣ�void SRAM_GPIO_Init()  
** ��    �ܣ�IO�ڳ�ʼ��
** �޸���־��
*******************************************************************************/
void SRAM_GPIO_Init(void)
{
  
  GPIO_InitTypeDef GPIO_InitStructure;


  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
 
  GPIO_InitStructure.GPIO_Pin = SRAM_SCK_Pin |  SRAM_SI_Pin | SRAM_CS_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  
  GPIO_InitStructure.GPIO_Pin = SRAM_SO_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_SetBits(GPIOB,SRAM_SCK_Pin|SRAM_SI_Pin|SRAM_CS_Pin|SRAM_SO_Pin); 
   
}

/*******************************************************************************
** �������ƣ�void SRAM_WriteOneByte(u8 DataBuffer)
** ��    �ܣ�����������д��
** �޸���־��
*******************************************************************************/
void SRAM_WriteOneByte(u8 DataBuffer)
{
	u8 i;
    SRAM_SCK_HIGH;
	
  for(i= 0;i < 8;i++)
    {
        SRAM_SCK_LOW;
        if(DataBuffer & 0x80)
        {
            SRAM_SI_HIGH;
        }
        else
        {
            SRAM_SI_LOW;
        }
       DataBuffer <<= 1;
        SRAM_SCK_HIGH;
    }
}

/*******************************************************************************
** �������ƣ�u8 SRAM_ReadOneByte(void)
** ��    �ܣ��½����������
** �޸���־��
*******************************************************************************/
u8 SRAM_ReadOneByte(void)
{
    u8 retValue = 0;
	u8 i;
    SRAM_SCK_HIGH;
    for( i= 0; i< 8; i++)
    {
        retValue <<= 1;
        SRAM_SCK_LOW;
        if((SRAM_SO_Read&0X20)==0X20)
        {
            retValue |= 0x01;
        }
        else
        {
            retValue &= 0xFE;
        }
        SRAM_SCK_HIGH;
    }
     SRAM_SCK_HIGH;
    return (retValue);
}

/*****************************************************************************
** �ļ����ƣ�void SpiFlash_Write_Adr(char *Adr)
** ��    �ܣ�д��ַ
** �޸���־��
******************************************************************************/
void SRAM_Write_Adr(u8 *Adr)
{
    u16 i = 0;
    for(i = 0;i < 3; i++)
    {
        SRAM_WriteOneByte(Adr[i]);
    }
    return ;
}

/*****************************************************************************
** �ļ����ƣ�void SRAM_Write_Data(u8 *pBuffer,int8_t Page_Num,u32 WriteBytesNum)
** ��    �ܣ�������ַд����
** �޸���־��
******************************************************************************/
void SRAM_Write_Data(u8 *pBuffer,u8 Page_Num,u32 WriteBytesNum)
{
    u32 i = 0;
    u8 pcmd[3] = {0};
    SRAM_CS_Enable;
    SRAM_WriteOneByte(0X01);//������
    SRAM_WriteOneByte(0X40);//������
    SRAM_CS_Disable;
    delay_ms(1);
    SRAM_CS_Enable;
    SRAM_WriteOneByte(0X02);//����дģʽ   
    pcmd[0] = 0;
    pcmd[1] = Page_Num;
    pcmd[2] = 0;
    SRAM_Write_Adr(pcmd);//д��ַ

    for(i = 0;i < WriteBytesNum; i++)
    {
        SRAM_WriteOneByte(pBuffer[i]);
    }
    SRAM_CS_Disable;
    return;
}

/*****************************************************************************
** �ļ����ƣ�void SRAM_Read_Data(u8 *pBuffer,u8 Page_Num ,u32 ReadBytesNum)
** ��    �ܣ�
** �޸���־��
******************************************************************************/
void SRAM_Read_Data(u8 *pBuffer,u8 Page_Num ,u32 ReadBytesNum)
{
    u32 i = 0;
    u8 pcmd[3] = {0};
    SRAM_CS_Enable;

    SRAM_WriteOneByte(0X03);//������
    pcmd[0] =0;
    pcmd[1] =Page_Num;
    pcmd[2] = 0;
    SRAM_Write_Adr(pcmd);

    for(  i = 0;i < ReadBytesNum; i++)
    {
       pBuffer[i] = SRAM_ReadOneByte();//��ȡSPIflash��ָ��bytes�ֽ�����
    }
    SRAM_CS_Disable;
    return;
}


