#include "led.h" 
#include "stm32f4xx.h" 
#include "stm32f4xx_gpio.h" 
#include "sensorpulse.h" 


//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 


//PD
#define  BSP_GPIO_LED_R1                        GPIO_Pin_14
#define  BSP_GPIO_LED_R2                        GPIO_Pin_15
#define  BSP_GPIO_LED_R3                        GPIO_Pin_00
#define  BSP_GPIO_LED_R4                        GPIO_Pin_01
//PE                                               
#define  BSP_GPIO_LED_R5                        GPIO_Pin_07
#define  BSP_GPIO_LED_R6                        GPIO_Pin_08
#define  BSP_GPIO_LED_R7                        GPIO_Pin_09
#define  BSP_GPIO_LED_R8                        GPIO_Pin_10

//PE
#define  BSP_GPIO_LED_G1                        GPIO_Pin_11
#define  BSP_GPIO_LED_G2                        GPIO_Pin_12
#define  BSP_GPIO_LED_G3                        GPIO_Pin_13
#define  BSP_GPIO_LED_G4                        GPIO_Pin_14
#define  BSP_GPIO_LED_G5                        GPIO_Pin_15
//PD
#define  BSP_GPIO_LED_G6                        GPIO_Pin_08
#define  BSP_GPIO_LED_G7                        GPIO_Pin_09
#define  BSP_GPIO_LED_G8                        GPIO_Pin_10
//PC                                            
#define  BSP_GPIO_LED_RUN                       GPIO_Pin_09

#define  GPIO_Low_Speed                         GPIO_SPEED_LOW

//初始化PF9和PF10为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOF时钟

  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化
	
  GPIO_SetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10);//GPIOF9,F10设置高，灯灭

}


extern  void  BSP_LED_Off (CPU_INT08U  led);


/*
*********************************************************************************************************
*                                           BSP_LED_Init()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    inialize ALL  LEDs
*                       1    inialize user LED1
*                       2    inialize user LED2
*                       3    inialize user LED3
*                       4    inialize user LED4
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Init(void)
{
    GPIO_InitTypeDef  gpio_init;

    gpio_init.GPIO_Mode = GPIO_Mode_OUT;        //普通输出模式
    gpio_init.GPIO_OType = GPIO_OType_PP;       //推挽输出
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;   //100MHz
    gpio_init.GPIO_PuPd = GPIO_PuPd_UP;         //上拉    
    
    gpio_init.GPIO_Pin   = BSP_GPIO_LED_RUN;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能时钟
    GPIO_Init(GPIOC, &gpio_init);               //初始化
    
    gpio_init.GPIO_Pin  = BSP_GPIO_LED_R1 
                        | BSP_GPIO_LED_R2 
                        | BSP_GPIO_LED_R3
                        | BSP_GPIO_LED_R4
                        | BSP_GPIO_LED_G6 
                        | BSP_GPIO_LED_G7
                        | BSP_GPIO_LED_G8;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能时钟
    GPIO_Init(GPIOD, &gpio_init);//初始化

    gpio_init.GPIO_Pin  = BSP_GPIO_LED_R5 
                        | BSP_GPIO_LED_R6
                        | BSP_GPIO_LED_R7
                        | BSP_GPIO_LED_R8
                        | BSP_GPIO_LED_G1 
                        | BSP_GPIO_LED_G2
                        | BSP_GPIO_LED_G3
                        | BSP_GPIO_LED_G4
                        | BSP_GPIO_LED_G5;
        
//    BSP_PeriphEn(BSP_PERIPH_ID_GPIOE);                          /* Configure GPIOI for LED3                             */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOF时钟
//    GPIO_Init(GPIOD, &gpio_init);
    GPIO_Init(GPIOE, &gpio_init);//初始化
    
    BSP_LED_Off(0);
}


/*
*********************************************************************************************************
*                                              BSP_LED_Off()
*
* Description : Turn OFF any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    turns OFF ALL the LEDs
*                       1    turns OFF user LED1
*                       2    turns OFF user LED2
*                       3    turns OFF user LED3
*                       4    turns OFF user LED4
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void  BSP_LED_Off (CPU_INT08U  led)
{
    switch (led) {
        case 0u:
             GPIO_WriteBit(GPIOD, (    BSP_GPIO_LED_R1  | 
                                       BSP_GPIO_LED_R2  | 
                                       BSP_GPIO_LED_R3  | 
                                       BSP_GPIO_LED_R4  | 
                                       BSP_GPIO_LED_G6  | 
                                       BSP_GPIO_LED_G7  | 
                                       BSP_GPIO_LED_G8                                       
                                       ), Bit_SET);
             GPIO_WriteBit(GPIOE, (    BSP_GPIO_LED_G1  | 
                                       BSP_GPIO_LED_G2  | 
                                       BSP_GPIO_LED_G3  | 
                                       BSP_GPIO_LED_G4  | 
                                       BSP_GPIO_LED_G5  | 
                                       BSP_GPIO_LED_R5  | 
                                       BSP_GPIO_LED_R6  |
                                       BSP_GPIO_LED_R7  | 
                                       BSP_GPIO_LED_R8
                                       ), Bit_SET);
                          
             GPIO_WriteBit(GPIOC, (BSP_GPIO_LED_RUN
                                       ), Bit_SET);
             break;

        case 1u:    GPIO_WriteBit(GPIOD, BSP_GPIO_LED_R1, Bit_SET);    break;
        case 2u:    GPIO_WriteBit(GPIOD, BSP_GPIO_LED_R2, Bit_SET);    break;
        case 3u:    GPIO_WriteBit(GPIOD, BSP_GPIO_LED_R3, Bit_SET);    break;
        case 4u:    GPIO_WriteBit(GPIOD, BSP_GPIO_LED_R4, Bit_SET);    break;
        case 5u:    GPIO_WriteBit(GPIOE, BSP_GPIO_LED_R5, Bit_SET);    break;
        case 6u:    GPIO_WriteBit(GPIOE, BSP_GPIO_LED_R6, Bit_SET);    break;
        case 7u:    GPIO_WriteBit(GPIOE, BSP_GPIO_LED_R7, Bit_SET);    break;
        case 8u:    GPIO_WriteBit(GPIOE, BSP_GPIO_LED_R8, Bit_SET);    break;
        case 9u:    GPIO_WriteBit(GPIOE, BSP_GPIO_LED_G1, Bit_SET);    break;
        case 10u:   GPIO_WriteBit(GPIOE, BSP_GPIO_LED_G2, Bit_SET);    break;
        case 11u:   GPIO_WriteBit(GPIOE, BSP_GPIO_LED_G3, Bit_SET);    break;
        case 12u:   GPIO_WriteBit(GPIOE, BSP_GPIO_LED_G4, Bit_SET);    break;
        case 13u:   GPIO_WriteBit(GPIOE, BSP_GPIO_LED_G5, Bit_SET);    break;
        case 14u:   GPIO_WriteBit(GPIOD, BSP_GPIO_LED_G6, Bit_SET);    break;
        case 15u:   GPIO_WriteBit(GPIOD, BSP_GPIO_LED_G7, Bit_SET);    break;
        case 16u:   GPIO_WriteBit(GPIOD, BSP_GPIO_LED_G8, Bit_SET);    break;
        case 17u:   GPIO_WriteBit(GPIOC, BSP_GPIO_LED_RUN,Bit_SET);    break;
        default:
             break;
    }
}


/*
*********************************************************************************************************
*                                             BSP_LED_On()
*
* Description : Turn ON any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    turns ON ALL  LEDs
*                       1    turns ON user LED1
*                       2    turns ON user LED2
*                       3    turns ON user LED3
*                       4    turns ON user LED4
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_PulseSel( CPU_INT08U ch1, CPU_INT08U ch2 );

void  BSP_LED_On (CPU_INT08U led)
{
    switch (led) {
        case 0u:             
             GPIO_WriteBit(GPIOD, (    BSP_GPIO_LED_R1  | 
                                       BSP_GPIO_LED_R2  | 
                                       BSP_GPIO_LED_R3  | 
                                       BSP_GPIO_LED_R4  | 
                                       BSP_GPIO_LED_G6  | 
                                       BSP_GPIO_LED_G7  | 
                                       BSP_GPIO_LED_G8  
                                       ), Bit_RESET);
             GPIO_WriteBit(GPIOE, (    BSP_GPIO_LED_G1  | 
                                       BSP_GPIO_LED_G2  | 
                                       BSP_GPIO_LED_G3  | 
                                       BSP_GPIO_LED_G4  | 
                                       BSP_GPIO_LED_G5  | 
                                       BSP_GPIO_LED_R5  | 
                                       BSP_GPIO_LED_R6  |
                                       BSP_GPIO_LED_R7  | 
                                       BSP_GPIO_LED_R8
                                       ), Bit_RESET);
             
             GPIO_WriteBit(GPIOC, (BSP_GPIO_LED_RUN
                                       ), Bit_RESET);
             break;

        case 1u:    GPIO_WriteBit(GPIOD, BSP_GPIO_LED_R1, Bit_RESET);    break;
        case 2u:    GPIO_WriteBit(GPIOD, BSP_GPIO_LED_R2, Bit_RESET);    break;
        case 3u:    GPIO_WriteBit(GPIOD, BSP_GPIO_LED_R3, Bit_RESET);    break;
        case 4u:    GPIO_WriteBit(GPIOD, BSP_GPIO_LED_R4, Bit_RESET);    break;
        case 5u:    GPIO_WriteBit(GPIOE, BSP_GPIO_LED_R5, Bit_RESET);    break;
        case 6u:    GPIO_WriteBit(GPIOE, BSP_GPIO_LED_R6, Bit_RESET);    break;
        case 7u:    GPIO_WriteBit(GPIOE, BSP_GPIO_LED_R7, Bit_RESET);    break;
        case 8u:    GPIO_WriteBit(GPIOE, BSP_GPIO_LED_R8, Bit_RESET);    break;
        case 9u:    GPIO_WriteBit(GPIOE, BSP_GPIO_LED_G1, Bit_RESET);    break;
        case 10u:   GPIO_WriteBit(GPIOE, BSP_GPIO_LED_G2, Bit_RESET);    break;
        case 11u:   GPIO_WriteBit(GPIOE, BSP_GPIO_LED_G3, Bit_RESET);    break;
        case 12u:   GPIO_WriteBit(GPIOE, BSP_GPIO_LED_G4, Bit_RESET);    break;
        case 13u:   GPIO_WriteBit(GPIOE, BSP_GPIO_LED_G5, Bit_RESET);    break;
        case 14u:   GPIO_WriteBit(GPIOD, BSP_GPIO_LED_G6, Bit_RESET);    break;
        case 15u:   GPIO_WriteBit(GPIOD, BSP_GPIO_LED_G7, Bit_RESET);    break;
        case 16u:   GPIO_WriteBit(GPIOD, BSP_GPIO_LED_G8, Bit_RESET);    break;
        case 17u:   GPIO_WriteBit(GPIOC, BSP_GPIO_LED_RUN,Bit_RESET);    break;

        default:
             break;
    }
}


/*
*********************************************************************************************************
*                                            BSP_LED_Toggle()
*
* Description : TOGGLE any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    TOGGLE ALL the LEDs
*                       1    TOGGLE user LED1
*                       2    TOGGLE user LED2
*                       3    TOGGLE user LED3
*                       4    TOGGLE user LED4
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Toggle (CPU_INT08U  led)
{
    switch (led) {
        case 0u:
             
             GPIO_ToggleBits(GPIOD, (  BSP_GPIO_LED_R1  | 
                                       BSP_GPIO_LED_R2  | 
                                       BSP_GPIO_LED_R3  | 
                                       BSP_GPIO_LED_R4  | 
                                       BSP_GPIO_LED_G6  | 
                                       BSP_GPIO_LED_G7  | 
                                       BSP_GPIO_LED_G8  
                                       ));
             GPIO_ToggleBits(GPIOE, (  BSP_GPIO_LED_G1  | 
                                       BSP_GPIO_LED_G2  | 
                                       BSP_GPIO_LED_G3  | 
                                       BSP_GPIO_LED_G4  | 
                                       BSP_GPIO_LED_G5  | 
                                       BSP_GPIO_LED_R5  | 
                                       BSP_GPIO_LED_R6  |
                                       BSP_GPIO_LED_R7  | 
                                       BSP_GPIO_LED_R8
                                       ));
             GPIO_ToggleBits(GPIOC, BSP_GPIO_LED_RUN);
             break;

        case 1u:    GPIO_ToggleBits(GPIOD, BSP_GPIO_LED_R1);   break;
        case 2u:    GPIO_ToggleBits(GPIOD, BSP_GPIO_LED_R2);   break;
        case 3u:    GPIO_ToggleBits(GPIOD, BSP_GPIO_LED_R3);   break;
        case 4u:    GPIO_ToggleBits(GPIOD, BSP_GPIO_LED_R4);   break;
        case 5u:    GPIO_ToggleBits(GPIOE, BSP_GPIO_LED_R5);   break;
        case 6u:    GPIO_ToggleBits(GPIOE, BSP_GPIO_LED_R6);   break;
        case 7u:    GPIO_ToggleBits(GPIOE, BSP_GPIO_LED_R7);   break;
        case 8u:    GPIO_ToggleBits(GPIOE, BSP_GPIO_LED_R8);   break;
        case 9u:    GPIO_ToggleBits(GPIOE, BSP_GPIO_LED_G1);   break;
        case 10u:   GPIO_ToggleBits(GPIOE, BSP_GPIO_LED_G2);   break;
        case 11u:   GPIO_ToggleBits(GPIOE, BSP_GPIO_LED_G3);   break;
        case 12u:   GPIO_ToggleBits(GPIOE, BSP_GPIO_LED_G4);   break;
        case 13u:   GPIO_ToggleBits(GPIOE, BSP_GPIO_LED_G5);   break;
        case 14u:   GPIO_ToggleBits(GPIOD, BSP_GPIO_LED_G6);   break;
        case 15u:   GPIO_ToggleBits(GPIOD, BSP_GPIO_LED_G7);   break;
        case 16u:   GPIO_ToggleBits(GPIOD, BSP_GPIO_LED_G8);   break;
        case 17u:   GPIO_ToggleBits(GPIOC, BSP_GPIO_LED_RUN);  break;
        default:
             break;
    }
}
u8  g_ch1outnum = 0;    //默认为0+1通道
u8  g_ch2outnum = 1;    //默认为1+1通道

extern  u32 g_time3cnt;
void    BSP_LED_Dis(void)
{
    static  u32 systime;
    u8      i = 0;

//    if(g_time3cnt - systime > 500 )      //50ms
    {
        systime = g_time3cnt;
        //printf("\r\n systime %d",g_time3cnt);
        
//        g_ch1outnum = 6;
//        g_ch2outnum = 7;
        
        for(i = 0;i <8 ;i++)
        {
            //if ( gs_pulsech[i].flg & (0x01 << CH_IN) ) {
            if (    ( gs_pulsech[i].flg & (0x01 << CH_OK)) 
                &&  (gs_pulsech[i].flg & (0x01 << CH_IN) )) 
            {
                if ( i == gs_selch.selch1num) 
                {
                    if(( gs_selch.selch1num >= 0 )&&( gs_selch.selch1num < 8 )) {  //工作通道闪烁
                    BSP_LED_Toggle(i+1);
                    //printf("\r\n ch1i %d %d %d",i,gs_selch.selch1num,gs_selch.selch2num);
                    } else {
                        BSP_LED_On(i+1);
                    }
                } else if ( i == gs_selch.selch2num ) 
                {
                    if(( gs_selch.selch2num >= 0 )&&( gs_selch.selch2num < 8 )) {  //工作通道闪烁
                    //printf("\r\n ch2i %d",i);
                    BSP_LED_Toggle(i+1);
                }
                } else {
                    BSP_LED_On(i+1);                    
                }
            } else {
                if((i != gs_selch.selch1num) && (i != gs_selch.selch2num) )
                    BSP_LED_Off(i+1);                
            }    
        
        //////////////停车也亮通道选择灯
            if (   ((gs_runpara.flg & (0x01<< RUN_FLG)) == 0)
                 &&((i == gs_selch.selch1num) ||( i == gs_selch.selch2num)) 
                ) 
            {
                BSP_LED_On(i+1);
            } 
            /////////////////////////////////
        }
                
        stcchpulse  *p  = &gs_pulsech[0];
        
        for( u8 i   = 0; i < 8; i++, p++ ) {
            if(   // (p->flg & (0x01  << CH_IN)) &&
                        ((p->flg & (0x01 << CH_ERR_LOSE)) 
                    ||  (p->flg & (0x01 << CH_ERR_FRQ)) ) ){
                BSP_LED_On(9+i);
            }  
            else
            {
                BSP_LED_Off(9+i);
            }  
        }
        
        BSP_LED_Toggle(17);
    }   
}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void BSP_Sel_GPIO_Init(void)
{
    GPIO_InitTypeDef  gpio_init;

    gpio_init.GPIO_Mode     = GPIO_Mode_OUT;        //普通输出模式
    gpio_init.GPIO_OType    = GPIO_OType_PP;        //推挽输出
    gpio_init.GPIO_Speed    = GPIO_Speed_100MHz;    //100MHz
    gpio_init.GPIO_PuPd     = GPIO_PuPd_NOPULL;       //下拉    
    gpio_init.GPIO_Pin      = GPIO_Pin_3 
                            | GPIO_Pin_4 
                            | GPIO_Pin_5
                            | GPIO_Pin_11
                            | GPIO_Pin_12
                            | GPIO_Pin_13;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能时钟
    GPIO_Init(GPIOD, &gpio_init);//初始化
    
    
    uint8   sel;
    ReadFM24CL64( FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));//读通道数据
    if(gs_selch.crc16 != GetCrc16Check((uint8 *)&gs_selch,sizeof(gs_selch)-2) )
    {
        printf("通道选择参数错误！");
        gs_selch.selchgroup = 0;
        gs_selch.selch1num = gs_selch.selchgroup*2;
        gs_selch.selch2num = gs_selch.selch1num + 1;
        gs_selch.crc16 = GetCrc16Check((u8 *)&gs_selch,sizeof(gs_selch)-2 );
        
        WriteFM24CL64( FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));
    }
    else
    {
        gs_selch.selch1num %= 8;
        gs_selch.selch2num %= 8;
    }
    
    printf("\r\n gs_selch %d,%d,%d,   %d,%d,%d",
                gs_selch.selchgroup,
                gs_selch.selch1num,
                gs_selch.selch2num,
                
                gs_selch.selchgroupbak,                
                gs_selch.selch1numbak,
                gs_selch.selch2numbak
                );
    
//    
//    if( gs_selch.selchgroup >3)  //通道组数大于4
//    {
//        gs_selch.selchgroup = 0;
//        gs_selch.selch1num = gs_selch.selchgroup*2;
//        gs_selch.selch2num = gs_selch.selch1num + 1;
//        gs_selch.crc16 = GetCrc16Check((u8 *)&gs_selch,sizeof(gs_selch)-2 );
//        WriteFM24CL64( FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));
//        
//    }
////        ){
//            gs_selch.selch1num = gs_selch.selchgroup*2;
//            gs_selch.selch2num = gs_selch.selch1num + 1;
//            gs_selch.crc16 = GetCrc16Check((u8 *)&gs_selch,sizeof(gs_selch)-2 );
//            WriteFM24CL64( FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));
//    }else{
//        if(gs_selch.selchgroupbak <4){
//            gs_selch.selchgroup  = gs_selch.selchgroupbak;
//            
//            gs_selch.selch1num   = gs_selch.selchgroup * 2;
//            gs_selch.selch2num   = gs_selch.selch1num + 1;
//            gs_selch.selchgroupbak  = 5;        //设置无效组号
//            
//            BSP_PulseSel( gs_selch.selch1num,  gs_selch.selch2num );
//            
//            gs_selch.crc16 = GetCrc16Check((u8 *)&gs_selch,sizeof(gs_selch)-2 );
//            WriteFM24CL64( FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));
//            printf("\r\n 11 gs_selch.selch1numbak %d %d",gs_selch.selch1num,gs_selch.selch2num);
//            }
//        }
    
    BSP_PulseSel( gs_selch.selch1num,  gs_selch.selch2num );
}   
    

void BSP_PulseSel( CPU_INT08U ch1, CPU_INT08U ch2 )
{    
    switch(ch1) {
        case 0:   
            GPIO_WriteBit(GPIOD, GPIO_PIN_3,  Bit_RESET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_4,  Bit_RESET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_5,  Bit_RESET);
            break;
        case 1: 
            GPIO_WriteBit(GPIOD, GPIO_PIN_3,  Bit_RESET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_4,  Bit_RESET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_5,  Bit_SET);
            break;            
        case 2: 
            GPIO_WriteBit(GPIOD, GPIO_PIN_3,  Bit_RESET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_4,  Bit_SET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_5,  Bit_RESET);
            break; 
        case 3: 
            GPIO_WriteBit(GPIOD, GPIO_PIN_3,  Bit_RESET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_4,  Bit_SET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_5,  Bit_SET);
            break; 
        case 4: 
            GPIO_WriteBit(GPIOD, GPIO_PIN_3,  Bit_SET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_4,  Bit_RESET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_5,  Bit_RESET);
            break; 
        case 5: 
            GPIO_WriteBit(GPIOD, GPIO_PIN_3,  Bit_SET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_4,  Bit_RESET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_5,  Bit_SET);
            break; 
        case 6: 
            GPIO_WriteBit(GPIOD, GPIO_PIN_3,  Bit_SET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_4,  Bit_SET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_5,  Bit_RESET);
            break; 
        case 7: 
            GPIO_WriteBit(GPIOD, GPIO_PIN_3,  Bit_SET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_4,  Bit_SET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_5,  Bit_SET);
            break; 
    default:        
            break;
    }
    
    switch(ch2) {
        case 0:   
            GPIO_WriteBit(GPIOD, GPIO_PIN_11,  Bit_RESET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_12,  Bit_RESET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_13,  Bit_RESET);
            break;
        case 1: 
            GPIO_WriteBit(GPIOD, GPIO_PIN_11,  Bit_RESET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_12,  Bit_RESET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_13,  Bit_SET);
            break;            
        case 2: 
            GPIO_WriteBit(GPIOD, GPIO_PIN_11,  Bit_RESET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_12,  Bit_SET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_13,  Bit_RESET);
            break; 
        case 3: 
            GPIO_WriteBit(GPIOD, GPIO_PIN_11,  Bit_RESET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_12,  Bit_SET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_13,  Bit_SET);
            break; 
        case 4: 
            GPIO_WriteBit(GPIOD, GPIO_PIN_11,  Bit_SET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_12,  Bit_RESET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_13,  Bit_RESET);
            break; 
        case 5: 
            GPIO_WriteBit(GPIOD, GPIO_PIN_11,  Bit_SET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_12,  Bit_RESET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_13,  Bit_SET);
            break; 
        case 6: 
            GPIO_WriteBit(GPIOD, GPIO_PIN_11,  Bit_SET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_12,  Bit_SET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_13,  Bit_RESET);
            break; 
        case 7: 
            GPIO_WriteBit(GPIOD, GPIO_PIN_11,  Bit_SET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_12,  Bit_SET);
            GPIO_WriteBit(GPIOD, GPIO_PIN_13,  Bit_SET);
            break; 
    default:        
            break;
    }
}


