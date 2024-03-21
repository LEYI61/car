/*********************************************************************************************************************
* CYT4BB Opensourec Library ���� CYT4BB ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ��� CYT4BB ��Դ���һ����
*
* CYT4BB ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          main_cm7_0
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          IAR 9.40.1
* ����ƽ̨          CYT4BB
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2024-1-4       pudding            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"
// ���µĹ��̻��߹����ƶ���λ�����ִ�����²���
// ��һ�� �ر��������д򿪵��ļ�
// �ڶ��� project->clean  �ȴ��·�����������

// �������ǿ�Դ��չ��� ��������ֲ���߲��Ը���������
// �������ǿ�Դ��չ��� ��������ֲ���߲��Ը���������
// �������ǿ�Դ��չ��� ��������ֲ���߲��Ը���������

// **************************** �������� ****************************
#define LED1                  (P19_0)//(P20_9)

#define X6F_CH1               P17_3      //(P18_6)//(P33_6)      //�����ͨ��
#define X6F_CH2               P17_4      //(P18_7)//(P33_7)      //����ͨ��
#define X6F_CH3               P07_7      //(P18_4)//(P21_4)      //ָʾ��ťͨ��
#define X6F_CH4               P10_2      //(P18_2)//(P21_2)      //������λͨ��
#define X6F_CH5               P07_6      //(P18_5)//(P21_5)      //��ťͨ��1
#define X6F_CH6               P10_3      //(P18_3)//(P21_3)      //��ťͨ��2

#define PIT_NUM                 (PIT_CH0 )                                      // ʹ�õ������жϱ��
#define PIT_PRIORITY            (CPUIntIdx2_IRQn)                               // ��Ӧ�����жϵ��жϱ��

int16 x6f_pin_map[6] = {X6F_CH1, X6F_CH2, X6F_CH3, X6F_CH4, X6F_CH5, X6F_CH6};
//��ͨ���ߵ�ƽ��������
int16 x6f_count[6];
//��ͨ���ߵ�ƽ�������
int16 x6f_out[6]={0};


int           ser_duty;        //���ռ�ձ�
int           mot_duty_1=0;       //���PWM1
int           mot_duty_2=0;       //���PWM2
int  dir=0;                        //�������

uint8 pit_state = 0;

#define SER_MID  765    //�����ֵ
#define SER_MAX  905    //���ֵ(������)
#define SER_MIX  625    //��Сֵ(������)

//IfxCpu_mutexLock mutexTFTIsOk = 0;                   /** TFT18ʹ�ñ�־λ **/
void car_status_control(void);
void x6f_scan(void);

int main(void)
{
    //__enable_irq();
   

    clock_init(SYSTEM_CLOCK_250M); 	// ʱ�����ü�ϵͳ��ʼ��<��ر���>
    debug_info_init();                  // ���Դ�����Ϣ��ʼ��
    
    // �˴���д�û����� ���������ʼ�������
    //��ʼ��LED
    
    gpio_init(LED1, GPO, GPIO_LOW,  GPO_PUSH_PULL);                             // ��ʼ�� LED1 ��� Ĭ�ϵ͵�ƽ �������ģʽ
    pit_ms_init(PIT_NUM, 500);                                               
    
    
    //��ʼ�����ջ�����
    gpio_init(X6F_CH1, GPI, GPIO_LOW, GPI_PULL_UP);
    gpio_init(X6F_CH2, GPI, GPIO_LOW, GPI_PULL_UP);
    gpio_init(X6F_CH3, GPI, GPIO_LOW, GPI_PULL_UP);
    gpio_init(X6F_CH4, GPI, GPIO_LOW, GPI_PULL_UP);
    gpio_init(X6F_CH5, GPI, GPIO_LOW, GPI_PULL_UP);
    gpio_init(X6F_CH6, GPI, GPIO_LOW, GPI_PULL_UP);
    
      tft180_init();
//    //gpio_init(P02_7, GPO, GPIO_LOW, GPO_PUSH_PULL); //�������
    gpio_init(P19_3, GPO, GPIO_LOW, GPO_PUSH_PULL); //�������
//    //pwm_init(ATOM0_CH4_P02_4, 17000, 0);                                                        // ���PWM��ʼ�� Ƶ��1KHz ռ�ձȳ�ʼΪ0
    pwm_init(TCPWM_CH12_P05_3, 17000, 0); 
//    //pwm_init(ATOM0_CH6_P02_6, 17000, 0);
    pwm_init(TCPWM_CH11_P05_2, 17000, 0);
//    //pwm_init(ATOM1_CH1_P33_9, 50, 765);                                                     //�����ʼ��
    pwm_init(TCPWM_CH20_P08_1, 50, 765);
//    
//    //uart_init(UART_4,115200,UART0_TX_P14_0,UART0_RX_P14_1);
//    uart_init(UART_4,115200,UART4_TX_P14_1,UART4_RX_P14_0);
      //��ʼ����ʱ�������ڶ�ȡ���ջ���ͨ�����Ÿߵ�ƽʱ��
      //pit_us_init(CCU60_CH0, 10);

      interrupt_set_priority(PIT_PRIORITY, 1);

    // �˴���д�û����� ���������ʼ�������
    while(true)
    {
      
        // �˴���д��Ҫѭ��ִ�еĴ���
        //��ӡ��ͨ���ߵ�ƽʱ������ֵ
        printf("CH1 = %d\tCH2 = %d\tCH3 = %d\tCH4= %d\tCH5 = %d\tCH6 = %d\r\n",
                x6f_out[0], x6f_out[1], x6f_out[2], x6f_out[3], x6f_out[4], x6f_out[5]);
        //gpio_toggle_level(LED1);

        tft180_show_uint(0,0,x6f_out[0],4);
        tft180_show_uint(35,0,ser_duty,4);
        tft180_show_uint(0,15,x6f_out[1],4);

        tft180_show_uint(35,15,mot_duty_1,4);
        tft180_show_uint(75,15,mot_duty_2,4);
        tft180_show_uint(0,30,x6f_out[2],4);
        tft180_show_uint(0,45,x6f_out[3],4);
        tft180_show_uint(0,60,x6f_out[4],4);
        tft180_show_uint(0,75,x6f_out[5],4);
        
        //�����PWM,
        pwm_set_duty(TCPWM_CH20_P08_1,ser_duty);
        //���PWM

        if(mot_duty_2==0&&mot_duty_1==0)
       {

           pwm_set_duty(TCPWM_CH11_P05_2,0);
       }

       if(mot_duty_2!=0)
       {
           gpio_set_level(P19_3,1);
           pwm_set_duty(TCPWM_CH11_P05_2,mot_duty_2);
       }
       if(mot_duty_1!=0)
       {
           gpio_set_level(P19_3,0);
           pwm_set_duty(TCPWM_CH11_P05_2,mot_duty_1);

       }

        
//        // �˴���д��Ҫѭ��ִ�еĴ���
        if(pit_state)
        {
            gpio_toggle_level(LED1);
            pit_state = 0;                                                      // ��������жϴ�����־λ
            
            x6f_scan();                                     // ɨ����ջ�����ͨ��
            car_status_control();
        }
    }
}

// **************************** �������� ****************************

void x6f_scan(void)                 // ͨ��ɨ��
{
    for(int i = 0; i < 6; i ++)
    {
        if(gpio_get_level(x6f_pin_map[i]))
        {
            x6f_count[i]++;
        }
        else if(x6f_count[i] > 0)
        {
            x6f_out[i] = x6f_count[i];
            x6f_count[i] = 0;
        }
    }
}

//IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
//{
//    interrupt_global_enable(0);                     // �����ж�Ƕ��
//    pit_clear_flag(CCU60_CH0);
//
//    x6f_scan();                                     // ɨ����ջ�����ͨ��
//    car_status_control();
//}

void car_status_control(void)
{

 
           //����������-->λ��ʽPID



           //ser_duty=Position_PID(yaw_cha,0,PID_kp);
           //ser_duty = SER_MID + ser_duty;  //�����ֵ��ȥ��������ռ�ձ�
            if(x6f_out[0]>150)
            {
                ser_duty=(int)(765+(x6f_out[0]-150)*2.8);  //ң�������ƶ��
            }
            else
            {

                ser_duty=(int)(765-(150-x6f_out[0])*2.8);  //ң�������ƶ��

            }
            if(x6f_out[1]>160)
            {

                mot_duty_1=(x6f_out[1]-160)*220;
                mot_duty_2=0;
            }
            else if((148<x6f_out[1])&&(x6f_out[1]<151))
            {

                mot_duty_1=0;
                mot_duty_2=0;
            }
            else if(x6f_out[1]>0&&x6f_out[1]<145)
            {

                mot_duty_1=0;
                mot_duty_2=(160-x6f_out[1])*70;

            }
            //����޷�
            if(ser_duty < SER_MIX)
                   ser_duty = SER_MIX;
            if(ser_duty > SER_MAX)
                   ser_duty = SER_MAX;

            if(mot_duty_1 < 0)
                mot_duty_1 = 0;
            if(mot_duty_1 > 10000)
                mot_duty_1 = 10000;
            if(mot_duty_2 < 0)
                mot_duty_2 = 0;
            if(mot_duty_2 > 10000)
                mot_duty_2 = 10000;

}