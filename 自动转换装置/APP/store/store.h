#ifndef __SENSORPULSE_H
#define __SENSORPULSE_H	 
#include "sys.h" 
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

/* sensor pulse para types ------------------------------------------------------------*/

/** 
  * @brief     
  */ 
#define PULSE_NUN   200

typedef struct {
    uint16_t    higtime;                 //
    uint16_t    period;                  //
} sfrp;

typedef struct
{
    uint64_t    lstrisetime;            //last rise time
   
    uint64_t    risetime;               //rise time

    uint64_t    lstfalltime;            //last fall time
    
    uint64_t    falltime;               //fall time
    
    uint64_t    higtime;                // high level time
    
    uint64_t    lowtime;                // low level time

    uint32_t    period;                 //pulse peirod = risetime - lstrisetime (basetime 84Mhz)

    uint32_t    lstperiod;                 //pulse peirod = risetime - lstrisetime (basetime 84Mhz)
    
    
    int16_t     worktimes;
    
    int16_t      errtimes;    
    
    uint16_t    freq;                   //freq
    
    int16_t    timeout;

    uint8_t     ratio;                  //pulse ratio = (lstfalltime - lstrisetime) / period;

    uint8_t     higvol;                 //hight voltage value = (3.3/ 256)*higvol;

    uint8_t     lowvol;                 //low voltage   value = (3.3/ 256)*lowvol;
    
    uint8_t     flg;                    //pulse statuse:    //bit[7] = 1,ch pluse work;
                                                            //bit[6] = 1,ch pluse err;
                                                            //bit[3] = 1,rise edge;
//                                                          //bit[2] = 1,fall edge;
                                                            //bit[0] = 1,new pulse;
//    sfrp        frp[PULSE_NUN];
//    uint16_t    frpctr;
}stcchpulse;                                

typedef struct
{
    uint8_t     selchgroup;             //ch1\ch2 group0��ch3\ch4 group0; ch5\ch6 group0;ch7\ch8 group0; ����������4���������
    uint8_t     selch1num;              //select input ch number as ch1-out  
    uint8_t     selch2num;              //select input ch number as ch2-out  
    uint8_t     selchgroupbak;          //ch1\ch2 group0��ch3\ch4 group0; ch5\ch6 group0;ch7\ch8 group0; ����������4���������
    uint8_t     selch1numbak;           //select input ch number as ch1-out  
    uint8_t     selch2numbak;           //select input ch number as ch2-out  
    uint8_t     sumcheck;               //sumcheck;
}stcselch;                                


typedef enum
{ 
    CH_IN           = 7,    
    CH_ERR_LOSE     = 6,
    CH_ERR_FRQ      = 5,
    CH_OK           = 4,
    PLUSE_RISEEDGE  = 3,
    PLUSE_FALLEDGE  = 2, 
    PLUSE_NEW       = 0
    
}pluseflg_TypeDef;

extern  stcchpulse  gs_pulsech[8];        //ͨ��8
extern  stcselch    gs_selch;       //���ͨ��

typedef struct
{
    uint8_t     pdtype[12];             //product type
    uint8_t     pdid[12];               //product id num 
    uint8_t     version[12];            //vision
}stcproinfo;  

#define FRAM_PD_INFO_ADDR    0          /*��Ʒ��Ϣ*/
#define FRAM_SEL_CH_ADDR    (FRAM_PD_INFO_ADDR +  sizeof(stcproinfo) )       /*��Ʒ��Ϣ*/
    


#endif
