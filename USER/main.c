#include "string.h"
#include "stdlib.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "USART3.h"
#include "ram.h"
#include "MMC_SD.h"
#include "ff.h"
#include "TEST_FATFS.h"
#include "led.h"
#include "apps.h"
#include "sccb.h"
#include "ov2640.h"

//EXCEL  ���ͷ
char senddata[256]="Beep,Light,Servo,Harmfulgas,Ultraviolet,Temperature,Humidity,	Photoconductor,Pressure,Gyro_x,Gyro_y,Gyro_z,Accel_x,Accel_y,Accel_z,Mag_x,mag_y,Mag_z,Long,Lati\r\n";
char receive_SD_data[128]; //���ڶ�ȡ�����ļ������ݻ�������
unsigned long int rmove_data_num=0; //SD������ƫ�Ƶ�ַ����
unsigned long int rmove_data_conf=0; //SD�������ļ�ƫ�Ƶ�ַ����
unsigned fdirectory=0; //��ȡ��SD������excel�����ļ��ļ�������
unsigned fphotos=0; //��ȡ��SD������photo�����ļ��ļ�������
char fname[40]; //Read_Times �����ļ�·��
char ftmp[40];  //excel�ļ�����
char pname[40]; //Photo_Times �����ļ�·��
char ptmp[40];  //photo�ļ�����
char errcode = 0;
char start_ov2640_flag=0;  //���OV2640�Ƿ��ʼ���ɹ� 1ʧ�� 0�ɹ�
uint16_t secs=0;

int main(void)
{
    SystemInit();
    delay_init(72);	//��ʱ��ʼ��
    NVIC_Configuration();
    uart_init(115200);
    uart3_init(115200);  //���ݽ���Ƶ������Ϊ115200
    LED_Init();

    while(SD_Initialize() != 0) {
        //��ⲻ��SD��
        printf("SD Card Failed!\r\n");
        LED_Red=1;
        delay_ms(500);
        printf("Please Check!\r\n");
        LED_Red=0;
        delay_ms(500);
    }
    printf("Check SD OK!\r\n");

    readfirenum("config.ini",receive_SD_data); // ��ȡ��������ݲ鿴�ǵڼ����ϵ�
    // ��ȡ�����������Ϣ
    json_data_config((char *)receive_SD_data,(char *)Read_times1,(char *)Read_times2,(char *)Read_times3,(char *)Read_times4,(char *)Read_times5,(char *)Read_times6);
    fdirectory = atoi((char *)Read_times1); //��ȡ�����ļ����excel���Ĵ洢����
    fphotos = atoi((char *)Read_times2);    //��ȡ�����ļ����photo��Ƭ�Ĵ洢����

    if(fdirectory == 0) {
        Test_f_mkdir("userdata");// ����ڴ濨�Ѿ�����ʽ�����򴴽��ļ��� ���ݴ洢
        printf("FIRST Create userdata success!\r\n");  //����excel���ɹ�
        fdirectory++;
        sprintf(fname,"Read_Times:%4d\r\n",fdirectory); // �����ļ������ݼ�¼���
        rmove_data_conf = strlen(fname);  // ��ȡ�����ļ���һ�εĳ���
        Test_f_writefile("config.ini",fname,0,strlen(fname)); //�����ļ�д��
        printf("FIRST Write Read_Times:%4d ok!\r\n",fdirectory);

        sprintf(ftmp,"userdata/%d_datas.csv",fdirectory);  //����userdata�µ� 1_datas.csv �ļ�
        rmove_data_num = strlen(senddata);// ��ͷ
        Test_f_writefile(ftmp,senddata,0,strlen(senddata)); //д��excel��ʽ�ļ��ı�ͷ
        printf("FIRST Write userdata/%d_datas.csv\r\n",fdirectory);
    } else {
        if(fdirectory == 0XFFFF) fdirectory=0;  //����洢������޶ȣ����0��ʼ���´洢
        fdirectory++;
        sprintf(fname,"Read_times:%4d\r\n",fdirectory); // �����ļ������ݼ�¼���
        rmove_data_conf = strlen(fname);  // ��ȡ�����ļ���һ�εĳ���
        Test_f_writefile("config.ini",fname,0,strlen(fname)); //�����ļ�д�� ����Read_times
        printf("Create config.ini success!\r\n");
        printf("Create Read_times:%4d OK !\r\n",fdirectory);

        sprintf(ftmp,"userdata/%d_datas.csv",fdirectory);  //����userdata�µ� x_datas.csv �ļ�
        rmove_data_num = strlen(senddata);// ��ͷ
        Test_f_writefile(ftmp,senddata,0,strlen(senddata)); //д��excel��ʽ�ļ��ı�ͷ
        printf("Create %d_datas.csv OK !\r\n",fdirectory);
    }
    //  ��Ƭ
    if(fphotos == 0) {
        Test_f_mkdir("picture");// ����ڴ濨�Ѿ�����ʽ�����򴴽��ļ���  ͼƬ�洢
        printf("FIRST Create picture success!\r\n"); //����1��ӡ��Ϣ
        fphotos++;
        sprintf(pname,"Photo_Times:%4d\r\n",fphotos); // �����ļ������ݼ�¼���
        Test_f_writefile("config.ini",pname,rmove_data_conf,strlen(pname)); //�����ļ�д��
        printf("FIRST Write Photo_Times:%4d OK!\r\n",fphotos);
    } else {
        if(fphotos == 0XFFFF) 	fphotos = 0;  //����洢������޶ȣ����0��ʼ���´洢
        fphotos++;
        sprintf(pname,"Photo_Times:%4d\r\n",fphotos); // �����ļ������ݼ�¼���
        Test_f_writefile("config.ini",pname,rmove_data_conf,strlen(pname)); //�����ļ�д�� ����Read_times
        printf("Create Photo_Times:%4d OK !\r\n",fphotos);
    }

    while(OV2640_Init()) {	//��ʼ��OV2640 1s ���
        if(errcode == 10) {
            errcode=0;
            printf("OV2640 ����!\r\n");
            start_ov2640_flag = 1;
            break;
        }
        errcode++;
        LED_Red=LED_Green=1;
        delay_ms(50);
        LED_Red=LED_Green=0;
        delay_ms(50);
    }
    OV2640_OutSize_Set(OV2640_JPEG_WIDTH,OV2640_JPEG_HEIGHT); //����ͼƬ�ߴ�
    ov2640_speed_ctrl();

    LED_Red=LED_Green=0;
    printf("Start !\r\n");
    while(1) {
        usart3_data_analysis();
        if(secs == 6000) {   //1min����һ����Ƭ
            secs=0;
            if( SD_Initialize() !=0 ) {
                LED_Red=1;
                delay_ms(500);
                LED_Red=0;
                printf("Check SD Card Failed!!\r\n");
                delay_ms(500);
            } else {
                if(start_ov2640_flag == 0)
                    ov2640_jpg_photo();
                printf("OK\r\n");
                fphotos++;
            }
        }
        secs++;
        delay_ms(10);
    }
}

