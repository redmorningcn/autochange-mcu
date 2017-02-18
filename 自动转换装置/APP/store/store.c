#include "sensorpulse.h"
#include "delay.h" 
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
    INTX_DISABLE();             //��ȫ���ж�

    for(u8 i = 0 ; i < 8 ;i++){
        u32 addr;
        addr = sizeof(stcchpulse) * i + FRAM_CUR_CH_INFO;
       
        WriteFM24CL64(addr,(u8 *)&gs_pulsech[i],sizeof(stcchpulse));
    }
    INTX_ENABLE();             //��ȫ���ж�
}


void    StoreProInfo_Fram(void)         //���Ʒ��Ϣ
{
    INTX_DISABLE();                     //��ȫ���ж�

    //for(u8 i = 0 ; i < 8 ;i++){
        u32 addr;
        addr = FRAM_PD_INFO_ADDR;
        //strcpy((u8 *)gs_proinfo.pdid,"1612001");
        WriteFM24CL64(addr,(u8 *)&gs_proinfo,sizeof(stcproinfo));
    //}
    INTX_ENABLE();             //��ȫ���ж�
}
void    ReadProInfo_Fram(void)         //���Ʒ��Ϣ
{
    INTX_DISABLE();                     //��ȫ���ж�

    //for(u8 i = 0 ; i < 8 ;i++){
        u32 addr;
        addr = FRAM_PD_INFO_ADDR;
        //strcpy((u8 *)gs_proinfo.pdid,"1612001");
        ReadFM24CL64(addr,(u8 *)&gs_proinfo,sizeof(stcproinfo));
    //}
    printfcom1("\r\n ");
    printfcom1("\r\n ��Ʒ��ţ�%s",gs_proinfo.pdid);
    INTX_ENABLE();             //��ȫ���ж�
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
    
    INTX_DISABLE();             //��ȫ���ж�

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
    INTX_ENABLE();             //��ȫ���ж�

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
    
//    INTX_DISABLE();             //��ȫ���ж�

    ReadTime( (stcTime *)&srecord.stime );         //ȡʱ����Ϣ
    PrintfTime();

    for(u8 i = 0;i < 8 ;i++){           //ȡ��¼��Ϣ
        
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

    uint32  addr  = sizeof(stcrecord)*((gs_runpara.writerecnum++) % MAX_REC_NUM );//ȡ�洢��ַ
    MX25L1602_WR(addr,(u8 *)&srecord,sizeof(stcrecord) );
    
    uint32  recnum = gs_runpara.writerecnum - 1;
    addr  = sizeof(stcrecord)*((recnum) % MAX_REC_NUM );//ȡ�洢��ַ
    
    MX25L1602_RD(addr,sizeof(stcrecord),(u8 *)&srecord );
    
    gs_runpara.crc16 = GetCrc16Check((u8 *)&gs_runpara,sizeof(gs_runpara)-2);
    
    WriteFM24CL64(FRAM_RUN_PARA_ADDR,(u8 *)&gs_runpara,sizeof(gs_runpara));
    
//ReadRunPara();
//INTX_ENABLE();             //��ȫ���ж�
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

        printf("\r\n ���в�������");
    }
    
   //gs_runpara.writerecnum = 0;
    printf("\r\n���в�����%d��%d��%d��%d�� %x�� "
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
        
    uint32  addr  = sizeof(stcrecord)*((recnum) % MAX_REC_NUM );//ȡ�洢��ַ
    
    MX25L1602_RD(addr,sizeof(stcrecord),(u8 *)&srecord );
    
	printfcom1("\r\n%u-%u-%u %2u:%2u:%2u,"
                                            ,srecord.stime.Year
                                            ,srecord.stime.Month
                                            ,srecord.stime.Date
                                            ,srecord.stime.Hour
                                            ,srecord.stime.Min
                                            ,srecord.stime.Sec);
    
    //printf("\tѡ��ͨ����%2d %2d %2d %2d %2d %2d "     
    printfcom1("\r\n\tѡ��ͨ����%2d %2d,"     
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
        printfcom1("\r\n\tͨ��[%1d].Ƶ��,%d,��ʶ,%x," ,i
                                            ,84000000/(srecord.spulse[i].period*9)
                                            ,srecord.spulse[i].flg);
    }
}
