#include "key.h"
#include "delay.h" 
#include "sensorpulse.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//按键输入驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

//按键初始化函数
void KEY_Init(void)
{
	
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA
                        |RCC_AHB1Periph_GPIOB
                        |RCC_AHB1Periph_GPIOC
                        |RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOA,GPIOE时钟
 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN ;//下拉
    
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA0
     
  GPIO_InitStructure.GPIO_Pin =     GPIO_Pin_5
                                |   GPIO_Pin_7
                                |   GPIO_Pin_8
                                |   GPIO_Pin_9
                                |   GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB
    
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC   
    
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2; //KEY0 KEY1 KEY2对应引脚
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE2,3,4
} 

//按键初始化函数
void Set_Key_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC , ENABLE);//使能GPIO
 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;//下拉
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOA0
} 


//设置485初始化
void Set_En485_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);//使能GPIO
 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN ;//下拉
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA0
} 

void Set_En485_EN(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);//使能GPIO
 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN ;//下拉
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA0
    
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET);        
} 


void Set_En485_DISEN(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);//使能GPIO
 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN ;//下拉
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA0
    
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET);        
} 
//按键初始化函数
void Hand_Key_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;


  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD , ENABLE);//使能GPIO
 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;//下拉
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
    
    
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);//使能GPIO
 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;//上拉
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
} 


//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY0==0)return 1;
		else if(KEY1==0)return 2;
		else if(KEY2==0)return 3;
		else if(WK_UP==1)return 4;
	}else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0)key_up=1; 	    
 	return 0;// 无按键按下
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
        
        printf("\r\n 读记录号：%d ",recnum);
        while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) == RESET)
        {
            if(recnum)
                PrintfRecord(--recnum);
           
            if(GetSysBaseTick() - systick > 1000*1000*42){ //5s
            
                //printf("\r\n setkey");
                Initch();
                
                gs_runpara.flg |= (0x01<<PLUSE_STORE);      //保存
                BSP_LED_Off(0);
            }            
        }
    }
    
    lsthandsts = handSts;
    
}

void    SetKeyChModle(void)     //设置通道模式，同侧或异侧，长按更改
{
    u8  handSts;
    static  u8 lsthandsts = SET;
    u8  keycnt = 0;
    handSts = GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12); 
    
    if ( ( RESET == handSts ) ) //按键是否按下
    {
        delay_ms(20);
        uint64_t    systick = GetSysBaseTick();
        
        printf("\r\n an");
        handSts = GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12); 
        
        systick = GetSysBaseTick();
        while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) == RESET
                )//按键按下
        {    
            if(GetSysBaseTick() - systick > 1000*1000*42/2)//2秒后，
            {
                BSP_LED_On(0);    
                keycnt=1;
            }
            
            if(GetSysBaseTick() - systick > 1000*1000*84)//2秒后，
               return; 
        }
    
       printf("\r\n song");
        systick = GetSysBaseTick();
        while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) == SET && keycnt == 1) //按键松开
        {    
            if(GetSysBaseTick() - systick > 1000*1000*42/2)//2秒后，
                BSP_LED_Off(0);    
        
            if(GetSysBaseTick() - systick > 1000*1000*42)
                return;
        }
        
        printf("\r\n an");
        systick = GetSysBaseTick();
        while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) == RESET) //按键按下
        {    
            if(GetSysBaseTick() - systick > 1000*1000*42/2)//2秒后，
            {                             
                //BSP_LED_Off(0);    
/////////////////////////////////////////////////////////////////////////////////                    
                if((gs_runpara.chmodel &(0x01<<0)) == 0)
                {
                    gs_runpara.chmodel |= (0x01<<0);        //异侧
                    for(u8 i=0;i<4;i++)                     //点亮高四位
                        BSP_LED_On(i+5);
                    printf("\r\n 异侧");
                    delay_ms(2000);
                }
                else
                {
                    for(u8 i=0;i<4;i++)                     //点亮低四位
                        BSP_LED_On(i+1);                    
                    gs_runpara.chmodel &= ~(0x01<<0);
                    printf("\r\n 同侧");
                    delay_ms(2000);
                }

                gs_runpara.crc16 = GetCrc16Check((u8 *)&gs_runpara,sizeof(gs_runpara)-2);

                WriteFM24CL64(FRAM_RUN_PARA_ADDR,(u8 *)&gs_runpara,sizeof(gs_runpara));

                printf("\r\n设置通道模式 %d",gs_runpara.chmodel);

                
                //if(GetSysBaseTick() - systick > 1000*1000*42)//2秒后，
                return;                                      //
            }              
        }            
    
    }
    
    lsthandsts = handSts;
    
}



















