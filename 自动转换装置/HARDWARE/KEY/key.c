#include "key.h"
#include "delay.h" 
#include "sensorpulse.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//����������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

//������ʼ������
void KEY_Init(void)
{
	
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA
                        |RCC_AHB1Periph_GPIOB
                        |RCC_AHB1Periph_GPIOC
                        |RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOA,GPIOEʱ��
 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN ;//����
    
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA0
     
  GPIO_InitStructure.GPIO_Pin =     GPIO_Pin_5
                                |   GPIO_Pin_7
                                |   GPIO_Pin_8
                                |   GPIO_Pin_9
                                |   GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB
    
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC   
    
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2; //KEY0 KEY1 KEY2��Ӧ����
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4
} 

//������ʼ������
void Set_Key_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC , ENABLE);//ʹ��GPIO
 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;//����
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOA0
} 


//����485��ʼ��
void Set_En485_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);//ʹ��GPIO
 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN ;//����
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA0
} 

void Set_En485_EN(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);//ʹ��GPIO
 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN ;//����
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA0
    
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET);        
} 


void Set_En485_DISEN(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);//ʹ��GPIO
 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN ;//����
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA0
    
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET);        
} 
//������ʼ������
void Hand_Key_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;


  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD , ENABLE);//ʹ��GPIO
 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;//����
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��
    
    
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);//ʹ��GPIO
 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;//����
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
} 


//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY2���� 
//4��WKUP���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY0==0)return 1;
		else if(KEY1==0)return 2;
		else if(KEY2==0)return 3;
		else if(WK_UP==1)return 4;
	}else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0)key_up=1; 	    
 	return 0;// �ް�������
}


void    SetKey(void)
{
    u8  handSts;
    static  u8 lsthandsts = SET;
    handSts = GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12); 
    
    if ( ( RESET == handSts ) && ( SET == lsthandsts ) ) {
        delay_ms(20);
        uint64_t    systick = GetSysBaseTick();

        handSts = GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12); 
        
        u32 recnum = gs_runpara.writerecnum;
        
        printf("\r\n ����¼�ţ�%d ",recnum);
        while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) == RESET)
        {
            if(recnum)
                PrintfRecord(--recnum);
           
            if(GetSysBaseTick() - systick > 1000*1000*42){ //5s
            
                //printf("\r\n setkey");
                Initch();
                
                gs_runpara.flg |= (0x01<<PLUSE_STORE);      //����
                BSP_LED_Off(0);
            }            
        }
    }
    
    lsthandsts = handSts;
    
}

void    SetKeyChModle(void)     //����ͨ��ģʽ��ͬ�����࣬��������
{
    u8  handSts;
    static  u8 lsthandsts = SET;
    u8  keycnt = 0;
    handSts = GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12); 
    
    if ( ( RESET == handSts ) ) //�����Ƿ���
    {
        delay_ms(20);
        uint64_t    systick = GetSysBaseTick();
        
        printf("\r\n an");
        handSts = GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12); 
        
        systick = GetSysBaseTick();
        while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) == RESET
                )//��������
        {    
            if(GetSysBaseTick() - systick > 1000*1000*42/2)//2���
            {
                BSP_LED_On(0);    
                keycnt=1;
            }
            
            if(GetSysBaseTick() - systick > 1000*1000*84)//2���
               return; 
        }
    
       printf("\r\n song");
        systick = GetSysBaseTick();
        while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) == SET && keycnt == 1) //�����ɿ�
        {    
            if(GetSysBaseTick() - systick > 1000*1000*42/2)//2���
                BSP_LED_Off(0);    
        
            if(GetSysBaseTick() - systick > 1000*1000*42)
                return;
        }
        
        printf("\r\n an");
        systick = GetSysBaseTick();
        while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) == RESET) //��������
        {    
            if(GetSysBaseTick() - systick > 1000*1000*42/2)//2���
            {                             
                //BSP_LED_Off(0);    
/////////////////////////////////////////////////////////////////////////////////                    
                if((gs_runpara.chmodel &(0x01<<0)) == 0)
                {
                    gs_runpara.chmodel |= (0x01<<0);        //���
                    for(u8 i=0;i<4;i++)                     //��������λ
                        BSP_LED_On(i+5);
                    printf("\r\n ���");
                    delay_ms(2000);
                }
                else
                {
                    for(u8 i=0;i<4;i++)                     //��������λ
                        BSP_LED_On(i+1);                    
                    gs_runpara.chmodel &= ~(0x01<<0);
                    printf("\r\n ͬ��");
                    delay_ms(2000);
                }

                gs_runpara.crc16 = GetCrc16Check((u8 *)&gs_runpara,sizeof(gs_runpara)-2);

                WriteFM24CL64(FRAM_RUN_PARA_ADDR,(u8 *)&gs_runpara,sizeof(gs_runpara));

                printf("\r\n����ͨ��ģʽ %d",gs_runpara.chmodel);

                
                //if(GetSysBaseTick() - systick > 1000*1000*42)//2���
                return;                                      //
            }              
        }            
    
    }
    
    lsthandsts = handSts;
    
}



















