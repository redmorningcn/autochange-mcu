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

//ALIENTEK ̽����STM32F407������ ʵ��53
//USB U�� ʵ��-�⺯���汾 
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com
//������������ӿƼ����޹�˾    
//���ߣ�����ԭ�� @ALIENTEK 
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

#define MAX_ADDR       0x7FFFFF                                         /* ����оƬ�ڲ�����ַ         */
#define SEC_MAX        2048                                              /* �������������               */
#define SEC_SIZE       0x1000                                           /* ������С                     */
#define MAX_REC_NUM     ((MAX_ADDR )/sizeof(stcrecord))

void    FlashToUsbFile(u8 *storefilename)
{
    stcrecord   srecord;
    u32         recnum = 0;
    u32         wrnum;
    u32         rdnum;
    
    wrnum = gs_runpara.writerecnum;
    rdnum = gs_runpara.readnum;
    
    mf_open(storefilename,0x04);    //���ļ�

    mf_open(storefilename,0X02);    //   
    
    if(rdnum > wrnum)               //�Ѷ������¸�ֵ
    {
        gs_runpara.readnum = gs_runpara.writerecnum ;
        StoreRunPara();             //�����в���
    }
        
    while(  (rdnum  <  wrnum )       //ȡ�������˳�
        &&  GetchPulseInitFlg() == 0 //�������˳�
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
        IWDG_ReloadCounter();   //�忴�Ź�  
        if( (sysbasetic - systick) > tmp64)      //time = n * 9/84Mhz
        {
            systick =  sysbasetic;    
            BSP_LED_Toggle(17);     //����ָʾ��
        }
    }
    
    if(gs_runpara.readnum  != rdnum )
    {
        gs_runpara.readnum  = rdnum;
        StoreRunPara();
    }
           
    mf_close();                                                     //����ǰ����
    //uint32  addr  = sizeof(stcrecord)*((recnum) % MAX_REC_NUM );//ȡ�洢��ַ    
}

//�û�����������
//����ֵ:0,����
//       1,������
u8  g_usbuserflg = 0;

u8 USH_User_App(void)
{ 
	u32 total,free;
	u8 res=0;
    
    u32 delay = 100*100;
    while(delay--) //U�̳��ʱ��
    {
        IWDG_ReloadCounter();   //�忴�Ź�  
    }
    
//    printfcom1("\r\n �û����򣡣�");
    disk_initialize(2);
    
//    if(HCD_IsDeviceConnected(&USB_OTG_Core) == 0)     //
//        usbuserflg = 0;
    
    BSP_LED_On(17);

    printfcom1("\r\ng_usbuserflg %d",g_usbuserflg);
    if(g_usbuserflg == 1)                           //�Ѿ�������һ�Σ����ǰγ����²���
        return res;    

    
    u8  storefilename[36]={'\0'};                   //�ļ�����
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
	while(HCD_IsDeviceConnected(&USB_OTG_Core))         //�豸���ӳɹ�
    {	
        BSP_LED_On(17);                                 //����ָʾ�ƣ�����
        IWDG_ReloadCounter();   //�忴�Ź�  
        
        delay_ms(10);
        
        if(g_usbuserflg == 0)
            FlashToUsbFile(storefilename);          //����д��U���ļ�
        
        g_usbuserflg = 1;
       
        IWDG_ReloadCounter();   //�忴�Ź�
        //
        printfcom1("u��ɣ����˳�");

        uint64_t     outtime = GetSysBaseTick();
        while(HCD_IsDeviceConnected(&USB_OTG_Core))                        //��ʱ����룬�˳�
        {  
           IWDG_ReloadCounter();   //�忴�Ź�

           if(GetSysBaseTick() > outtime + 84000000*7)
           {
                printfcom1("u�̳�ʱ���˳�");
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

    iwdg_init();    //���Ź�
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);		//��ʱ��ʼ�� 
    
    delay_ms(10);

	uart_init(57600);	    //���ڳ�ʼ��������Ϊ57600

    EnUartContrlIO_Init();

    //printfcom1("\r\n\r\n�����͵���ϢΪ:\r\n");

    InitFramIO();

    InitFlashIO();

    delay_ms(10);
        
    InitFramIO();

    TIM2_Int_Init(0xffffFFFF,8);    //��ʱ��ʱ��84M����Ƶϵ��1������84M�ļ���Ƶ��  //0.78ms  

    EXTIX_Init();   
    
	LED_Init();                     //��ʼ����LED���ӵ�Ӳ���ӿ� 
    
    BSP_LED_Init();

    Set_Key_Init();
    
    Hand_Key_Init();
    
    InitDS3231();
    
    BSP_Sel_GPIO_Init();

    ReadCurch_Fram();
    
    IWDG_ReloadCounter();   //�忴�Ź�

    //////usb
    my_mem_init(SRAMIN);	//��ʼ���ڲ��ڴ��	    
    exfuns_init();			//Ϊfatfs��ر��������ڴ�
    USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks); 
    f_mount(fs[2],"2:",1); 	//����U��
        
//  TIM3_Int_Init(0xFFFF,8400-1);	//��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����5000��Ϊ500ms     
    
    //SetTime(11,11,11,17,00,00);
    
    JudgeFlashIDErrFlg();
    JudgeFlashIDErrFlg();

    ReadRunPara();
    
    ReadProInfo_Fram();
    
    //  TestUart();

    IWDG_ReloadCounter();   //�忴�Ź�

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
                            
            BSP_LED_Dis();                     //�Զ�λ������
            
            if(sysbasetic > 1000*1000*84*4 )        //36s��Ž����л�
            {
                AutoChangeCh();
            }

            for(u8 i = 0;i < 8 ;i++){           //ȡ��¼��Ϣ
                
                gs_storepluse[i].period = gs_pulsech[i].period;
                gs_storepluse[i].higvol = gs_pulsech[i].higvol;
                gs_storepluse[i].lowvol = gs_pulsech[i].lowvol;
                gs_storepluse[i].ratio  = gs_pulsech[i].ratio;
                gs_storepluse[i].flg    = gs_pulsech[i].flg;
            }                
            
            SetKey();
            
            if(     runtimes++ %1000==0 
                || (gs_runpara.flg & (0x01<<PLUSE_STORE))
              )      //״̬����)
            {
                IWDG_ReloadCounter();   //�忴�Ź�
                
                printfcom1();

                INTX_DISABLE();             //��ȫ���ж�
                StoreCurch_Fram();
                
                StoreRec();

                INTX_ENABLE();              //��ȫ���ж�

                gs_runpara.flg &= ~(0x01<<PLUSE_STORE);//�屣���ʶ
                printfcom1("\r\n store");
                //ReadCurch_Fram();
                //BSP_LED_Dis();
                UartSendRec(gs_runpara.writerecnum-1);  //���͵�ǰ��¼����
                delay_ms(50);
            }              
        }
        

        UartConTask();
        
        //�����ٶ�ʱ������U������Ӧ
         if((gs_runpara.flg & (0x01<< RUN_FLG)) == 0)
         {
             //printfcom1("\r\n ���źţ�����ͣ��");
             USBH_Process(&USB_OTG_Core, &USB_Host);     
         }           
         
         if(sysbasetic > 1000*1000*84/3 )
         {
            JudgePulse();   
            delay_us(10);
         }
         
         IWDG_ReloadCounter();   //�忴�Ź�
    }
}    


//int main(void)
//{        
//	u8 t;
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
//	delay_init(168);  //��ʼ����ʱ����
//	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
//    EnUartContrlIO_Init();
//    
//    printfcom1(" \r\n test usb!! ");

//	LED_Init();				//��ʼ����LED���ӵ�Ӳ���ӿ�
//	//KEY_Init();				//����
//  	//LCD_Init();				//��ʼ��LCD 
//	//W25QXX_Init();			//SPI FLASH��ʼ��
//	//usmart_dev.init(84); 	//��ʼ��USMART	 
//	my_mem_init(SRAMIN);	//��ʼ���ڲ��ڴ��	
// 	exfuns_init();			//Ϊfatfs��ر��������ڴ� 
//	//piclib_init();			//��ʼ����ͼ
//  	//f_mount(fs[0],"0:",1); 	//����SD��  
//  	//f_mount(fs[1],"1:",1); 	//����SD��  
//  	f_mount(fs[2],"2:",1); 	//����U��
//	//POINT_COLOR=RED;      
//// 	while(font_init()) 				//����ֿ�
////	{	    
////		LCD_ShowString(60,50,200,16,16,"Font Error!");
////		delay_ms(200);				  
////		LCD_Fill(60,50,240,66,WHITE);//�����ʾ	     
////		delay_ms(200);				  
////	}
////	Show_Str(30,50,200,16,"̽����STM32F407������",16,0);				    	 
////	Show_Str(30,70,200,16,"USB U��ʵ��",16,0);					    	 
////	Show_Str(30,90,200,16,"2014��7��22��",16,0);	    	 
////	Show_Str(30,110,200,16,"����ԭ��@ALIENTEK",16,0); 
////	Show_Str(30,140,200,16,"�豸������...",16,0);			 		
//	//��ʼ��USB����
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
