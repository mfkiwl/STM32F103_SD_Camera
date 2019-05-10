#include "string.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "USART3.h"
#include "apps.h"
#include "MMC_SD.h"
#include "ff.h"
#include "TEST_FATFS.h"
#include "ov2640.h"
#include "led.h"
#include "ram.h"

uint8_t times=0; // �����ʱ��
uint8_t ov2640_framebuf[60000];//֡����
//uint8_t jpg_frame_data_byte_new = 0, jpg_frame_data_byte_old = 0;
//uint8_t jpg_frame_data_start = 0;
//uint32_t jpg_frame_data_length = 0;

char ovname[40]; //Photo_Times �����ļ�·��
char ovtemp[40]; //��Ƭ

uint8_t Read_times1[5];//��һ�����ñ���
uint8_t Read_times2[5];//�ڶ������ñ���
uint8_t Read_times3[5];//���������ñ���
uint8_t Read_times4[5];//���Ķ����ñ���
uint8_t Read_times5[5];//��������ñ���
uint8_t Read_times6[5];//���������ñ���

//uint8_t *jpeg_buf[59000];  //ͼƬ��������

//OV2640�ٶȿ���
//����LCD�ֱ��ʵĲ�ͬ�����ò�ͬ�Ĳ���
void ov2640_speed_ctrl(void)
{
    u8 clkdiv,pclkdiv;			//ʱ�ӷ�Ƶϵ����PCLK��Ƶϵ��
    clkdiv=15;  //15  20
    pclkdiv=10;  //4   25
    SCCB_WR_Reg(0XFF,0X00);
    SCCB_WR_Reg(0XD3,pclkdiv);	//����PCLK��Ƶ
    SCCB_WR_Reg(0XFF,0X01);
    SCCB_WR_Reg(0X11,clkdiv);	//����CLK��Ƶ
}

//OV2640����jpgͼƬ
//pname:Ҫ�����jpg��Ƭ·��+����
//����ֵ:0,�ɹ�
//    ����,�������
void ov2640_jpg_photo(void)
{
    u32 i=0;
    //u8 jpg_frame_data_temp;
    u32 jpeglen=0;
    u8* pbuf;
    uint8_t j=0;
    //jpg_frame_data_length = 0;
    OV2640_JPEG_Mode();//�л�ΪJPEGģʽ
    OV2640_OutSize_Set(OV2640_JPEG_WIDTH,OV2640_JPEG_HEIGHT); //����ͼƬ�ߴ�
    //ov2640_speed_ctrl();
    SCCB_WR_Reg(0XFF,0X00);
    SCCB_WR_Reg(0XD3,30);
    SCCB_WR_Reg(0XFF,0X01);
    SCCB_WR_Reg(0X11,0X1);
    LED_Green=1;  //��
    for(i=0; i<10; i++) {	//����10֡���ȴ�OV2640�Զ����ںã��ع��ƽ��֮��ģ�
        while(OV2640_VSYNC==1);
        while(OV2640_VSYNC==0);
    }
    while(OV2640_VSYNC==1) {	//��ʼ�ɼ�jpeg����
        while(OV2640_HREF) {
            while(OV2640_PCLK==0);
            ov2640_framebuf[jpeglen]=OV2640_DATA;
            while(OV2640_PCLK==1);
            jpeglen++;
        }
    }
    LED_Green=0;  //��
    printf("jpeg data size: %d \r\n",jpeglen);
    pbuf = (uint8_t *)ov2640_framebuf;
    for(i=0; i<jpeglen; i++) { //����0XFF,0XD8
        if((pbuf[i]==0XFF)&&(pbuf[i+1]==0XD8))
            break;
    }
    if(i==jpeglen) {
        for(j=0; j<5; j++) {
            LED_Red=1;
            delay_ms(50);
            LED_Green=0;
            delay_ms(50);
        }
        printf("û�ҵ�0XFF,0XD8......\r\n");//û�ҵ�0XFF,0XD8
    } else {	//�ҵ���
        LED_Green=0;  //��
        pbuf+=i;//ƫ�Ƶ�0XFF,0XD8��
        sprintf(ovname,"Photo_Times:%4d\r\n",fphotos); // �����ļ������ݼ�¼���6
        Test_f_writefile("config.ini",ovname,rmove_data_conf,strlen(ovname)); //�����ļ�д�� ����Read_times
        printf("Write Photos:%4d OK!\r\n",fphotos);
        sprintf(ovtemp,"picture/IMG_%d.jpg",fphotos);
        Test_f_writefile(ovtemp,(char *)pbuf,0,jpeglen-i); //�����ļ�д��
        printf("photo data is: %d \r\n",jpeglen-i);
    }
    /*
    while(OV2640_VSYNC == 1) {
        while(OV2640_HREF) {
            while(OV2640_PCLK == 0);
            jpg_frame_data_temp = (u8)OV2640_DATA;
            jpg_frame_data_byte_new = jpg_frame_data_temp;
            if( (jpg_frame_data_start == 0) && (jpg_frame_data_byte_old == 0xFF) && (jpg_frame_data_byte_new == 0xD8) ) {
                jpeg_buf[0] = 0xFF;
                jpeg_buf[1] = 0xD8;
                jpg_frame_data_start = 1;
                jpg_frame_data_length = 2;
            } else if(jpg_frame_data_start == 1) {
                if(jpg_frame_data_length < 0XE678) {
                    jpeg_buf[jpg_frame_data_length] = jpg_frame_data_byte_new;
                    jpg_frame_data_length ++;
                    if( (jpg_frame_data_byte_old == 0xFF) && (jpg_frame_data_byte_new == 0xD9) ) {
                        jpg_frame_data_start = 0;
                    }
                }
            }
            jpg_frame_data_byte_old = jpg_frame_data_byte_new;
            while(OV2640_PCLK == 1);
        }
    }

    LED_Green=0;  //��
    sprintf(ovname,"Photo_Times:%4d\r\n",fphotos); // �����ļ������ݼ�¼���
    Test_f_writefile("config.ini",ovname,rmove_data_conf,strlen(ovname)); //�����ļ�д�� ����Read_times
    printf("Write Photos:%4d OK!\r\n",fphotos);
    sprintf(ovtemp,"picture/IMG_%d.jpg",fphotos);
    Test_f_writefile(ovtemp,(char *)jpeg_buf,0,jpg_frame_data_length); //�����ļ�д��
    printf("photo data is: %d \r\n",jpg_frame_data_length);
    jpg_frame_data_length=0;
    */
}

//�ļ������������⸲�ǣ�
//jpg��ϳ�:����"0:PHOTO/PIC13141.jpg"���ļ���
//pname = "picture/IMG_%d.jpeg"
//ovtemp ������jpeg�ļ�·��
/*
void camera_new_pathname(void)
{
    sprintf(ovname,"Photo_Times:%4d\r\n",fphotos); // �����ļ������ݼ�¼���
    Test_f_writefile("config.ini",ovname,rmove_data_conf,strlen(ovname)); //�����ļ�д�� ����Read_times
    printf("Write Photos:%4d OK!\r\n",fphotos);
    sprintf(ovtemp,"picture/IMG_%d.jpg",fphotos);
    //Test_f_writefile(ovname,(char *)jpeg_buf,0,jpg_frame_data_length); //�����ļ�д��
    printf("photo data is: %d \r\n",jpg_frame_data_length);
}
*/
//���������ļ���ȡ��ֵ
void json_data_config(char *str,char *R1,char *R2,char *R3,char *R4,char *R5,char *R6)
{
    uint8_t i;
    uint8_t comma_num = 0;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == ':') {
            switch (comma_num) {
            case 0: {
                comma_num = 1;
                R1[0] = str[i + 1];
                R1[1] = str[i + 2];
                R1[2] = str[i + 3];
                R1[3] = str[i + 4];
                break;
            }
            case 1: {
                comma_num =2;
                R2[0] = str[i + 1];
                R2[1] = str[i + 2];
                R2[2] = str[i + 3];
                R2[3] = str[i + 4];
                break;
            }

            case 2: {
                comma_num =3;
                R3[0] = str[i + 1];
                R3[1] = str[i + 2];
                R3[2] = str[i + 3];
                R3[3] = str[i + 4];
                break;
            }

            case 3: {
                comma_num += 1;
                R4[0] = str[i + 1];
                R4[1] = str[i + 2];
                R4[2] = str[i + 3];
                R4[3] = str[i + 4];
                break;
            }

            case 4: {
                comma_num =5;
                R5[0] = str[i + 1];
                R5[1] = str[i + 2];
                R5[2] = str[i + 3];
                R5[3] = str[i + 4];
                break;
            }

            case 5: {
                comma_num = 0;
                R6[0] = str[i + 1];
                R6[1] = str[i + 2];
                R6[2] = str[i + 3];
                R6[3] = str[i + 4];
                break;
            }

            }
        } else
            continue;
    }
}


// ����3���յ�����Ϣ
void usart3_data_analysis(void)
{
    uint16_t cra16_check_Data=0;
    uint16_t cra16_rec_Data=0;
    uint16_t len=0;

    if(USART3_RX_STA&0x8000) {
        len=USART3_RX_STA&0x3FFF;   //�õ��˴ν��յ������ݳ���
        printf("Receive Data OK\r\n");
        cra16_rec_Data = crc16((char *)USART3_RX_BUF,len-2);
        cra16_check_Data = USART3_RX_BUF[len-2]<<8|USART3_RX_BUF[len-1];
        if(cra16_rec_Data == cra16_check_Data) {
            USART3_RX_BUF[len-2] = 0x0d;
            USART3_RX_BUF[len-1] = 0x0a;
            USART3_RX_BUF[len] = '\0';
            if(SD_Initialize() == 0) {
                printf("Write data to Excel OK!!!\r\n");
                Test_f_writefile(ftmp,(char *)USART3_RX_BUF,rmove_data_num,len);
                rmove_data_num += strlen((char *)USART3_RX_BUF);
            } else {
                printf("SD Card Failed! Please Check!\r\n");
            }
        }
        USART3_RX_STA=0;
    } else {
        if(times == 50) {
            if(SD_Initialize() != 0) {
                LED_Red=!LED_Red;
                printf("Check SD Card Failed!!\r\n");
            } else
                LED_Red=0;
            times=0;
        }
        times++;
    }

}



















