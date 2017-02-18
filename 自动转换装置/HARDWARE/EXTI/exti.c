#include "exti.h"
#include "delay.h" 
#include "led.h" 
#include "key.h"
#include "beep.h"
#include "sensorpulse.h"
#include "stm32f4xx_exti.h"


//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//外部中断 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/4
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#define GetTickCnt()    \
    uint64_t    time64;   \
    timetmp = TIM2->CNT;    \
    time64  = g_time3cnt;   \


extern uint64_t g_time3cnt;
#define TIMEOUT      10

void EXTI9_5_IRQHandler(void)
{
    u32     timetmp;
    u8      bittmp1,bittmp2,bittmp3;
    GetTickCnt();
    //delay_us(150);

    //通道1
    if(EXTI_GetITStatus(EXTI_Line5) == SET )
    {
        if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) == SET)  //上升沿
        {
            gs_pulsech[0].lstrisetime =   gs_pulsech[0].risetime;
            gs_pulsech[0].risetime    =   ((uint64_t)time64 << 32) + timetmp;
            gs_pulsech[0].lstperiod   = gs_pulsech[0].period;

            gs_pulsech[0].period      =   gs_pulsech[0].risetime  - gs_pulsech[0].lstrisetime;            
            gs_pulsech[0].flg         |=  (0x01 << PLUSE_NEW);      
        }
        else
        {
            gs_pulsech[0].lstfalltime =   gs_pulsech[0].falltime;
            gs_pulsech[0].falltime    =   ((uint64_t)time64 << 32) + timetmp;
            

            //增加相位关系 ，0-1
            if( gs_pulsech[0].risetime   < gs_pulsech[1].risetime){
                gs_pulsech[0].flg   |=  (0x01 << SIDE_FLG);  
            }else{
                gs_pulsech[0].flg   &=  ~(0x01 << SIDE_FLG);  
            }            
        }
            
        EXTI_ClearITPendingBit(EXTI_Line5);//清除LINE5上的中断标志位 
    }    
    
    //通道2
    if(EXTI_GetITStatus(EXTI_Line6) == SET )
    {
        if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) == SET)  //上升沿
        {
            gs_pulsech[1].lstrisetime =   gs_pulsech[1].risetime;
            gs_pulsech[1].risetime    =   ((uint64_t)time64 << 32) + timetmp;
            gs_pulsech[1].lstperiod   = gs_pulsech[1].period;
            gs_pulsech[1].period      =   gs_pulsech[1].risetime  - gs_pulsech[1].lstrisetime;            
            gs_pulsech[1].flg         |=  (0x01 << PLUSE_NEW);        
        }
        else
        {
            gs_pulsech[1].lstfalltime =   gs_pulsech[1].falltime;
            gs_pulsech[1].falltime    =   ((uint64_t)time64 << 32) + timetmp;
        }
        
        EXTI_ClearITPendingBit(EXTI_Line6);//清除LINE8上的中断标志位 
    }          
    
    //通道3
    if(EXTI_GetITStatus(EXTI_Line7) == SET )
    {
        if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7) == SET)  //上升沿
        {
            gs_pulsech[2].lstrisetime   =   gs_pulsech[2].risetime;
            gs_pulsech[2].risetime      =   ((uint64_t)time64 << 32) + timetmp;
            gs_pulsech[2].lstperiod     =   gs_pulsech[2].period;
            gs_pulsech[2].period        =   gs_pulsech[2].risetime  - gs_pulsech[2].lstrisetime;
            gs_pulsech[2].flg          |=   (0x01 << PLUSE_NEW);        
        }
        else
        {
            gs_pulsech[2].lstfalltime   = gs_pulsech[2].falltime;
            gs_pulsech[2].falltime      = ((uint64_t)time64 << 32) + timetmp;
        }
        EXTI_ClearITPendingBit(EXTI_Line7);//清除LINE5上的中断标志位  
    }
    
 
    //通道4
    if(EXTI_GetITStatus(EXTI_Line8) == SET )
    {
        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) == SET)  //上升沿
        {
            gs_pulsech[3].lstrisetime   =   gs_pulsech[3].risetime;
            gs_pulsech[3].risetime      =   ((uint64_t)time64 << 32) + timetmp;
            gs_pulsech[3].lstperiod     = gs_pulsech[3].period;
            gs_pulsech[3].period        =   gs_pulsech[3].risetime  - gs_pulsech[3].lstrisetime;            
            gs_pulsech[3].flg           |=  (0x01 << PLUSE_NEW);        
        }
        else
        {
            gs_pulsech[3].lstfalltime =   gs_pulsech[3].falltime;
            gs_pulsech[3].falltime    =   ((uint64_t)time64 << 32) + timetmp;
        }
        
        EXTI_ClearITPendingBit(EXTI_Line8);//清除LINE7上的中断标志位 
    }           
}


void EXTI15_10_IRQHandler(void)
{
    u32         timetmp; 
    GetTickCnt();   
    
    //通道5
    if(EXTI_GetITStatus(EXTI_Line12) == SET )
    {
        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == SET)  //上升沿
        {            
            gs_pulsech[4].lstrisetime =   gs_pulsech[4].risetime;
            gs_pulsech[4].risetime    =   ((uint64_t)time64 << 32) + timetmp;
            gs_pulsech[4].lstperiod   =   gs_pulsech[4].period;
            gs_pulsech[4].period      =   gs_pulsech[4].risetime  - gs_pulsech[4].lstrisetime;            
            gs_pulsech[4].flg        |=  (0x01 << PLUSE_NEW);              
        }
        else
        {           
            gs_pulsech[4].lstfalltime =   gs_pulsech[4].falltime;
            gs_pulsech[4].falltime    =   ((uint64_t)time64 << 32) + timetmp;
            

            //增加相位关系 ，4-5
            if( gs_pulsech[4].risetime   < gs_pulsech[5].risetime){
                gs_pulsech[4].flg   |=  (0x01 << SIDE_FLG);  
            }else{
                gs_pulsech[4].flg   &=  ~(0x01 << SIDE_FLG);  
            }            
        }
        
        EXTI_ClearITPendingBit(EXTI_Line12);//清除LINE6上的中断标志位 
    }    
   
 //通道6
    if(EXTI_GetITStatus(EXTI_Line13) == SET )
    {
        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13) == SET)  //上升沿
        {            
            gs_pulsech[5].lstrisetime =   gs_pulsech[5].risetime;
            gs_pulsech[5].risetime    =   ((uint64_t)time64 << 32) + timetmp;
            gs_pulsech[5].lstperiod   =   gs_pulsech[5].period;
            gs_pulsech[5].period      =   gs_pulsech[5].risetime  - gs_pulsech[5].lstrisetime;            
            gs_pulsech[5].flg        |=   (0x01 << PLUSE_NEW);              
        }
        else
        {           
            gs_pulsech[5].lstfalltime =   gs_pulsech[5].falltime;
            gs_pulsech[5].falltime    =   ((uint64_t)time64 << 32) + timetmp;
        }
        
        EXTI_ClearITPendingBit(EXTI_Line13);//清除LINE上的中断标志位 
    }    
   
 //通道7
    if(EXTI_GetITStatus(EXTI_Line14) == SET )
    {
        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14) == SET)  //上升沿
        {            
            gs_pulsech[6].lstrisetime =   gs_pulsech[6].risetime;
            gs_pulsech[6].risetime    =   ((uint64_t)time64 << 32) + timetmp;
            gs_pulsech[6].lstperiod   =   gs_pulsech[6].period;
            gs_pulsech[6].period      =   gs_pulsech[6].risetime  - gs_pulsech[6].lstrisetime;            
            gs_pulsech[6].flg        |=  (0x01 << PLUSE_NEW);              
        }
        else
        {           
            gs_pulsech[6].lstfalltime =   gs_pulsech[6].falltime;
            gs_pulsech[6].falltime    =   ((uint64_t)time64 << 32) + timetmp;
            
        }
        
        EXTI_ClearITPendingBit(EXTI_Line14);//清除LINE上的中断标志位 
    }    
   

  
    //通道8
    if(EXTI_GetITStatus(EXTI_Line15) == SET )
    {
        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == SET)              //上升沿
        {
            gs_pulsech[7].lstrisetime =   gs_pulsech[7].risetime;
            gs_pulsech[7].risetime    =   ((uint64_t)time64 << 32) + timetmp;
            gs_pulsech[7].lstperiod     = gs_pulsech[7].period;
            gs_pulsech[7].period      =   gs_pulsech[7].risetime  - gs_pulsech[7].lstrisetime;            
            gs_pulsech[7].flg         |=  (0x01 << PLUSE_NEW);        
        }
        else
        {
            gs_pulsech[7].lstfalltime =   gs_pulsech[7].falltime;
            gs_pulsech[7].falltime    =   ((uint64_t)time64 << 32) + timetmp;
        }

        EXTI_ClearITPendingBit(EXTI_Line15);//清除LINE上的中断标志位          
    }       
}

////外部中断2服务程序
//void EXTI2_IRQHandler(void)
//{
//    u32     timetmp;
//    u8      bittmp1,bittmp2,bittmp3;
//    GetTickCnt();
//    //通道6
////    delay_us(150);
//    if(EXTI_GetITStatus(EXTI_Line2) == SET ) {
//        if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))  //上升沿
//        {
//            gs_pulsech[5].lstrisetime   = gs_pulsech[5].risetime;
//            gs_pulsech[5].risetime      = ((uint64_t)time64 << 32) + timetmp;
//            gs_pulsech[5].lstperiod     = gs_pulsech[5].period;
//            gs_pulsech[5].period        = gs_pulsech[5].risetime  - gs_pulsech[5].lstrisetime;
//            gs_pulsech[5].flg          |=  (0x01 << PLUSE_NEW);        
//        }
//        else
//        {
//            gs_pulsech[5].lstfalltime   = gs_pulsech[5].falltime;
//            gs_pulsech[5].falltime      = ((uint64_t)time64 << 32) + timetmp;
//        }
//        
//        EXTI_ClearITPendingBit(EXTI_Line2);//清除LINE2上的中断标志位 
//    }
//}

//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)
//定时器5中断服务程序	 
void TIM1_UP_TIM10_IRQHandler(void)
{ 		    
    u32         timetmp;
    GetTickCnt();  
    
    if(TIM_GetITStatus(TIM10, TIM_IT_CC1) != RESET)//捕获1发生捕获事件
    {	
        TIM_SetCounter(TIM10,0);
        TIM_GetCapture1(TIM10);//获取当前的捕获值.
        
        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) == SET)  //上升沿
        {
            gs_pulsech[6].lstrisetime =   gs_pulsech[6].risetime;
            gs_pulsech[6].risetime    =   ((uint64_t)time64 << 32) + timetmp;
            gs_pulsech[6].lstperiod   = gs_pulsech[6].period;
            gs_pulsech[6].period      =   gs_pulsech[6].risetime  - gs_pulsech[6].lstrisetime;
            gs_pulsech[6].flg         |=  (0x01 << PLUSE_NEW);        
        }
        else
        {
            gs_pulsech[6].lstfalltime =   gs_pulsech[6].falltime;
            gs_pulsech[6].falltime    =   ((uint64_t)time64 << 32) + timetmp;
        }
                
        TIM_ClearITPendingBit(TIM10, TIM_IT_CC1); //清除中断标志位
        return;
 	}    
    
    if(TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET)//溢出
    {	     
        TIM_SetCounter(TIM10,0);
        TIM_ClearITPendingBit(TIM10, TIM_IT_Update); //清除中断标志位
    }
}



////端口初始化函数
//void Exti_Pin_Init(void)
//{
//	
//  GPIO_InitTypeDef  GPIO_InitStructure;

//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA
//                        |RCC_AHB1Periph_GPIOB
//                        |RCC_AHB1Periph_GPIOC
//                        |RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOA,GPIOE时钟
// 
//  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;//普通输入模式
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//100M
//  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL ;//
//    
//  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA0
//     
//  GPIO_InitStructure.GPIO_Pin =     GPIO_Pin_5
//                                |   GPIO_Pin_7
//                                |   GPIO_Pin_8
//                                |   GPIO_Pin_9
//                                |   GPIO_Pin_10;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB
//    
//  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC   
//    
//  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2; //KEY0 KEY1 KEY2对应引脚
//  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE2,3,4
//} 



//端口初始化函数
void Exti_Pin_Init(void)
{
	
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA
                        |RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOA,GPIOE时钟
 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//100M
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL ;//
    
  GPIO_InitStructure.GPIO_Pin =     GPIO_Pin_5
                                |   GPIO_Pin_6
                                |   GPIO_Pin_7
                                ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA0
     
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8
                                |   GPIO_Pin_12
                                |   GPIO_Pin_13
                                |   GPIO_Pin_14
                                |   GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB
} 


extern  void TIM10_CH1_Cap_Init(u32 arr,u16 psc);
//外部中断初始化程序
//初始化PE2~4,PA0为中断输入.
//void EXTIX_Init(void)
//{
//	NVIC_InitTypeDef   NVIC_InitStructure;
//	EXTI_InitTypeDef   EXTI_InitStructure;
//	
//	Exti_Pin_Init();                    //按键对应的IO口初始化
//    TIM10_CH1_Cap_Init(0XFFFF,84-1);    //外部捕获初始化
//    
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
//	
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8 );   //PA8 连接到中断线8   ch1
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource10);   //PB10连接到中断线10  ch2
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5 );   //PB5 连接到中断线5   ch3
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource7 );   //PB7 连接到中断线7   ch4
//    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource6 );   //PC6 连接到中断线6   ch5
//    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2 );   //PE2 连接到中断线2   ch6
//    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource9 );   //PB9 连接到中断线9   ch8
//	
//    /* 配置EXTI_Line2,3,4 */
//    EXTI_InitStructure.EXTI_Line =      EXTI_Line8 | EXTI_Line10 | EXTI_Line5| EXTI_Line7
//                                    |   EXTI_Line6 | EXTI_Line2 | EXTI_Line9;
//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //沿触发
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
//    EXTI_Init(&EXTI_InitStructure);//配置
// 
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//外部中断2
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级3
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
//    NVIC_Init(&NVIC_InitStructure);//配置
//          
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//外部中断5-9
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级1
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
//    NVIC_Init(&NVIC_InitStructure);//配置
//    
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//外部中断15-10
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级1
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
//    NVIC_Init(&NVIC_InitStructure);//配置    
//	   
//}

void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	Exti_Pin_Init();                    //按键对应的IO口初始化
//    TIM10_CH1_Cap_Init(0XFFFF,84-1);    //外部捕获初始化
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource5 );   //     ch1
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource6 );   //     ch2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource7 );   //     ch3
    
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource8  );   //    ch4    
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource12 );   //    ch5
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource13 );   //    ch6
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource14 );   //    ch7
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15 );   //    ch7

	
    /* 配置EXTI_Line */
    EXTI_InitStructure.EXTI_Line    =      EXTI_Line5 | EXTI_Line6 | EXTI_Line7| EXTI_Line8
                                    |   EXTI_Line12 | EXTI_Line13 | EXTI_Line14| EXTI_Line15;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;//中断事件
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
    EXTI_Init(&EXTI_InitStructure);//配置
 
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//外部中断2
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级3
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
//    NVIC_Init(&NVIC_InitStructure);//配置
          
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//外部中断5-9
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);//配置
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//外部中断15-10
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);//配置    
	   
}

//TIM_ICInitTypeDef  TIM10_ICInitStructure;

////定时器10通道1输入捕获配置
////arr：自动重装值(TIM2,TIM10是16位的!!)
////psc：时钟预分频数
//void TIM10_CH1_Cap_Init(u32 arr,u16 psc)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);  	//TIM10时钟使能    
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//使能PORTA时钟	
//	
//	GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_8;           //GPIOB8
//	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;         //复用功能
//	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_25MHz;	//速度100MHz
//	//GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;        //推挽复用输出
//	GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL;       //下拉
//	GPIO_Init(GPIOB,&GPIO_InitStructure);                   //初始化PA0

//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_TIM10);  //PB8复用位定时器10
//  
//	TIM_TimeBaseStructure.TIM_Prescaler=psc;                //定时器分频
//	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//向上计数模式
//	TIM_TimeBaseStructure.TIM_Period=arr;                   //自动重装载值
//	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
//	
//	TIM_TimeBaseInit(TIM10,&TIM_TimeBaseStructure);
//	
//	//初始化TIM10输入捕获参数
//    TIM10_ICInitStructure.TIM_Channel       = TIM_Channel_1;            //CC1S=01 	选择输入端 IC1映射到TI1上
//    TIM10_ICInitStructure.TIM_ICPolarity 	= TIM_ICPolarity_BothEdge;	//上升沿捕获
//    TIM10_ICInitStructure.TIM_ICSelection   = TIM_ICSelection_DirectTI; //映射到TI1上
//    TIM10_ICInitStructure.TIM_ICPrescaler   = TIM_ICPSC_DIV1;	        //配置输入分频,不分频 
//    TIM10_ICInitStructure.TIM_ICFilter      = 0x00;                     //IC1F=0000 配置输入滤波器 不滤波
//    
//    TIM_ICInit(TIM10, &TIM10_ICInitStructure);
//            
//	TIM_ITConfig(TIM10,TIM_IT_CC1,ENABLE);    //允许更新中断 ,允许CC1IE捕获中断	
//	
//    TIM_Cmd(TIM10,ENABLE );     //使能定时器10

//    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;    //TIM10
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=01;     //抢占优先级3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority =00;		    //子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			    //IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
//}

