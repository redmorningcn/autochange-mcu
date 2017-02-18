#ifndef __SENSORPULSE_H
#define __SENSORPULSE_H	 
#include "sys.h" 
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

/* sensor pulse para types ------------------------------------------------------------*/

/** 
  * @brief     
  */ 
#define PULSE_NUN   200
#include "DS3231.h"

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
    
    uint64_t    dealtime;               // 信号处理 time
    
    uint64_t    lowtime;                // low level time

    uint32_t    period;                 //pulse peirod = risetime - lstrisetime (basetime 84Mhz)

    uint32_t    lstperiod;                 //pulse peirod = risetime - lstrisetime (basetime 84Mhz)
    
    int16_t     worktimes;
    
    int16_t     errtimes;    
    
//    int16_t     errtimes;    

    int16_t     frqerrtimes;   
    
    int16_t     noinputtimes;        
    
    uint16_t    freq;                   //freq
    
    int16_t     timeout;

    uint8_t     ratio;                  //pulse ratio = (lstfalltime - lstrisetime) / period;

    uint8_t     higvol;                 //hight voltage value = (3.3/ 256)*higvol;

    uint8_t     lowvol;                 //low voltage   value = (3.3/ 256)*lowvol;
    
    uint16_t     flg;                    //pulse statuse:    //bit[7] = 1,ch pluse work;
                                                            //bit[6] = 1,ch pluse err;
                                                            //bit[3] = 1,rise edge;
//                                                          //bit[2] = 1,fall edge;
                                                            //bit[0] = 1,new pulse;
//    sfrp        frp[PULSE_NUN];
//    uint16_t    frpctr;
}stcchpulse;                                

typedef struct
{
    uint8_t     selchgroup;             //ch1\ch2 group0；ch3\ch4 group0; ch5\ch6 group0;ch7\ch8 group0; 如果分组大于4，分组错误
    uint8_t     selch1num;              //select input ch number as ch1-out  
    uint8_t     selch2num;              //select input ch number as ch2-out  
    uint8_t     selchgroupbak;          //ch1\ch2 group0；ch3\ch4 group0; ch5\ch6 group0;ch7\ch8 group0; 如果分组大于4，分组错误
    uint8_t     selch1numbak;           //select input ch number as ch1-out  
    uint8_t     selch2numbak;           //select input ch number as ch2-out  
    uint16_t    crc16;               //sumcheck;
}stcselch;                                
typedef enum
{ 
    
    SIDE_FLG            = 11,/* 相位关系，0超前，1滞后，只对0、1   4、5通道有效 */    
    TEN_PLUSE           = 10, 
    FIVE_PLUSE          = 9, 
    LONG_TIME_NO_SING   = 8,    
    CH_IN               = 7,    
    CH_ERR_LOSE         = 6,
    CH_ERR_FRQ          = 5,
    CH_OK               = 4,
    PLUSE_STORE         = 3, /* 存通道信息 */
    PLUSE_FRQ_E         = 2, /* 故障处理用 */
    PLUSE_LOSE_E        = 1, /* 故障处理用 */
    PLUSE_NEW           = 0  /* 新脉冲     */

}pluseflg_TypeDef;

typedef struct
{
    uint8_t     pdtype[12];             //product type
    uint8_t     pdid[12];               //product id num 
    uint8_t     version[12];            //vision
    uint16_t    crc16;
}stcproinfo;  

typedef struct
{
    uint32      writerecnum;            //reccord num
    uint32      readnum;                //上次读取的流水记录好
    uint16      runmodel;               //yun运行模式
    uint16      chmodel;                //通道模式，00 同侧，01 异侧
    uint16      flg;
    uint16      crc16;                  //crc16 校验
}stcrunpara;  


typedef struct
{
    uint32      period;                 //
    uint8_t     ratio;                  //pulse ratio = (lstfalltime - lstrisetime) / period;
    uint8_t     higvol;                 //hight voltage value = (3.3/ 256)*higvol;
    uint8_t     lowvol;                 //low voltage   value = (3.3/ 256)*lowvol;
    uint8_t     flg;                    //pulse statuse:    //bit[7] = 1,ch pluse work;
                                                            //bit[6] = 1,ch pluse err;
                                                            //bit[3] = 1,rise edge;
    //                                                      //bit[2] = 1,fall edge;
                                                            //bit[0] = 1,new pulse;
}stcpulse;  

extern  stcchpulse      gs_pulsech[8];      //通道8
extern  stcselch        gs_selch;           //输出通到
extern  stcrunpara      gs_runpara;         //输出通到
extern  stcpulse        gs_storepluse[8];   //存储信息  




typedef enum
{ 
    TIME_FLG        = 1,  /* 定时存储     */
    STORE_FLG       = 1,  /* 主动存储     */
    START_FLG       = 0,  /* 开机数据     */
}Data_TypeDef;

typedef struct
{
    stcTime         stime;              //时间            8
    stcselch        sselch;             //通道选择状态    8
    stcpulse        spulse[8];          //脉冲信息        8*8       
    uint8           bak8;               //                1  
    uint8           type;               //数据类型        1  
    uint16          crc16;              //crc16 校验      2
}stcrecord;  



//dizhi
#define FRAM_PD_INFO_ADDR    0          /*产品信息*/
#define FRAM_SEL_CH_ADDR    (FRAM_PD_INFO_ADDR +  sizeof(stcproinfo) )  /*通道选择信息*/
#define FRAM_RUN_PARA_ADDR  (FRAM_SEL_CH_ADDR +  sizeof(stcselch) )     /*运行参数    */

#define FRAM_CUR_CH_INFO    1000        /*当前通道状态*/



#endif
