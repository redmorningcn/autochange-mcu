#include "exti.h"
#include "delay.h" 
#include "led.h" 
#include "key.h"
#include "beep.h"
#include "sensorpulse.h"
#include "stm32f4xx_exti.h"


//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//�ⲿ�ж� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/4
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
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

    //ͨ��1
    if(EXTI_GetITStatus(EXTI_Line5) == SET )
    {
        if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) == SET)  //������
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
            

            //������λ��ϵ ��0-1
            if( gs_pulsech[0].risetime   < gs_pulsech[1].risetime){
                gs_pulsech[0].flg   |=  (0x01 << SIDE_FLG);  
            }else{
                gs_pulsech[0].flg   &=  ~(0x01 << SIDE_FLG);  
            }            
        }
            
        EXTI_ClearITPendingBit(EXTI_Line5);//���LINE5�ϵ��жϱ�־λ 
    }    
    
    //ͨ��2
    if(EXTI_GetITStatus(EXTI_Line6) == SET )
    {
        if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) == SET)  //������
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
        
        EXTI_ClearITPendingBit(EXTI_Line6);//���LINE8�ϵ��жϱ�־λ 
    }          
    
    //ͨ��3
    if(EXTI_GetITStatus(EXTI_Line7) == SET )
    {
        if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7) == SET)  //������
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
        EXTI_ClearITPendingBit(EXTI_Line7);//���LINE5�ϵ��жϱ�־λ  
    }
    
 
    //ͨ��4
    if(EXTI_GetITStatus(EXTI_Line8) == SET )
    {
        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) == SET)  //������
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
        
        EXTI_ClearITPendingBit(EXTI_Line8);//���LINE7�ϵ��жϱ�־λ 
    }           
}


void EXTI15_10_IRQHandler(void)
{
    u32         timetmp; 
    GetTickCnt();   
    
    //ͨ��5
    if(EXTI_GetITStatus(EXTI_Line12) == SET )
    {
        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == SET)  //������
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
            

            //������λ��ϵ ��4-5
            if( gs_pulsech[4].risetime   < gs_pulsech[5].risetime){
                gs_pulsech[4].flg   |=  (0x01 << SIDE_FLG);  
            }else{
                gs_pulsech[4].flg   &=  ~(0x01 << SIDE_FLG);  
            }            
        }
        
        EXTI_ClearITPendingBit(EXTI_Line12);//���LINE6�ϵ��жϱ�־λ 
    }    
   
 //ͨ��6
    if(EXTI_GetITStatus(EXTI_Line13) == SET )
    {
        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13) == SET)  //������
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
        
        EXTI_ClearITPendingBit(EXTI_Line13);//���LINE�ϵ��жϱ�־λ 
    }    
   
 //ͨ��7
    if(EXTI_GetITStatus(EXTI_Line14) == SET )
    {
        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14) == SET)  //������
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
        
        EXTI_ClearITPendingBit(EXTI_Line14);//���LINE�ϵ��жϱ�־λ 
    }    
   

  
    //ͨ��8
    if(EXTI_GetITStatus(EXTI_Line15) == SET )
    {
        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == SET)              //������
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

        EXTI_ClearITPendingBit(EXTI_Line15);//���LINE�ϵ��жϱ�־λ          
    }       
}

////�ⲿ�ж�2�������
//void EXTI2_IRQHandler(void)
//{
//    u32     timetmp;
//    u8      bittmp1,bittmp2,bittmp3;
//    GetTickCnt();
//    //ͨ��6
////    delay_us(150);
//    if(EXTI_GetITStatus(EXTI_Line2) == SET ) {
//        if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))  //������
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
//        EXTI_ClearITPendingBit(EXTI_Line2);//���LINE2�ϵ��жϱ�־λ 
//    }
//}

//����״̬
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û���񵽵͵�ƽ;1,�Ѿ����񵽵͵�ƽ��.
//[5:0]:����͵�ƽ������Ĵ���(����32λ��ʱ����˵,1us��������1,���ʱ��:4294��)
//��ʱ��5�жϷ������	 
void TIM1_UP_TIM10_IRQHandler(void)
{ 		    
    u32         timetmp;
    GetTickCnt();  
    
    if(TIM_GetITStatus(TIM10, TIM_IT_CC1) != RESET)//����1���������¼�
    {	
        TIM_SetCounter(TIM10,0);
        TIM_GetCapture1(TIM10);//��ȡ��ǰ�Ĳ���ֵ.
        
        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) == SET)  //������
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
                
        TIM_ClearITPendingBit(TIM10, TIM_IT_CC1); //����жϱ�־λ
        return;
 	}    
    
    if(TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET)//���
    {	     
        TIM_SetCounter(TIM10,0);
        TIM_ClearITPendingBit(TIM10, TIM_IT_Update); //����жϱ�־λ
    }
}



////�˿ڳ�ʼ������
//void Exti_Pin_Init(void)
//{
//	
//  GPIO_InitTypeDef  GPIO_InitStructure;

//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA
//                        |RCC_AHB1Periph_GPIOB
//                        |RCC_AHB1Periph_GPIOC
//                        |RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOA,GPIOEʱ��
// 
//  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;//��ͨ����ģʽ
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//100M
//  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL ;//
//    
//  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA0
//     
//  GPIO_InitStructure.GPIO_Pin =     GPIO_Pin_5
//                                |   GPIO_Pin_7
//                                |   GPIO_Pin_8
//                                |   GPIO_Pin_9
//                                |   GPIO_Pin_10;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB
//    
//  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC   
//    
//  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2; //KEY0 KEY1 KEY2��Ӧ����
//  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4
//} 



//�˿ڳ�ʼ������
void Exti_Pin_Init(void)
{
	
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA
                        |RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOA,GPIOEʱ��
 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//100M
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL ;//
    
  GPIO_InitStructure.GPIO_Pin =     GPIO_Pin_5
                                |   GPIO_Pin_6
                                |   GPIO_Pin_7
                                ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA0
     
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8
                                |   GPIO_Pin_12
                                |   GPIO_Pin_13
                                |   GPIO_Pin_14
                                |   GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB
} 


extern  void TIM10_CH1_Cap_Init(u32 arr,u16 psc);
//�ⲿ�жϳ�ʼ������
//��ʼ��PE2~4,PA0Ϊ�ж�����.
//void EXTIX_Init(void)
//{
//	NVIC_InitTypeDef   NVIC_InitStructure;
//	EXTI_InitTypeDef   EXTI_InitStructure;
//	
//	Exti_Pin_Init();                    //������Ӧ��IO�ڳ�ʼ��
//    TIM10_CH1_Cap_Init(0XFFFF,84-1);    //�ⲿ�����ʼ��
//    
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
//	
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8 );   //PA8 ���ӵ��ж���8   ch1
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource10);   //PB10���ӵ��ж���10  ch2
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5 );   //PB5 ���ӵ��ж���5   ch3
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource7 );   //PB7 ���ӵ��ж���7   ch4
//    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource6 );   //PC6 ���ӵ��ж���6   ch5
//    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2 );   //PE2 ���ӵ��ж���2   ch6
//    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource9 );   //PB9 ���ӵ��ж���9   ch8
//	
//    /* ����EXTI_Line2,3,4 */
//    EXTI_InitStructure.EXTI_Line =      EXTI_Line8 | EXTI_Line10 | EXTI_Line5| EXTI_Line7
//                                    |   EXTI_Line6 | EXTI_Line2 | EXTI_Line9;
//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //�ش���
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
//    EXTI_Init(&EXTI_InitStructure);//����
// 
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//�ⲿ�ж�2
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�3
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
//    NVIC_Init(&NVIC_InitStructure);//����
//          
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//�ⲿ�ж�5-9
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�1
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
//    NVIC_Init(&NVIC_InitStructure);//����
//    
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//�ⲿ�ж�15-10
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�1
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
//    NVIC_Init(&NVIC_InitStructure);//����    
//	   
//}

void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	Exti_Pin_Init();                    //������Ӧ��IO�ڳ�ʼ��
//    TIM10_CH1_Cap_Init(0XFFFF,84-1);    //�ⲿ�����ʼ��
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource5 );   //     ch1
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource6 );   //     ch2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource7 );   //     ch3
    
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource8  );   //    ch4    
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource12 );   //    ch5
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource13 );   //    ch6
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource14 );   //    ch7
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15 );   //    ch7

	
    /* ����EXTI_Line */
    EXTI_InitStructure.EXTI_Line    =      EXTI_Line5 | EXTI_Line6 | EXTI_Line7| EXTI_Line8
                                    |   EXTI_Line12 | EXTI_Line13 | EXTI_Line14| EXTI_Line15;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;//�ж��¼�
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //�ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
    EXTI_Init(&EXTI_InitStructure);//����
 
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//�ⲿ�ж�2
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�3
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
//    NVIC_Init(&NVIC_InitStructure);//����
          
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//�ⲿ�ж�5-9
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);//����
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//�ⲿ�ж�15-10
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);//����    
	   
}

//TIM_ICInitTypeDef  TIM10_ICInitStructure;

////��ʱ��10ͨ��1���벶������
////arr���Զ���װֵ(TIM2,TIM10��16λ��!!)
////psc��ʱ��Ԥ��Ƶ��
//void TIM10_CH1_Cap_Init(u32 arr,u16 psc)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);  	//TIM10ʱ��ʹ��    
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//ʹ��PORTAʱ��	
//	
//	GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_8;           //GPIOB8
//	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;         //���ù���
//	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_25MHz;	//�ٶ�100MHz
//	//GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;        //���츴�����
//	GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL;       //����
//	GPIO_Init(GPIOB,&GPIO_InitStructure);                   //��ʼ��PA0

//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_TIM10);  //PB8����λ��ʱ��10
//  
//	TIM_TimeBaseStructure.TIM_Prescaler=psc;                //��ʱ����Ƶ
//	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//���ϼ���ģʽ
//	TIM_TimeBaseStructure.TIM_Period=arr;                   //�Զ���װ��ֵ
//	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
//	
//	TIM_TimeBaseInit(TIM10,&TIM_TimeBaseStructure);
//	
//	//��ʼ��TIM10���벶�����
//    TIM10_ICInitStructure.TIM_Channel       = TIM_Channel_1;            //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
//    TIM10_ICInitStructure.TIM_ICPolarity 	= TIM_ICPolarity_BothEdge;	//�����ز���
//    TIM10_ICInitStructure.TIM_ICSelection   = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
//    TIM10_ICInitStructure.TIM_ICPrescaler   = TIM_ICPSC_DIV1;	        //���������Ƶ,����Ƶ 
//    TIM10_ICInitStructure.TIM_ICFilter      = 0x00;                     //IC1F=0000 ���������˲��� ���˲�
//    
//    TIM_ICInit(TIM10, &TIM10_ICInitStructure);
//            
//	TIM_ITConfig(TIM10,TIM_IT_CC1,ENABLE);    //��������ж� ,����CC1IE�����ж�	
//	
//    TIM_Cmd(TIM10,ENABLE );     //ʹ�ܶ�ʱ��10

//    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;    //TIM10
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=01;     //��ռ���ȼ�3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority =00;		    //�����ȼ�3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			    //IRQͨ��ʹ��
//	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
//}

