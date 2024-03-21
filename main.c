/*********************************************************************************************************************
* CYT4BB Opensourec Library 即（ CYT4BB 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 CYT4BB 开源库的一部分
*
* CYT4BB 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          main_cm7_0
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          IAR 9.40.1
* 适用平台          CYT4BB
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2024-1-4       pudding            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"
// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完

// 本例程是开源库空工程 可用作移植或者测试各类内外设
// 本例程是开源库空工程 可用作移植或者测试各类内外设
// 本例程是开源库空工程 可用作移植或者测试各类内外设

// **************************** 代码区域 ****************************
#define LED1                  (P19_0)//(P20_9)

#define X6F_CH1               P17_3      //(P18_6)//(P33_6)      //方向舵通道
#define X6F_CH2               P17_4      //(P18_7)//(P33_7)      //油门通道
#define X6F_CH3               P07_7      //(P18_4)//(P21_4)      //指示按钮通道
#define X6F_CH4               P10_2      //(P18_2)//(P21_2)      //拨动挡位通道
#define X6F_CH5               P07_6      //(P18_5)//(P21_5)      //旋钮通道1
#define X6F_CH6               P10_3      //(P18_3)//(P21_3)      //旋钮通道2

#define PIT_NUM                 (PIT_CH0 )                                      // 使用的周期中断编号
#define PIT_PRIORITY            (CPUIntIdx2_IRQn)                               // 对应周期中断的中断编号

int16 x6f_pin_map[6] = {X6F_CH1, X6F_CH2, X6F_CH3, X6F_CH4, X6F_CH5, X6F_CH6};
//各通道高电平计数变量
int16 x6f_count[6];
//各通道高电平计数输出
int16 x6f_out[6]={0};


int           ser_duty;        //舵机占空比
int           mot_duty_1=0;       //电机PWM1
int           mot_duty_2=0;       //电机PWM2
int  dir=0;                        //电机方向

uint8 pit_state = 0;

#define SER_MID  765    //舵机中值
#define SER_MAX  905    //最大值(右拉满)
#define SER_MIX  625    //最小值(左拉满)

//IfxCpu_mutexLock mutexTFTIsOk = 0;                   /** TFT18使用标志位 **/
void car_status_control(void);
void x6f_scan(void);

int main(void)
{
    //__enable_irq();
   

    clock_init(SYSTEM_CLOCK_250M); 	// 时钟配置及系统初始化<务必保留>
    debug_info_init();                  // 调试串口信息初始化
    
    // 此处编写用户代码 例如外设初始化代码等
    //初始化LED
    
    gpio_init(LED1, GPO, GPIO_LOW,  GPO_PUSH_PULL);                             // 初始化 LED1 输出 默认低电平 推挽输出模式
    pit_ms_init(PIT_NUM, 500);                                               
    
    
    //初始化接收机引脚
    gpio_init(X6F_CH1, GPI, GPIO_LOW, GPI_PULL_UP);
    gpio_init(X6F_CH2, GPI, GPIO_LOW, GPI_PULL_UP);
    gpio_init(X6F_CH3, GPI, GPIO_LOW, GPI_PULL_UP);
    gpio_init(X6F_CH4, GPI, GPIO_LOW, GPI_PULL_UP);
    gpio_init(X6F_CH5, GPI, GPIO_LOW, GPI_PULL_UP);
    gpio_init(X6F_CH6, GPI, GPIO_LOW, GPI_PULL_UP);
    
      tft180_init();
//    //gpio_init(P02_7, GPO, GPIO_LOW, GPO_PUSH_PULL); //电机方向
    gpio_init(P19_3, GPO, GPIO_LOW, GPO_PUSH_PULL); //电机方向
//    //pwm_init(ATOM0_CH4_P02_4, 17000, 0);                                                        // 电机PWM初始化 频率1KHz 占空比初始为0
    pwm_init(TCPWM_CH12_P05_3, 17000, 0); 
//    //pwm_init(ATOM0_CH6_P02_6, 17000, 0);
    pwm_init(TCPWM_CH11_P05_2, 17000, 0);
//    //pwm_init(ATOM1_CH1_P33_9, 50, 765);                                                     //舵机初始化
    pwm_init(TCPWM_CH20_P08_1, 50, 765);
//    
//    //uart_init(UART_4,115200,UART0_TX_P14_0,UART0_RX_P14_1);
//    uart_init(UART_4,115200,UART4_TX_P14_1,UART4_RX_P14_0);
      //初始化定时器，用于读取接收机各通道引脚高电平时间
      //pit_us_init(CCU60_CH0, 10);

      interrupt_set_priority(PIT_PRIORITY, 1);

    // 此处编写用户代码 例如外设初始化代码等
    while(true)
    {
      
        // 此处编写需要循环执行的代码
        //打印各通道高电平时长计数值
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
        
        //舵机的PWM,
        pwm_set_duty(TCPWM_CH20_P08_1,ser_duty);
        //电机PWM

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

        
//        // 此处编写需要循环执行的代码
        if(pit_state)
        {
            gpio_toggle_level(LED1);
            pit_state = 0;                                                      // 清空周期中断触发标志位
            
            x6f_scan();                                     // 扫描接收机各个通道
            car_status_control();
        }
    }
}

// **************************** 代码区域 ****************************

void x6f_scan(void)                 // 通道扫描
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
//    interrupt_global_enable(0);                     // 开启中断嵌套
//    pit_clear_flag(CCU60_CH0);
//
//    x6f_scan();                                     // 扫描接收机各个通道
//    car_status_control();
//}

void car_status_control(void)
{

 
           //舵机方向控制-->位置式PID



           //ser_duty=Position_PID(yaw_cha,0,PID_kp);
           //ser_duty = SER_MID + ser_duty;  //舵机中值减去舵机舵机的占空比
            if(x6f_out[0]>150)
            {
                ser_duty=(int)(765+(x6f_out[0]-150)*2.8);  //遥控器控制舵机
            }
            else
            {

                ser_duty=(int)(765-(150-x6f_out[0])*2.8);  //遥控器控制舵机

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
            //舵机限幅
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