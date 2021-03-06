//#include "judgepulse.h"
#include "delay.h" 
#include "sensorpulse.h" 

#define     BASE_PLUSE_NUM          200
#define     PER_PERIOD              2       /*周期偏差百分比 50%    */
#define     ONE_KILM_PERIOD         659400  /*1 km/h 速度对应的周期 */
#define     NO_PLUSE_CIRCLE_NUM     16      /*16*10 个脉冲无信号，认为通道无信号故障*/


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

extern  uint64_t    GetSysBaseTick(void);

static  u32         l_allplusecnt = 0;          //总脉冲个数

void    printfflg(void)
{
    printfcom1("\r\n1:%02x\t2:%02x\t3:%02x\t4:%02x\t5:%02x\t6:%02x\t7:%02x\t8:%02x",
       (u8)gs_pulsech[0].flg,  
       (u8)gs_pulsech[1].flg,   
       (u8)gs_pulsech[2].flg,   
       (u8)gs_pulsech[3].flg,   
       (u8)gs_pulsech[4].flg,   
       (u8)gs_pulsech[5].flg,   
       (u8)gs_pulsech[6].flg,   
       (u8)gs_pulsech[7].flg);   

    printfcom1("\r\n频率 ");
    for(u8 i = 0 ;i < 8;i++)
    {
        printfcom1("\t%5d",84000000/(gs_pulsech[i].period*9));
    }
}


//通道有脉冲输入,返回1，无脉冲返回0
uint8  GetchPulseInitFlg(void)
{
    for(u8 i = 0;i < 8;i++)
    {
        if(gs_pulsech[i].flg & (0x01<< PLUSE_NEW))//有新脉冲
            return  1;
    }
    return 0;
}


//多通道对比
void    JudgemultichPulse(void)
{
    uint8           workchnum = 0;                  //正常工作通道
    static  u32     lst_allplusecnt = 0;            //上次计数的脉冲数
    
    if((lst_allplusecnt  > l_allplusecnt))          //控制判断次数
    {
        lst_allplusecnt = l_allplusecnt;
    }
    
    if(l_allplusecnt - lst_allplusecnt < 8)         //通道数
        return;

    lst_allplusecnt = l_allplusecnt;
    
    for(u8 i   = 0; i < 8; i++)
        if(     (gs_pulsech[i].flg & (0x01<<CH_IN))
            &&  (gs_pulsech[i].flg & (0x01<<CH_ERR_LOSE)) == 0  )    //通道有效
            workchnum++;
        
    if(workchnum < 2 )                              //通道数小于2，不比较
       return;
    
    int period;
    u8  sametimes;                                  //相同次数
    
    for( u8 i   = 0; i < 8; i++ )                   //确认正常工作通道数 
    {    
        if(     (gs_pulsech[i].flg & (0x01<<CH_IN))                 //通道有效
            &&  (gs_pulsech[i].flg & (0x01<<CH_ERR_LOSE)) == 0  )   //没有丢脉冲
        {      
            period = gs_pulsech[i].period;
            sametimes = 0;
            
            for(u8 j = 0; j < 8; j++ )
            {
                if(
                            (gs_pulsech[j].flg & (0x01<<CH_IN))             //通道有效
                        &&  (gs_pulsech[j].flg & (0x01<<CH_ERR_LOSE)) == 0  //没有丢脉冲
                  ) 
                {   
                    if(j != i)
                    {
                        u32 period1 = gs_pulsech[j].period;
                        if((abs(period - period1) < period/PER_PERIOD))     //判断是否有周期超限
                            sametimes++;
                    }
                }
            }
            
            if(sametimes < ((workchnum -1) / 2))                            //通道数相近的小于半数，认为故障
            {
                gs_pulsech[i].frqerrtimes++;
                
                //if( gs_pulsech[i].frqerrtimes > 400)                      //2圈
                if( gs_pulsech[i].frqerrtimes > 800)                        //圈半
                {
                    if((gs_pulsech[i].flg & (0x01 << CH_ERR_FRQ)) == 0){ 
                        printfcom1("\r\n通道频率故障CH%d：err %d",i,gs_pulsech[i].frqerrtimes);
                        gs_pulsech[i].flg |= (0x01 << CH_ERR_FRQ);      //频率故�
                        gs_runpara.flg |= (0x01<<PLUSE_STORE);          //状态保存
                    }
                    gs_pulsech[i].frqerrtimes = 100;
                }            
            }
            else
            {
                if(gs_pulsech[i].frqerrtimes)
                {
                    gs_pulsech[i].frqerrtimes--;
                }
                else
                {
                    gs_pulsech[i].frqerrtimes = 0;
                    
                    if((gs_pulsech[i].flg & (0x01 << CH_ERR_FRQ)) )
                    { 
                        printfcom1("\r\n通道频率故障CH%d清除",i);
                        gs_pulsech[i].flg   &= ~(0x01 << CH_ERR_FRQ);   //频率清除
                        gs_runpara.flg      |= (0x01<<PLUSE_STORE);     //状态保存
                    }
                }                
            }
        }
    }
}

static  u16 addspeedtimes = 0;
static  u16 subspeedtimes = 0;


//单通道处理
void    JudgeonechPulse(void)
{
    int         period;
    int         lstperiod;
    u8          flg;
    uint64_t    risetime; 
    uint64_t    systime;   

    uint8       workchnum = 0;                          //正常工作通道
    for(u8 i   = 0; i < 8; i++)
        if( (gs_pulsech[i].flg & (0x01<<CH_IN)))    //通道有效
            workchnum++;

    for( u8 i   = 0; i < 8; i++ )                       //循环遍历8个通道
    {
        period      = gs_pulsech[i].period;
        lstperiod   = gs_pulsech[i].lstperiod;
        flg         = gs_pulsech[i].flg;
        risetime    = gs_pulsech[i].risetime;
        systime     = GetSysBaseTick();
        
        if( flg &(0x01<<PLUSE_NEW) )                                //信号通道有新脉冲
        {                   
            gs_pulsech[i].flg &= ~(0x01<<PLUSE_NEW);                //清新脉冲标识
            l_allplusecnt++;                                        //有脉冲信号输入，计数器++
            
            if(period < ONE_KILM_PERIOD/1000)                                //速度大于1000km/h，认为错误退出
            {
                printfcom1("\r\n通道【%d】频率过快");
                break;
            }
//开机判断,如果开机选择通道故障，切换通道
////////////////////////////////////////////////////////////////////////////////////////////            
            if(gs_pulsech[i].worktimes > 5)
                gs_pulsech[i].flg |= (0x01<<FIVE_PLUSE); 
                        
            if(gs_pulsech[i].worktimes > 50)
  
            gs_pulsech[i].flg |= (0x01<<TEN_PLUSE);            

//同侧，异侧判断            
/////////////////////////////////////////////////////////////////////////////////////////////
//           
            static  u16 samesidetimes = 0;
            static  u16 difsidetimes  = 0;
            
            static  u8  sinsor1phase   = 0;     //1超前，2滞后
            static  u8  sinsor2phase   = 0;     //1超前，2滞后
            if(i == 1 || i == 5)
            {
                
               if(        gs_pulsech[0].flg & (0x01<<CH_OK)
                      &&  gs_pulsech[1].flg & (0x01<<CH_OK)
               )            //型号正常判断
               {
                  if(i == 1)
                  {
                       sinsor1phase = 0; 
                       if(gs_pulsech[0].flg & (0x01<<SIDE_FLG))
                       {
                           sinsor1phase = 1;
                       }else{
                           sinsor1phase = 2;
                       }                           
                   }
                }else{
                    sinsor1phase = 0; 
                }
                
              
                    
               if(        gs_pulsech[4].flg & (0x01<<CH_OK)
                      &&  gs_pulsech[5].flg & (0x01<<CH_OK)
               )            //型号正常判断
               {
                    if(i == 5)
                    {
                        sinsor2phase = 0;
                       if(gs_pulsech[4].flg & (0x01<<SIDE_FLG))
                       {
                           sinsor2phase = 1;
                       }else{
                           sinsor2phase = 2;
                       }                           
                   }
                }else{
                    sinsor2phase = 0;
                }
               

                if(     (sinsor1phase == 1 && sinsor2phase == 1)    //同时超前
                    ||  (sinsor1phase == 2 && sinsor2phase == 2)    //同时滞后
                   )
                {
                    samesidetimes++;    //同侧
                    
                    if(samesidetimes > 2000)
                    {
                        difsidetimes = 0;
                        if(gs_runpara.chmodel & (0x01<<0))//传感器安装与异侧
                        {
                           gs_runpara.chmodel &=  ~(0x01<<0);
                           
                           gs_runpara.crc16 = GetCrc16Check((u8 *)&gs_runpara,sizeof(gs_runpara)-2);

                           WriteFM24CL64(FRAM_RUN_PARA_ADDR,(u8 *)&gs_runpara,sizeof(gs_runpara));

                           BSP_Sel_GPIO_Init();

                           printfcom1("\r\n 主动设置为同侧");
                        }
                    }
                }
                
                if(     (sinsor1phase == 1 && sinsor2phase == 2)
                    ||  (sinsor1phase == 2 && sinsor2phase == 1)
                )
                {
                    difsidetimes++;
                
                    if(difsidetimes > 2000)
                    {
                        samesidetimes = 0;
                        if((gs_runpara.chmodel & (0x01<<0)) == 0)//传感器安装与同侧
                        {
                            gs_runpara.chmodel |=  (0x01<<0);
                           
                            gs_runpara.crc16 = GetCrc16Check((u8 *)&gs_runpara,sizeof(gs_runpara)-2);

                            WriteFM24CL64(FRAM_RUN_PARA_ADDR,(u8 *)&gs_runpara,sizeof(gs_runpara));
                            
                            BSP_Sel_GPIO_Init();

                            printfcom1("\r\n 主动设置为异侧");
                        }
                    }                    
                }
                
                //printfcom1("\r\n %d,%d,%d,%d",sinsor1phase,sinsor2phase,samesidetimes,difsidetimes);
            }
/////////////////////////////////////////////////////////////////////////////////////////////            

////////////////////////////////////////////////////////////////////////////////////////////
                //增加滑行 、空转 判断,加减速
                
                //if(gs_runpara.chmodel & (01<<0))//传感器安装与异侧
                //movenulflg = 1;
                if(gs_pulsech[i].flg & (0x01<<CH_IN))
                {
                    //movenulflg = 1;
                    gs_runpara.flg &= ~(0x01 <<SUB_SPEED_FLG);
                    if(gs_pulsech[i].period > gs_pulsech[i].lstperiod +200)
                    {
                        if(addspeedtimes)    
                            addspeedtimes--;
                        
                        subspeedtimes++;
                        
                        if(subspeedtimes > (workchnum +4)* 20)
                        {
                            printfcom1("\r\n 减速%d",subspeedtimes);
                            //movenulflg = 2;
                            gs_runpara.flg |= (0x01 <<SUB_SPEED_FLG);
                        }
                        
                    }else if(gs_pulsech[i].period + 200 < gs_pulsech[i].lstperiod )
                    {
                        addspeedtimes++;
                        
                        if(subspeedtimes)
                            subspeedtimes--; 
                        
                        if(addspeedtimes > (workchnum + 4) * 20)
                            printfcom1("\r\n  加速%d",addspeedtimes);
                    }else{
                        
                        if(subspeedtimes)
                            subspeedtimes--; 
                        
                        if(addspeedtimes)    
                            addspeedtimes--;
                    }
                      
                   // printfcom1("\r\n addspeedtimes %d，%d",addspeedtimes,subspeedtimes);
                }
                        ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



            //通道正常次数
            if(( abs(period - lstperiod) ) < period/PER_PERIOD)         //连续两周期偏差小于%30
            {       
                gs_pulsech[i].worktimes++; 
                
                if(gs_pulsech[i].worktimes > 150)                       
                {
                    if((gs_pulsech[i].flg & (0x01 << CH_IN)) == 0){
                        gs_pulsech[i].flg |= (0x01<<CH_IN);             //正常100个脉冲，认为有信号输入
                        printfcom1("\r\n通道[%d],有信号输入",i);
                    }                        
                }

                if(gs_pulsech[i].worktimes > 400)                       //正常工作1圈，清故障计数器
                //if(gs_pulsech[i].worktimes > 300)                       //正常工作1圈，清故障计数器
                {
                    gs_pulsech[i].errtimes = 0;                         //正常210个脉冲

                    if(gs_pulsech[i].flg &(0x01 << CH_ERR_LOSE) )
                    {
                        gs_pulsech[i].flg   &= ~(0x01 << CH_ERR_LOSE);  //|(0x01 << CH_ERR_FRQ)); //通道恢复
                        gs_runpara.flg      |= (0x01<<PLUSE_STORE);             //状态保存
                        printfcom1("\r\n丢脉冲通道%d，信号恢复",i);
                    }                

                    gs_pulsech[i].worktimes = 1000;                     
                }
            }
            else
            {
                //20161221 取消搞干扰措施
                
                //if(gs_pulsech[i].errtimes > 1)                           //不顺便清零，过滤干扰
                //if(gs_pulsech[i].errtimes > 0)                           //不顺便清零，过滤干扰
                //if((gs_runpara.flg &(0x01 <<SUB_SPEED_FLG)))    
                    gs_pulsech[i].worktimes = 0;                            //信号问题，重新计算正常工作次数
                 //printfcom1("\r\n %d周期偏差值%d，%d，错误次数：%d,总脉冲数 %d",i,period,lstperiod,gs_pulsech[i].errtimes,l_allplusecnt);
                
                //if((gs_pulsech[i].flg & (0x01<<CH_IN)))                 //信号稳定后，再进行丢脉冲
                {
                    if(l_allplusecnt > 2*200)                               //刚开车，不做丢脉冲故障运算
                        gs_pulsech[i].errtimes++;                           //丢脉冲故障次数                  
                }
            }
            
///////////////////////////////////////////////////////////////////////////////////////////////
            //丢脉冲判断
            //if(gs_pulsech[i].errtimes > 10)                               //出现10次，5圈，认为丢脉冲
            if(         gs_pulsech[i].errtimes > 20 
                //&&    (gs_pulsech[i].flg & (0x01<<CH_IN))                 //
              )                              //出现10次，5圈，认为丢脉冲
            {               
                if((gs_pulsech[i].flg   & (0x01 << CH_ERR_LOSE)) == 0)
                {
                    gs_pulsech[i].flg   |= (0x01 << CH_ERR_LOSE);       //通道丢脉冲
                    gs_runpara.flg      |= (0x01<<PLUSE_STORE);         //状态保存

                    if((gs_pulsech[i].flg &     (0x01 << CH_IN)) )
                        gs_pulsech[i].flg &=    ~(0x01 << CH_IN);          ////标记通道无信号  
                    
                    printfcom1("\r\n通道%d丢脉冲故障",i);
                }
            }
            
            if(gs_pulsech[i].worktimes > 10)                              //过滤晃动
                gs_pulsech[i].noinputtimes = 0;                           //有信号输入
        }
////////////////////////////////////////////////////////////////////////////////////////////////////  
        //没有新脉冲,判断是否长时间无信号        
        else                                                            
        {
            if(period > ONE_KILM_PERIOD)                             //周期超标//1km/h
                period = ONE_KILM_PERIOD/10;
            
//            if(period == 0)                                
//                period = ONE_KILM_PERIOD/100;                        //周期超标//100km/h
//            
//////////////////////////////////////////////////////////////////////////////////////////////////
//判断是否没有信号
            int     avgperiod = 0;
            
            for(u8 k=0 ; k<8; k++)
            {
                u8  tmp8 = 0;
                u32 periodtmp;
                
                periodtmp = gs_pulsech[k].period;
                if(periodtmp > ONE_KILM_PERIOD)
                     periodtmp = ONE_KILM_PERIOD/50;
                
                if(gs_pulsech[k].flg & (0x01 << CH_IN))
                {
                    if(avgperiod)
                    {
                        avgperiod += periodtmp;
                        avgperiod  /= 2;
                    }
                    else
                    {
                        avgperiod = periodtmp;
                    }
                }
//                
//                //if((systime - gs_pulsech[i].risetime) >  100 * periodtmp)
//                if((systime - gs_pulsech[k].risetime) >  100 * avgperiod)
//                {
//                    tmp8++;
//                }
//        
//                if(tmp8 == 8)
//                {
//                    for(u8 j = 0;j < 8;j++ )
//                    {                        
//                       if((gs_pulsech[j].flg & (0x01 << CH_IN)))
//                        {
//                            gs_pulsech[j].flg &= ~(0x01 << CH_IN);  //标记通道无信号  
//                            printfcom1("\r\n 通道>【 %d 】无输入 %x",j, (u8)gs_pulsech[j].flg);
//                        }   
//                    }
//                    
//                    Initchexcptflg();//清通道参数，除flg外
//                }                                                
            }
            
            if(avgperiod > ONE_KILM_PERIOD)                     //1km/h 
                avgperiod = ONE_KILM_PERIOD/50;                 //按平均50km/h计算。
                
            if(     (systime - risetime  >  ONE_KILM_PERIOD  )       //最低时间0.02s
                //&&  (systime - risetime  >  100*period ))   //信号半圈无信号
            &&  (systime - risetime  >  10*avgperiod ))   //信号半圈无信号
             {
                 if(systime < gs_pulsech[i].dealtime)
                     gs_pulsech[i].dealtime = systime;
                 
                if(     systime - gs_pulsech[i].dealtime > ONE_KILM_PERIOD
                   //||  (systime - gs_pulsech[i].dealtime  >  100*period ) 
                &&  (systime - gs_pulsech[i].dealtime  >  10*avgperiod ) 
                  )  //最大时间0.15s 执行一次
                {
                    gs_pulsech[i].dealtime  = systime;

                    gs_pulsech[i].noinputtimes++;

                    if(gs_pulsech[i].noinputtimes > NO_PLUSE_CIRCLE_NUM/2 )             //0.5圈
                    {   
                        if((gs_pulsech[i].flg & (0x01 << CH_IN)))
                        {
                            gs_pulsech[i].flg &= ~(0x01 << CH_IN);  //标记通道无信号 
                            gs_runpara.flg      |= (0x01 << PLUSE_STORE);           //状态保存
                            
                            printfcom1("\r\n 通道【%d】无输入 %x",i, gs_pulsech[i].flg);
                        }                        
                        
                        u8 tmp = 0;
                        for(u8 j = 0;j < 8;j++)                     //停车补充
                        {
                            if(gs_pulsech[j].noinputtimes  > 1)     //大于1
                                tmp++;  
                        }
                        
                        if(tmp == 8)
                        {
                            if(gs_runpara.flg & (0x01<<RUN_FLG))
                            {
                                gs_runpara.flg     &= ~(0x01<<RUN_FLG);             //机车停车运行
                                printfcom1("\r\n机车停车");
                            }
                            
                            for(u8 j = 0;j < 8;j++ )
                            {                        
                               if((gs_pulsech[j].flg & (0x01 << CH_IN)))
                                {
                                    gs_runpara.flg      |= (0x01 << PLUSE_STORE);           //状态保存
                                    gs_pulsech[j].flg   &= ~(0x01 << CH_IN);  //标记通道无信号  
                                    printfcom1("\r\n 通道【 %d 】无输入 %x",j, gs_pulsech[j].flg);
                                }   
                            }
                            
                            static  uint64_t  no_pluse_time = 0;
                            if(       l_allplusecnt > 200*100                   //至少要有10/8圈或者
                                   || (systime - no_pluse_time) > 120*1000*1000   // (9*2 / 12) 5s清零一次
                                )
                            {
                                no_pluse_time = systime;
                                printfcom1("\r\n 无信号%d！！%lld",l_allplusecnt,no_pluse_time);

                                l_allplusecnt = 0;
                                Initchexcptflg();//清通道参数，除flg外
                                addspeedtimes = 0;
                                subspeedtimes = 0;
                                gs_runpara.flg &= ~(0x01<<RUN_FLG);
                                //printfcom1("\r\n 无信号！！");
                            }
                        }                        
                    }
////////////////////////////////////////////////////////////////////////////////////////////////////  
                  
                     //无信号故障判断
                    if(workchnum > 0)
                    {
                        ///////////////////////////////////////////////////////
                       ///if( gs_pulsech[i].noinputtimes > NO_PLUSE_CIRCLE_NUM)                           //1圈，或1.5s
                    
                        u8 movenulflg = 1;          //空转。滑行标记
                        if(gs_runpara.flg &(0x01 <<SUB_SPEED_FLG))
                            movenulflg = 3;         //空转、滑行时，3倍故障判断时间
                
                        if( gs_pulsech[i].noinputtimes > NO_PLUSE_CIRCLE_NUM * movenulflg  )                           //1圈，或1.5s
                        {
                            if((gs_pulsech[i].flg   & (0x01 << CH_ERR_LOSE)) == 0)
                            {
                                gs_pulsech[i].flg   |= (0x01 << CH_ERR_LOSE);           //通道丢脉冲
                                gs_runpara.flg      |= (0x01 << PLUSE_STORE);           //状态保存
                                
                                u16 flgtmp;
                                flgtmp = gs_pulsech[i].flg;
                                memset(&gs_pulsech[i],0,sizeof(gs_pulsech[i]));         //清数据
                                gs_pulsech[i].flg = flgtmp; 
                                
                                printfcom1("\r\nI通道%d无信号故障",i);
                            }
                        }                         
                    }
                }
            }
        }
    }
}


void    JudgePulseok(void)
{
    //信号正常判定
    for( u8 i   = 0; i < 8; i++ )   //确认正常工作通道数     
    {            
        if(     ( gs_pulsech[i].flg & (0x01<<CH_IN)) &&       //通道有效，且没故障
                ((gs_pulsech[i].flg & (0x01<<CH_ERR_LOSE)) == 0) &&
                ((gs_pulsech[i].flg & (0x01<<CH_ERR_FRQ))  ==0)   
           )
        {   
            gs_runpara.flg     |= (0x01<<RUN_FLG);             //机车运行
            gs_pulsech[i].flg  |= (0x01<<CH_OK);               //通道正常
        }else{
            gs_pulsech[i].flg  &= ~(0x01<<CH_OK);
        }
    }  
}


void    JudgePulse(void)
{
    JudgeonechPulse();

    static  uint64_t    dealtime = 0;
    if(GetSysBaseTick() < dealtime + 1000*100)   //10ms判断一次9/84mhz
        return;
    dealtime = GetSysBaseTick();
    
    JudgePulseok();  
    
    JudgemultichPulse();        //多通道处理
}



stcselch    gs_selch;       //输出通到

extern u8  g_ch1outnum ;    //默认为0+1通道
extern u8  g_ch2outnum ;    //默认为1+1通道

extern void BSP_PulseSel( CPU_INT08U ch1, CPU_INT08U ch2 );

void    AutoChangeCh(void)
{    
    uint8   sel = 0;
    uint8   tmp8;
    uint8   lstbitsta = Bit_SET;
    u8      group = 0;
    u8      ch1,ch2;
 
    ch1 = gs_selch.selch1num;
    ch2 = gs_selch.selch2num; 

    
/////////////////////////////////////////////////////////
    //通道正常，选择的信号通道正常 
    //增加有同组输出，立即切换功能
    
//    if(     (gs_pulsech[ch1].flg & (0x01 << CH_OK)) &&
//            (gs_pulsech[ch2].flg & (0x01 << CH_OK)) &&
//            (ch1 != ch2)
//    ){
//    更改切换条件，通道不故障不切换。避免刚运行，未选中通道先确认合格而切换通道      
    if(     ((gs_pulsech[ch1].flg & (0x01 << CH_ERR_LOSE))  == 0) &&    //无丢脉冲故障
            ((gs_pulsech[ch1].flg & (0x01 << CH_ERR_FRQ))   == 0) &&    //无频率故障
            ((gs_pulsech[ch2].flg & (0x01 << CH_ERR_LOSE))  == 0) &&    //无丢脉冲故障
            ((gs_pulsech[ch2].flg & (0x01 << CH_ERR_FRQ))   == 0) &&    //无频率故障
            (ch1 != ch2)
      ){
        if(     ((ch1%2==0) && ((ch1+1)==ch2))
            ||  ((ch1%2==1) && ((ch1-1)==ch2))
           ) 
        {    //同组输出，不找备用通道
        //  if( abs(ch1-ch2) == 1) {    //同组输出，不找备用通道
            //printfcom1("ch1%2==0) %d，%d",ch1,ch2);
            if(    ((gs_pulsech[ch1].flg & (0x01 << CH_IN)) && (gs_pulsech[ch2].flg & (0x01 << CH_IN)))   //通道输入正常
                ||  (gs_pulsech[ch1].period > ONE_KILM_PERIOD /10 &&  gs_pulsech[ch2].period > ONE_KILM_PERIOD /10) //速度小于10km/h
                )   
            {
                return;
            }else{
                //刚开机时，选择通到无信号，输出，避免拔插头情况
    ////////////////////////////////////////////////////////////////////////////////////
                
                if(
                    (gs_pulsech[ch1].flg & (0x01 << FIVE_PLUSE) ) &&
                    (gs_pulsech[ch2].flg & (0x01 << FIVE_PLUSE) )       
                )
                {return;}//有信号输入，不处理
                
                //无信号输入，找其他通道是否有数据，
                group = 0;
                for(u8 i = 0;i<4;i++,group++ )  
                {
                    ch1 = (( group % 4)*2)%8;
                    ch2 = ( ch1 + 1 )% 8;
                    if(                         //其他通道有数据输入
                        (gs_pulsech[ch1].flg & (0x01 << TEN_PLUSE)) &&
                        (gs_pulsech[ch2].flg & (0x01 << TEN_PLUSE))
                       )
                     {
                        gs_selch.selchgroup     = group % 4;
                        gs_selch.selch1num      = gs_selch.selchgroup * 2; 
                        gs_selch.selch2num      = gs_selch.selch1num  + 1;

                        if(gs_runpara.chmodel & (01<<0))//传感器安装与异侧
                        {
                            if(gs_selch.selchgroup > 1)//将5、6、7、8通道反相
                            {
                                gs_selch.selch1num = gs_selch.selch2num;      //通道反向 
                                gs_selch.selch2num = gs_selch.selch1num - 1;  //通道反向       
                            }
                            
                            printfcom1("\r\n 通道反向");
                        }
                        printfcom1("\r\n开车切换 gs_selch.selchgroup %d，%d，%d",gs_selch.selchgroup,gs_selch.selch1num,gs_selch.selch2num);

                        gs_selch.crc16 = GetCrc16Check((u8 *)&gs_selch,sizeof(gs_selch)-2 );            
                        WriteFM24CL64(FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));//存通道1 

                        BSP_PulseSel( gs_selch.selch1num, gs_selch.selch2num );//选择信号输出
                        gs_runpara.flg      |= (0x01 << PLUSE_STORE);    


                        //状态保存                        
                    }
                }                               
            }
        }
    }
      
////////////////////////////////////////////////////////////////////////////////////////   
//    printfcom1("\r\ngroup %d，%d，%x，%x",gs_selch.selch1num,gs_selch.selch2num,
//                            gs_pulsech[gs_selch.selch1num].flg,gs_pulsech[gs_selch.selch2num].flg);
//    
    static  uint64_t   changetick = 0;
      
    if(GetSysBaseTick() - changetick < 1000*1000*8*2)  //s 切换内，不进行切换
        return;
    changetick = GetSysBaseTick();
    
    //分组切换  
    //printfcom1("ppp");    
    group = gs_selch.selchgroup;
    for(u8 i = 0;i<4;i++,group++ )  
    {
        ch1 = (( group % 4)*2)%8;
        ch2 = ( ch1 + 1 )% 8;
        if(
            (gs_pulsech[ch1].flg & (0x01 << CH_OK)) &&
            (gs_pulsech[ch2].flg & (0x01 << CH_OK))
           )
        {
            printfcom1("\r\ngroup %d ch1 %d ch2 %d ",group,ch1,ch2);
            gs_selch.selchgroup     = group % 4;
            gs_selch.selchgroupbak  = 5;
            gs_selch.selch1num  = gs_selch.selchgroup * 2; 
            gs_selch.selch2num  = gs_selch.selch1num  + 1;
            
            if(gs_runpara.chmodel & (01<<0))//传感器安装与异侧
            {
                if(gs_selch.selchgroup > 1)//将5、6、7、8通道反相
                {
                    gs_selch.selch1num = gs_selch.selch2num;      //通道反向 
                    gs_selch.selch2num = gs_selch.selch1num - 1;  //通道反向       
                }
                
                printfcom1("\r\n 通道反向");
            }
            printfcom1("\r\n 121 gs_selch.selchgroup %d，%d，%d",gs_selch.selchgroup,gs_selch.selch1num,gs_selch.selch2num);
            
            gs_selch.crc16 = GetCrc16Check((u8 *)&gs_selch,sizeof(gs_selch)-2 );            
            WriteFM24CL64(FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));//存通道1 
            
            BSP_PulseSel( gs_selch.selch1num, gs_selch.selch2num );//选择信号输出
            gs_runpara.flg      |= (0x01 << PLUSE_STORE);           //状态保存

            changetick = GetSysBaseTick();
            //printfcom1("\r\n 131 gs_selch.selchgroup %d，%d，%d",gs_selch.selchgroup,gs_selch.selch1num,gs_selch.selch2num);
            printfflg();    
            return;
        }
    }

    
    
 //////////////////////////////////////////////////////////////////////
 //单信号切换    
    u8  flg = 0;
    u8  curch1,curch2;
    curch1 = gs_selch.selch1num;
    curch2 = gs_selch.selch2num;
    
//    printfcom1("\r\ncurch1 %d,%d",gs_selch.selch1num,gs_selch.selch2num);
//    static  uint16_t singchangetime = 0;
//    if(GetSysBaseTick() - singchangetime <  1000*1000*8*200)
//            return;
//    singchangetime = GetSysBaseTick();
//    
    for(u8 i = 0; i<8; i++ )
    {
        ch1 = i%8;
        
        if(gs_pulsech[ch1].flg & (0x01 << CH_OK)){ //信号有效
        
            gs_selch.selch1num  = ch1;              //找到第一个有效通道
            flg = 1;
            for(u8 j = 0; j < 8; j++ ) {
                ch2 = (ch1+1+j)%8;
                
                if(gs_pulsech[ch2].flg & (0x01 << CH_OK)){//信号有效
                    gs_selch.selch2num  = ch2;
                    flg = 2;
                    break;
                }
            }
        }
        
        if(flg == 2)
            break;
    }

    if(flg)                 //找到有效通道，进行通道切换 
    {
        if(flg == 1)
        {
            gs_selch.selch2num = gs_selch.selch1num;
        }
        
       //printfcom1("\r\ncurch1 %d,%d,%d,%d",gs_selch.selch1num,gs_selch.selch2num,curch1,curch2);
        
        if(     gs_selch.selch1num == curch1
            &&  gs_selch.selch2num == curch2
          )
            return;        //通道未切换，不处理
        
        //printfcom1("\r\ncurch1 %d,%d",gs_selch.selch1num,gs_selch.selch2num);

        gs_selch.selchgroup     = 5;
        gs_selch.selchgroupbak  = 5;

//        gs_selch.sumcheck   = gs_selch.selchgroup  
//                            + gs_selch.selch1num  
//                            + gs_selch.selch2num;  
        gs_selch.crc16 = GetCrc16Check((u8 *)&gs_selch,sizeof(gs_selch)-2 );

        WriteFM24CL64(FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));//存通道1 
        ReadFM24CL64( FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));//读通道数据
        
        BSP_PulseSel( gs_selch.selch1num, gs_selch.selch2num );//选择信号输出
        gs_runpara.flg      |= (0x01 << PLUSE_STORE);           //状态保存

        changetick = GetSysBaseTick();
        printfcom1("\r\n 单通道切换 selch1num %d，%d，",gs_selch.selch1num,gs_selch.selch2num);
        printfflg();    
    }
}
