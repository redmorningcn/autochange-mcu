#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "usmart.h"  
#include "sram.h"   
#include "malloc.h" 
#include "w25qxx.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	
#include "piclib.h"	 
#include "usbh_usr.h" 
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "sensorpulse.h"

//ALIENTEK 探索者STM32F407开发板 实验53
//USB U盘 实验-库函数版本 
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司    
//作者：正点原子 @ALIENTEK 
//Time = (XX /(40*10^3))*Val(RLR) (s)
// = (32/40000)*4095 = 3.25s

uint64_t    GetSysBaseTick(void);

void iwdg_init(void){
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//??IWDG_PR?IWDG_RLR????
    IWDG_SetReload(0xfff);//???????0xfff
    IWDG_SetPrescaler(IWDG_Prescaler_32);//????????32   
    IWDG_Enable();//?????
}

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;

#define MAX_ADDR       0x7FFFFF                                         /* 定义芯片内部最大地址         */
#define SEC_MAX        2048                                              /* 定义最大扇区号               */
#define SEC_SIZE       0x1000                                           /* 扇区大小                     */
#define MAX_REC_NUM     ((MAX_ADDR )/sizeof(stcrecord))

void    FlashToUsbFile(u8 *storefilename)
{
    stcrecord   srecord;
    u32         recnum = 0;
    u32         wrnum;
    u32         rdnum;
    
    wrnum = gs_runpara.writerecnum;
    rdnum = gs_runpara.readnum;
    
    mf_open(storefilename,0x04);    //打开文件

    mf_open(storefilename,0X02);    //   
    
    if(rdnum > wrnum)               //已读，重新赋值
    {
        gs_runpara.readnum = gs_runpara.writerecnum ;
        StoreRunPara();             //存运行参数
    }
        
    while(  (rdnum  <  wrnum )       //取完数据退出
        &&  GetchPulseInitFlg() == 0 //有脉冲退出
        )        //
    {
        uint32  addr;
        uint32  times = 0;
        addr = sizeof(srecord)*((rdnum++) % MAX_REC_NUM );
    
        MX25L1602_RD(addr,sizeof(stcrecord),(u8 *)&srecord );        
        //printfcom1("\r\n srecord %d,%d,%d",sizeof(srecord),rdnum,wrnum);
        mf_write((u8 *)&srecord,sizeof(srecord));
        
        uint64_t  sysbasetic,tmp64,systick;     
        sysbasetic = GetSysBaseTick();
        tmp64 = 1000*1000*5;
        IWDG_ReloadCounter();   //清看门狗  
        if( (sysbasetic - systick) > tmp64)      //time = n * 9/84Mhz
        {
            systick =  sysbasetic;    
            BSP_LED_Toggle(17);     //运行指示灯
        }
    }
    
    if(gs_runpara.readnum  != rdnum )
    {
        gs_runpara.readnum  = rdnum;
        StoreRunPara();
    }
           
    mf_close();                                                     //读当前数据
    //uint32  addr  = sizeof(stcrecord)*((recnum) % MAX_REC_NUM );//取存储地址    
}

//用户测试主程序
//返回值:0,正常
//       1,有问题
u8  g_usbuserflg = 0;

u8 USH_User_App(void)
{ 
	u32 total,free;
	u8 res=0;
    
    u32 delay = 100*100;
    while(delay--) //U盘充电时间
    {
        IWDG_ReloadCounter();   //清看门狗  
    }
    
//    printfcom1("\r\n 用户程序！！");
    disk_initialize(2);
    
//    if(HCD_IsDeviceConnected(&USB_OTG_Core) == 0)     //
//        usbuserflg = 0;
    
    BSP_LED_On(17);

    printfcom1("\r\ng_usbuserflg %d",g_usbuserflg);
    if(g_usbuserflg == 1)                           //已经操作过一次，除非拔出重新操作
        return res;    

    
    u8  storefilename[36]={'\0'};                   //文件名称
    strcat(storefilename,"2:(");
    strcat(storefilename,gs_proinfo.pdid);
    strcat(storefilename,") ");
    
    u8  buf[16] ={'\0'};
    stcTime sTime;
    ReadTime((stcTime *)&sTime);
    
    buf[0]='\0';
    sprintf(buf,"%02d-%02d-%02d ",sTime.Year,sTime.Month,sTime.Date);
    strcat(storefilename,buf);
    
    buf[0]='\0';
    sprintf(buf,"%02d_%02d_%02d.QX8",sTime.Hour,sTime.Min,sTime.Sec);
    strcat(storefilename,buf);
    printfcom1("\r\n %s",storefilename);
    
    
    //
	while(HCD_IsDeviceConnected(&USB_OTG_Core))         //设备连接成功
    {	
        BSP_LED_On(17);                                 //运行指示灯，点亮
        IWDG_ReloadCounter();   //清看门狗  
        
        delay_ms(10);
        
        if(g_usbuserflg == 0)
            FlashToUsbFile(storefilename);          //数据写入U盘文件
        
        g_usbuserflg = 1;
       
        IWDG_ReloadCounter();   //清看门狗
        //
        printfcom1("u完成，请退出");

        uint64_t     outtime = GetSysBaseTick();
        while(HCD_IsDeviceConnected(&USB_OTG_Core))                        //长时间插入，退出
        {  
           IWDG_ReloadCounter();   //清看门狗

           if(GetSysBaseTick() > outtime + 84000000*7)
           {
                printfcom1("u盘超时，退出");
                return res;
           }
        }
	}
    
	return res;
} 

#define EnUartSend_IO()     GPIO_SetBits(GPIOA,GPIO_Pin_15)
#define EnUartRec_IO()      GPIO_ClearBits(GPIOA,GPIO_Pin_15)
 
extern uint64_t g_time3cnt;

int main(void)
{ 
	u8 t;
	u8 len;	
	u16 times=0;

    iwdg_init();    //看门狗
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);		//延时初始化 
    
    delay_ms(10);

	uart_init(57600);	    //串口初始化波特率为57600

    EnUartContrlIO_Init();

    //printfcom1("\r\n\r\n您发送的消息为:\r\n");

    InitFramIO();

    InitFlashIO();

    delay_ms(10);
        
    InitFramIO();

    TIM2_Int_Init(0xffffFFFF,8);    //定时器时钟84M，分频系数1，所以84M的计数频率  //0.78ms  

    EXTIX_Init();   
    
	LED_Init();                     //初始化与LED连接的硬件接口 
    
    BSP_LED_Init();

    Set_Key_Init();
    
    Hand_Key_Init();
    
    InitDS3231();
    
    BSP_Sel_GPIO_Init();

    ReadCurch_Fram();
    
    IWDG_ReloadCounter();   //清看门狗

    //////usb
    my_mem_init(SRAMIN);	//初始化内部内存池	    
    exfuns_init();			//为fatfs相关变量申请内存
    USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks); 
    f_mount(fs[2],"2:",1); 	//挂载U盘
        
//  TIM3_Int_Init(0xFFFF,8400-1);	//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数5000次为500ms     
    
    //SetTime(11,11,11,17,00,00);
    
    JudgeFlashIDErrFlg();
    JudgeFlashIDErrFlg();

    ReadRunPara();
    
    ReadProInfo_Fram();
    
    //  TestUart();

    IWDG_ReloadCounter();   //清看门狗

    printfcom1("\r\n");
    
    //InitDS3231();
    printfcom1();    
    printfcom1("\r\n");
    printfcom1("\r\n");

    SetKeyChModle();
    
    uint64_t    systick     = 0;
    uint32      runtimes    = 0; 
    
    while(1)
    {
        uint64_t  sysbasetic,tmp64;     
        sysbasetic = GetSysBaseTick();
        tmp64 = 1000*1000;

        HoldUartRecSta();
        
        if( (sysbasetic - systick) > tmp64)      //time = n * 9/84Mhz
        {
            systick =  sysbasetic;
                            
            BSP_LED_Dis();                     //自动位，开灯
            
            if(sysbasetic > 1000*1000*84*4 )        //36s后才进行切换
            {
                AutoChangeCh();
            }

            for(u8 i = 0;i < 8 ;i++){           //取记录信息
                
                gs_storepluse[i].period = gs_pulsech[i].period;
                gs_storepluse[i].higvol = gs_pulsech[i].higvol;
                gs_storepluse[i].lowvol = gs_pulsech[i].lowvol;
                gs_storepluse[i].ratio  = gs_pulsech[i].ratio;
                gs_storepluse[i].flg    = gs_pulsech[i].flg;
            }                
            
            SetKey();
            
            if(     runtimes++ %1000==0 
                || (gs_runpara.flg & (0x01<<PLUSE_STORE))
              )      //状态保存)
            {
                IWDG_ReloadCounter();   //清看门狗
                
                printfcom1();

                INTX_DISABLE();             //关全局中断
                StoreCurch_Fram();
                
                StoreRec();

                INTX_ENABLE();              //开全局中断

                gs_runpara.flg &= ~(0x01<<PLUSE_STORE);//清保存标识
                printfcom1("\r\n store");
                //ReadCurch_Fram();
                //BSP_LED_Dis();
                UartSendRec(gs_runpara.writerecnum-1);  //发送当前记录数据
                delay_ms(50);
            }              
        }
        

        UartConTask();
        
        //在有速度时，插入U盘无响应
         if((gs_runpara.flg & (0x01<< RUN_FLG)) == 0)
         {
             //printfcom1("\r\n 无信号，机车停车");
             USBH_Process(&USB_OTG_Core, &USB_Host);     
         }           
         
         if(sysbasetic > 1000*1000*84/3 )
         {
            JudgePulse();   
            delay_us(10);
         }
         
         IWDG_ReloadCounter();   //清看门狗
    }
}    


//int main(void)
//{        
//	u8 t;
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
//	delay_init(168);  //初始化延时函数
//	uart_init(115200);		//初始化串口波特率为115200
//    EnUartContrlIO_Init();
//    
//    printfcom1(" \r\n test usb!! ");

//	LED_Init();				//初始化与LED连接的硬件接口
//	//KEY_Init();				//按键
//  	//LCD_Init();				//初始化LCD 
//	//W25QXX_Init();			//SPI FLASH初始化
//	//usmart_dev.init(84); 	//初始化USMART	 
//	my_mem_init(SRAMIN);	//初始化内部内存池	
// 	exfuns_init();			//为fatfs相关变量申请内存 
//	//piclib_init();			//初始化画图
//  	//f_mount(fs[0],"0:",1); 	//挂载SD卡  
//  	//f_mount(fs[1],"1:",1); 	//挂载SD卡  
//  	f_mount(fs[2],"2:",1); 	//挂载U盘
//	//POINT_COLOR=RED;      
//// 	while(font_init()) 				//检查字库
////	{	    
////		LCD_ShowString(60,50,200,16,16,"Font Error!");
////		delay_ms(200);				  
////		LCD_Fill(60,50,240,66,WHITE);//清除显示	     
////		delay_ms(200);				  
////	}
////	Show_Str(30,50,200,16,"探索者STM32F407开发板",16,0);				    	 
////	Show_Str(30,70,200,16,"USB U盘实验",16,0);					    	 
////	Show_Str(30,90,200,16,"2014年7月22日",16,0);	    	 
////	Show_Str(30,110,200,16,"正点原子@ALIENTEK",16,0); 
////	Show_Str(30,140,200,16,"设备连接中...",16,0);			 		
//	//初始化USB主机
//  	USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);  

//    while(1)
//	{
//		//USBH_Process(&USB_OTG_Core, &USB_Host);
//		delay_ms(1);
//		t++;
//		if(t==200)
//		{
//			LED0=!LED0;
//			t=0;
//		}
//	}	
//}
