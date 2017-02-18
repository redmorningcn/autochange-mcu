#include "sensorpulse.h"
#include "delay.h" 
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

#include "stdint.h"

#include  <stdarg.h>
#include  <stdio.h>
#include  <math.h>
#if defined(USE_HAL_DRIVER)
#include  <stm32f4xx_hal.h>
#else
#include  <stm32f4xx.h>
#endif

#include "MX25L1602Drv.h"
#include "sensorpulse.h"

void    ReadRunPara(void);
void    printfflg(void);

void    StoreCurch_Fram(void)
{
    INTX_DISABLE();             //关全局中断

    for(u8 i = 0 ; i < 8 ;i++){
        u32 addr;
        addr = sizeof(stcchpulse) * i + FRAM_CUR_CH_INFO;
       
        WriteFM24CL64(addr,(u8 *)&gs_pulsech[i],sizeof(stcchpulse));
    }
    INTX_ENABLE();             //关全局中断
}


void    StoreProInfo_Fram(void)         //存产品信息
{
    INTX_DISABLE();                     //关全局中断

    //for(u8 i = 0 ; i < 8 ;i++){
        u32 addr;
        addr = FRAM_PD_INFO_ADDR;
        //strcpy((u8 *)gs_proinfo.pdid,"1612001");
        WriteFM24CL64(addr,(u8 *)&gs_proinfo,sizeof(stcproinfo));
    //}
    INTX_ENABLE();             //关全局中断
}
void    ReadProInfo_Fram(void)         //存产品信息
{
    INTX_DISABLE();                     //关全局中断

    //for(u8 i = 0 ; i < 8 ;i++){
        u32 addr;
        addr = FRAM_PD_INFO_ADDR;
        //strcpy((u8 *)gs_proinfo.pdid,"1612001");
        ReadFM24CL64(addr,(u8 *)&gs_proinfo,sizeof(stcproinfo));
    //}
    printfcom1("\r\n ");
    printfcom1("\r\n 产品编号：%s",gs_proinfo.pdid);
    INTX_ENABLE();             //关全局中断
}


void    printfch(void)
{
    for(u8 i = 0 ; i < 8 ;i++){        
        printf("\r\n gs_pulsech \t%d,\t%d,\t%x,\t%d",i,
                        gs_pulsech[i].period,
                        gs_pulsech[i].flg,
                        gs_pulsech[i].lstrisetime);
    }
}

void    ReadCurch_Fram(void)
{
    u16 flg;
    
    INTX_DISABLE();             //关全局中断

    for(u8 i = 0 ; i < 8 ;i++){        
        u32 addr;
        addr = sizeof(stcchpulse) * i + FRAM_CUR_CH_INFO;
        
        ReadFM24CL64(addr,(u8 *)&gs_pulsech[i],sizeof(stcchpulse));
        
        flg = gs_pulsech[i].flg;
        flg &=   ~(   (0x01<< CH_IN)\
                                |   (0x01<< CH_OK)
                                |   (0x01<< PLUSE_STORE)
                                |   (0x01<< PLUSE_FRQ_E)
                                |   (0x01<< PLUSE_LOSE_E)
                                |   (0x01<< PLUSE_NEW)
                                |   (0x01<< LONG_TIME_NO_SING)
                               );
                
        memset(&gs_pulsech[i],0,sizeof(gs_pulsech[i]));
    
        gs_pulsech[i].flg = flg;

        printf(" \r\n flg %x",gs_pulsech[i].flg);
    }
    INTX_ENABLE();             //关全局中断

    printfch();
}

void    Initch(void)
{
    u16 flg;

    for(u8 i = 0 ; i < 8 ;i++){

        memset(&gs_pulsech[i],0,sizeof(gs_pulsech[i]));
    }
}

void    Initchexcptflg(void)
{
    u16 flg;

    for(u8 i = 0 ; i < 8 ;i++){
        
        u32 addr;
        flg = gs_pulsech[i].flg;
        flg &=      ~(   (0x01<< CH_IN)\
                    |   (0x01<< CH_OK)
                    |   (0x01<< PLUSE_STORE)
                    |   (0x01<< PLUSE_FRQ_E)
                    |   (0x01<< PLUSE_LOSE_E)
                    |   (0x01<< PLUSE_NEW)
                    |   (0x01<< LONG_TIME_NO_SING)
                    |   (0x01<< TEN_PLUSE)
                    |   (0x01<< FIVE_PLUSE)
                    |   (0x01<< SIDE_FLG)
                    );
        memset(&gs_pulsech[i],0,sizeof(gs_pulsech[i]));
        
        gs_pulsech[i].flg = flg;
    }
}



//      uint32      period;                 //
//      uint8_t     ratio;                  //pulse ratio = (lstfalltime - lstrisetime) / period;
//      uint8_t     higvol;                 //hight voltage value = (3.3/ 256)*higvol;
//      uint8_t     lowvol;                 //low voltage   value = (3.3/ 256)*lowvol;
//      uint8_t     flg;  

#define MAX_REC_NUM     ((MAX_ADDR )/sizeof(stcrecord))
    
//#
void    StoreRec(void)
{
    stcrecord   srecord;
    
    static  u32 times = 0;
    srecord.type =0;
    if(times++ ==0){
        printf("\r\n first run!");
        srecord.type = (0x01<<START_FLG);
    }else{
        if((gs_runpara.flg & (0x01<<PLUSE_STORE))){
            srecord.type = (0x01<<STORE_FLG);
        }
    }
    
//    INTX_DISABLE();             //关全局中断

    ReadTime( (stcTime *)&srecord.stime );         //取时间信息
    PrintfTime();

    for(u8 i = 0;i < 8 ;i++){           //取记录信息
        
//        srecord.spulse[i].period = gs_pulsech[i].period;
//        srecord.spulse[i].higvol = gs_pulsech[i].higvol;
//        srecord.spulse[i].lowvol = gs_pulsech[i].lowvol;
//        srecord.spulse[i].ratio  = gs_pulsech[i].ratio;
//        srecord.spulse[i].flg    = gs_pulsech[i].flg;
        srecord.spulse[i].period = gs_storepluse[i].period;
        srecord.spulse[i].higvol = gs_storepluse[i].higvol;
        srecord.spulse[i].lowvol = gs_storepluse[i].lowvol;
        srecord.spulse[i].ratio  = gs_storepluse[i].ratio;
        srecord.spulse[i].flg    = gs_storepluse[i].flg;
    }
    
    printf("\r\n store in");
    for(u8 i = 0; i < 8;i++)
    {
        printf("\t %5d",84000000/(srecord.spulse[i].period*9));
    }
    //printfflg();
    
    srecord.sselch = gs_selch;         //
    srecord.crc16 = GetCrc16Check((u8 *)&srecord,sizeof(srecord)-2);

    uint32  addr  = sizeof(stcrecord)*((gs_runpara.writerecnum++) % MAX_REC_NUM );//取存储地址
    MX25L1602_WR(addr,(u8 *)&srecord,sizeof(stcrecord) );
    
    uint32  recnum = gs_runpara.writerecnum - 1;
    addr  = sizeof(stcrecord)*((recnum) % MAX_REC_NUM );//取存储地址
    
    MX25L1602_RD(addr,sizeof(stcrecord),(u8 *)&srecord );
    
    gs_runpara.crc16 = GetCrc16Check((u8 *)&gs_runpara,sizeof(gs_runpara)-2);
    
    WriteFM24CL64(FRAM_RUN_PARA_ADDR,(u8 *)&gs_runpara,sizeof(gs_runpara));
    
//ReadRunPara();
//INTX_ENABLE();             //关全局中断
}

//
//
//
//
//
void    ReadRunPara(void)
{
    ReadFM24CL64(FRAM_RUN_PARA_ADDR,(u8 *)&gs_runpara,sizeof(gs_runpara));
    if(gs_runpara.crc16 != GetCrc16Check((uint8 *)&gs_runpara,sizeof(gs_runpara)-2))
    {
        gs_runpara.writerecnum = 0;
        gs_runpara.readnum     = 0;
        gs_runpara.chmodel     = 0;
        gs_runpara.runmodel    = 0;

        printf("\r\n 运行参数错误！");
    }
    
   //gs_runpara.writerecnum = 0;
    printf("\r\n运行参数：%d，%d，%d，%d， %x， "
                                        ,gs_runpara.writerecnum
                                        ,gs_runpara.readnum
                                        ,gs_runpara.flg
                                        ,gs_runpara.runmodel
                                        ,gs_runpara.chmodel
                                        );
}

void    StoreRunPara(void)
{
    gs_runpara.crc16 = GetCrc16Check((uint8 *)&gs_runpara,sizeof(gs_runpara)-2);
    WriteFM24CL64(FRAM_RUN_PARA_ADDR,(u8 *)&gs_runpara,sizeof(gs_runpara));
}

void    PrintfRecord(u32    recnum)
{
    stcrecord   srecord;
        
    uint32  addr  = sizeof(stcrecord)*((recnum) % MAX_REC_NUM );//取存储地址
    
    MX25L1602_RD(addr,sizeof(stcrecord),(u8 *)&srecord );
    
	printfcom1("\r\n%u-%u-%u %2u:%2u:%2u,"
                                            ,srecord.stime.Year
                                            ,srecord.stime.Month
                                            ,srecord.stime.Date
                                            ,srecord.stime.Hour
                                            ,srecord.stime.Min
                                            ,srecord.stime.Sec);
    
    //printf("\t选择通道：%2d %2d %2d %2d %2d %2d "     
    printfcom1("\r\n\t选择通道：%2d %2d,"     
                                            //,srecord.sselch.selchgroup
                                            ,srecord.sselch.selch1num
                                            ,srecord.sselch.selch2num
                                            //,srecord.sselch.selchgroupbak
                                            //,srecord.sselch.selch1numbak
                                            //,srecord.sselch.selch2numbak
                                            );    
    
    //printf("\r\n");
    for(u8 i = 0;i < 8; i++)
    {
        printfcom1("\r\n\t通道[%1d].频率,%d,标识,%x," ,i
                                            ,84000000/(srecord.spulse[i].period*9)
                                            ,srecord.spulse[i].flg);
    }
}
