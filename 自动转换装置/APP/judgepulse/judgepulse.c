//#include "judgepulse.h"
#include "delay.h" 
#include "sensorpulse.h" 

#define     BASE_PLUSE_NUM          200
#define     PER_PERIOD              2       /*ÖÜÆÚÆ«²î°Ù·Ö±È 50%    */
#define     ONE_KILM_PERIOD         659400  /*1 km/h ËÙ¶È¶ÔÓ¦µÄÖÜÆÚ */
#define     NO_PLUSE_CIRCLE_NUM     16      /*16*10 ¸öÂö³åÎÞÐÅºÅ£¬ÈÏÎªÍ¨µÀÎÞÐÅºÅ¹ÊÕÏ*/


//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌÐòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßÐí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEK STM32F407¿ª·¢°å
//°´¼üÊäÈëÇý¶¯´úÂë	   
//ÕýµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//´´½¨ÈÕÆÚ:2014/5/3
//°æ±¾£ºV1.0
//°æÈ¨ËùÓÐ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖÝÊÐÐÇÒíµç×Ó¿Æ¼¼ÓÐÏÞ¹«Ë¾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

extern  uint64_t    GetSysBaseTick(void);

static  u32         l_allplusecnt = 0;          //×ÜÂö³å¸öÊý

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

    printfcom1("\r\nÆµÂÊ ");
    for(u8 i = 0 ;i < 8;i++)
    {
        printfcom1("\t%5d",84000000/(gs_pulsech[i].period*9));
    }
}


//Í¨µÀÓÐÂö³åÊäÈë,·µ»Ø1£¬ÎÞÂö³å·µ»Ø0
uint8  GetchPulseInitFlg(void)
{
    for(u8 i = 0;i < 8;i++)
    {
        if(gs_pulsech[i].flg & (0x01<< PLUSE_NEW))//ÓÐÐÂÂö³å
            return  1;
    }
    return 0;
}


//¶àÍ¨µÀ¶Ô±È
void    JudgemultichPulse(void)
{
    uint8           workchnum = 0;                  //Õý³£¹¤×÷Í¨µÀ
    static  u32     lst_allplusecnt = 0;            //ÉÏ´Î¼ÆÊýµÄÂö³åÊý
    
    if((lst_allplusecnt  > l_allplusecnt))          //¿ØÖÆÅÐ¶Ï´ÎÊý
    {
        lst_allplusecnt = l_allplusecnt;
    }
    
    if(l_allplusecnt - lst_allplusecnt < 8)         //Í¨µÀÊý
        return;

    lst_allplusecnt = l_allplusecnt;
    
    for(u8 i   = 0; i < 8; i++)
        if(     (gs_pulsech[i].flg & (0x01<<CH_IN))
            &&  (gs_pulsech[i].flg & (0x01<<CH_ERR_LOSE)) == 0  )    //Í¨µÀÓÐÐ§
            workchnum++;
        
    if(workchnum < 2 )                              //Í¨µÀÊýÐ¡ÓÚ2£¬²»±È½Ï
       return;
    
    int period;
    u8  sametimes;                                  //ÏàÍ¬´ÎÊý
    
    for( u8 i   = 0; i < 8; i++ )                   //È·ÈÏÕý³£¹¤×÷Í¨µÀÊý 
    {    
        if(     (gs_pulsech[i].flg & (0x01<<CH_IN))                 //Í¨µÀÓÐÐ§
            &&  (gs_pulsech[i].flg & (0x01<<CH_ERR_LOSE)) == 0  )   //Ã»ÓÐ¶ªÂö³å
        {      
            period = gs_pulsech[i].period;
            sametimes = 0;
            
            for(u8 j = 0; j < 8; j++ )
            {
                if(
                            (gs_pulsech[j].flg & (0x01<<CH_IN))             //Í¨µÀÓÐÐ§
                        &&  (gs_pulsech[j].flg & (0x01<<CH_ERR_LOSE)) == 0  //Ã»ÓÐ¶ªÂö³å
                  ) 
                {   
                    if(j != i)
                    {
                        u32 period1 = gs_pulsech[j].period;
                        if((abs(period - period1) < period/PER_PERIOD))     //ÅÐ¶ÏÊÇ·ñÓÐÖÜÆÚ³¬ÏÞ
                            sametimes++;
                    }
                }
            }
            
            if(sametimes < ((workchnum -1) / 2))                            //Í¨µÀÊýÏà½üµÄÐ¡ÓÚ°ëÊý£¬ÈÏÎª¹ÊÕÏ
            {
                gs_pulsech[i].frqerrtimes++;
                
                //if( gs_pulsech[i].frqerrtimes > 400)                      //2È¦
                if( gs_pulsech[i].frqerrtimes > 800)                        //È¦°ë
                {
                    if((gs_pulsech[i].flg & (0x01 << CH_ERR_FRQ)) == 0){ 
                        printfcom1("\r\nÍ¨µÀÆµÂÊ¹ÊÕÏCH%d£ºerr %d",i,gs_pulsech[i].frqerrtimes);
                        gs_pulsech[i].flg |= (0x01 << CH_ERR_FRQ);      //ÆµÂÊ¹ÊÕ
                        gs_runpara.flg |= (0x01<<PLUSE_STORE);          //×´Ì¬±£´æ
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
                        printfcom1("\r\nÍ¨µÀÆµÂÊ¹ÊÕÏCH%dÇå³ý",i);
                        gs_pulsech[i].flg   &= ~(0x01 << CH_ERR_FRQ);   //ÆµÂÊÇå³ý
                        gs_runpara.flg      |= (0x01<<PLUSE_STORE);     //×´Ì¬±£´æ
                    }
                }                
            }
        }
    }
}

static  u16 addspeedtimes = 0;
static  u16 subspeedtimes = 0;


//µ¥Í¨µÀ´¦Àí
void    JudgeonechPulse(void)
{
    int         period;
    int         lstperiod;
    u8          flg;
    uint64_t    risetime; 
    uint64_t    systime;   

    uint8       workchnum = 0;                          //Õý³£¹¤×÷Í¨µÀ
    for(u8 i   = 0; i < 8; i++)
        if( (gs_pulsech[i].flg & (0x01<<CH_IN)))    //Í¨µÀÓÐÐ§
            workchnum++;

    for( u8 i   = 0; i < 8; i++ )                       //Ñ­»·±éÀú8¸öÍ¨µÀ
    {
        period      = gs_pulsech[i].period;
        lstperiod   = gs_pulsech[i].lstperiod;
        flg         = gs_pulsech[i].flg;
        risetime    = gs_pulsech[i].risetime;
        systime     = GetSysBaseTick();
        
        if( flg &(0x01<<PLUSE_NEW) )                                //ÐÅºÅÍ¨µÀÓÐÐÂÂö³å
        {                   
            gs_pulsech[i].flg &= ~(0x01<<PLUSE_NEW);                //ÇåÐÂÂö³å±êÊ¶
            l_allplusecnt++;                                        //ÓÐÂö³åÐÅºÅÊäÈë£¬¼ÆÊýÆ÷++
            
            if(period < ONE_KILM_PERIOD/1000)                                //ËÙ¶È´óÓÚ1000km/h£¬ÈÏÎª´íÎóÍË³ö
            {
                printfcom1("\r\nÍ¨µÀ¡¾%d¡¿ÆµÂÊ¹ý¿ì");
                break;
            }
//¿ª»úÅÐ¶Ï,Èç¹û¿ª»úÑ¡ÔñÍ¨µÀ¹ÊÕÏ£¬ÇÐ»»Í¨µÀ
////////////////////////////////////////////////////////////////////////////////////////////            
            if(gs_pulsech[i].worktimes > 5)
                gs_pulsech[i].flg |= (0x01<<FIVE_PLUSE); 
                        
            if(gs_pulsech[i].worktimes > 50)
  
            gs_pulsech[i].flg |= (0x01<<TEN_PLUSE);            

//Í¬²à£¬Òì²àÅÐ¶Ï            
/////////////////////////////////////////////////////////////////////////////////////////////
//           
            static  u16 samesidetimes = 0;
            static  u16 difsidetimes  = 0;
            
            static  u8  sinsor1phase   = 0;     //1³¬Ç°£¬2ÖÍºó
            static  u8  sinsor2phase   = 0;     //1³¬Ç°£¬2ÖÍºó
            if(i == 1 || i == 5)
            {
                
               if(        gs_pulsech[0].flg & (0x01<<CH_OK)
                      &&  gs_pulsech[1].flg & (0x01<<CH_OK)
               )            //ÐÍºÅÕý³£ÅÐ¶Ï
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
               )            //ÐÍºÅÕý³£ÅÐ¶Ï
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
               

                if(     (sinsor1phase == 1 && sinsor2phase == 1)    //Í¬Ê±³¬Ç°
                    ||  (sinsor1phase == 2 && sinsor2phase == 2)    //Í¬Ê±ÖÍºó
                   )
                {
                    samesidetimes++;    //Í¬²à
                    
                    if(samesidetimes > 2000)
                    {
                        difsidetimes = 0;
                        if(gs_runpara.chmodel & (0x01<<0))//´«¸ÐÆ÷°²×°ÓëÒì²à
                        {
                           gs_runpara.chmodel &=  ~(0x01<<0);
                           
                           gs_runpara.crc16 = GetCrc16Check((u8 *)&gs_runpara,sizeof(gs_runpara)-2);

                           WriteFM24CL64(FRAM_RUN_PARA_ADDR,(u8 *)&gs_runpara,sizeof(gs_runpara));

                           BSP_Sel_GPIO_Init();

                           printfcom1("\r\n Ö÷¶¯ÉèÖÃÎªÍ¬²à");
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
                        if((gs_runpara.chmodel & (0x01<<0)) == 0)//´«¸ÐÆ÷°²×°ÓëÍ¬²à
                        {
                            gs_runpara.chmodel |=  (0x01<<0);
                           
                            gs_runpara.crc16 = GetCrc16Check((u8 *)&gs_runpara,sizeof(gs_runpara)-2);

                            WriteFM24CL64(FRAM_RUN_PARA_ADDR,(u8 *)&gs_runpara,sizeof(gs_runpara));
                            
                            BSP_Sel_GPIO_Init();

                            printfcom1("\r\n Ö÷¶¯ÉèÖÃÎªÒì²à");
                        }
                    }                    
                }
                
                //printfcom1("\r\n %d,%d,%d,%d",sinsor1phase,sinsor2phase,samesidetimes,difsidetimes);
            }
/////////////////////////////////////////////////////////////////////////////////////////////            

////////////////////////////////////////////////////////////////////////////////////////////
                //Ôö¼Ó»¬ÐÐ ¡¢¿Õ×ª ÅÐ¶Ï,¼Ó¼õËÙ
                
                //if(gs_runpara.chmodel & (01<<0))//´«¸ÐÆ÷°²×°ÓëÒì²à
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
                            printfcom1("\r\n ¼õËÙ%d",subspeedtimes);
                            //movenulflg = 2;
                            gs_runpara.flg |= (0x01 <<SUB_SPEED_FLG);
                        }
                        
                    }else if(gs_pulsech[i].period + 200 < gs_pulsech[i].lstperiod )
                    {
                        addspeedtimes++;
                        
                        if(subspeedtimes)
                            subspeedtimes--; 
                        
                        if(addspeedtimes > (workchnum + 4) * 20)
                            printfcom1("\r\n  ¼ÓËÙ%d",addspeedtimes);
                    }else{
                        
                        if(subspeedtimes)
                            subspeedtimes--; 
                        
                        if(addspeedtimes)    
                            addspeedtimes--;
                    }
                      
                   // printfcom1("\r\n addspeedtimes %d£¬%d",addspeedtimes,subspeedtimes);
                }
                        ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



            //Í¨µÀÕý³£´ÎÊý
            if(( abs(period - lstperiod) ) < period/PER_PERIOD)         //Á¬ÐøÁ½ÖÜÆÚÆ«²îÐ¡ÓÚ%30
            {       
                gs_pulsech[i].worktimes++; 
                
                if(gs_pulsech[i].worktimes > 150)                       
                {
                    if((gs_pulsech[i].flg & (0x01 << CH_IN)) == 0){
                        gs_pulsech[i].flg |= (0x01<<CH_IN);             //Õý³£100¸öÂö³å£¬ÈÏÎªÓÐÐÅºÅÊäÈë
                        printfcom1("\r\nÍ¨µÀ[%d],ÓÐÐÅºÅÊäÈë",i);
                    }                        
                }

                if(gs_pulsech[i].worktimes > 400)                       //Õý³£¹¤×÷1È¦£¬Çå¹ÊÕÏ¼ÆÊýÆ÷
                //if(gs_pulsech[i].worktimes > 300)                       //Õý³£¹¤×÷1È¦£¬Çå¹ÊÕÏ¼ÆÊýÆ÷
                {
                    gs_pulsech[i].errtimes = 0;                         //Õý³£210¸öÂö³å

                    if(gs_pulsech[i].flg &(0x01 << CH_ERR_LOSE) )
                    {
                        gs_pulsech[i].flg   &= ~(0x01 << CH_ERR_LOSE);  //|(0x01 << CH_ERR_FRQ)); //Í¨µÀ»Ö¸´
                        gs_runpara.flg      |= (0x01<<PLUSE_STORE);             //×´Ì¬±£´æ
                        printfcom1("\r\n¶ªÂö³åÍ¨µÀ%d£¬ÐÅºÅ»Ö¸´",i);
                    }                

                    gs_pulsech[i].worktimes = 1000;                     
                }
            }
            else
            {
                //20161221 È¡Ïû¸ã¸ÉÈÅ´ëÊ©
                
                //if(gs_pulsech[i].errtimes > 1)                           //²»Ë³±ãÇåÁã£¬¹ýÂË¸ÉÈÅ
                //if(gs_pulsech[i].errtimes > 0)                           //²»Ë³±ãÇåÁã£¬¹ýÂË¸ÉÈÅ
                //if((gs_runpara.flg &(0x01 <<SUB_SPEED_FLG)))    
                    gs_pulsech[i].worktimes = 0;                            //ÐÅºÅÎÊÌâ£¬ÖØÐÂ¼ÆËãÕý³£¹¤×÷´ÎÊý
                 //printfcom1("\r\n %dÖÜÆÚÆ«²îÖµ%d£¬%d£¬´íÎó´ÎÊý£º%d,×ÜÂö³åÊý %d",i,period,lstperiod,gs_pulsech[i].errtimes,l_allplusecnt);
                
                //if((gs_pulsech[i].flg & (0x01<<CH_IN)))                 //ÐÅºÅÎÈ¶¨ºó£¬ÔÙ½øÐÐ¶ªÂö³å
                {
                    if(l_allplusecnt > 2*200)                               //¸Õ¿ª³µ£¬²»×ö¶ªÂö³å¹ÊÕÏÔËËã
                        gs_pulsech[i].errtimes++;                           //¶ªÂö³å¹ÊÕÏ´ÎÊý                  
                }
            }
            
///////////////////////////////////////////////////////////////////////////////////////////////
            //¶ªÂö³åÅÐ¶Ï
            //if(gs_pulsech[i].errtimes > 10)                               //³öÏÖ10´Î£¬5È¦£¬ÈÏÎª¶ªÂö³å
            if(         gs_pulsech[i].errtimes > 20 
                //&&    (gs_pulsech[i].flg & (0x01<<CH_IN))                 //
              )                              //³öÏÖ10´Î£¬5È¦£¬ÈÏÎª¶ªÂö³å
            {               
                if((gs_pulsech[i].flg   & (0x01 << CH_ERR_LOSE)) == 0)
                {
                    gs_pulsech[i].flg   |= (0x01 << CH_ERR_LOSE);       //Í¨µÀ¶ªÂö³å
                    gs_runpara.flg      |= (0x01<<PLUSE_STORE);         //×´Ì¬±£´æ

                    if((gs_pulsech[i].flg &     (0x01 << CH_IN)) )
                        gs_pulsech[i].flg &=    ~(0x01 << CH_IN);          ////±ê¼ÇÍ¨µÀÎÞÐÅºÅ  
                    
                    printfcom1("\r\nÍ¨µÀ%d¶ªÂö³å¹ÊÕÏ",i);
                }
            }
            
            if(gs_pulsech[i].worktimes > 10)                              //¹ýÂË»Î¶¯
                gs_pulsech[i].noinputtimes = 0;                           //ÓÐÐÅºÅÊäÈë
        }
////////////////////////////////////////////////////////////////////////////////////////////////////  
        //Ã»ÓÐÐÂÂö³å,ÅÐ¶ÏÊÇ·ñ³¤Ê±¼äÎÞÐÅºÅ        
        else                                                            
        {
            if(period > ONE_KILM_PERIOD)                             //ÖÜÆÚ³¬±ê//1km/h
                period = ONE_KILM_PERIOD/10;
            
//            if(period == 0)                                
//                period = ONE_KILM_PERIOD/100;                        //ÖÜÆÚ³¬±ê//100km/h
//            
//////////////////////////////////////////////////////////////////////////////////////////////////
//ÅÐ¶ÏÊÇ·ñÃ»ÓÐÐÅºÅ
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
//                            gs_pulsech[j].flg &= ~(0x01 << CH_IN);  //±ê¼ÇÍ¨µÀÎÞÐÅºÅ  
//                            printfcom1("\r\n Í¨µÀ>¡¾ %d ¡¿ÎÞÊäÈë %x",j, (u8)gs_pulsech[j].flg);
//                        }   
//                    }
//                    
//                    Initchexcptflg();//ÇåÍ¨µÀ²ÎÊý£¬³ýflgÍâ
//                }                                                
            }
            
            if(avgperiod > ONE_KILM_PERIOD)                     //1km/h 
                avgperiod = ONE_KILM_PERIOD/50;                 //°´Æ½¾ù50km/h¼ÆËã¡£
                
            if(     (systime - risetime  >  ONE_KILM_PERIOD  )       //×îµÍÊ±¼ä0.02s
                //&&  (systime - risetime  >  100*period ))   //ÐÅºÅ°ëÈ¦ÎÞÐÅºÅ
            &&  (systime - risetime  >  10*avgperiod ))   //ÐÅºÅ°ëÈ¦ÎÞÐÅºÅ
             {
                 if(systime < gs_pulsech[i].dealtime)
                     gs_pulsech[i].dealtime = systime;
                 
                if(     systime - gs_pulsech[i].dealtime > ONE_KILM_PERIOD
                   //||  (systime - gs_pulsech[i].dealtime  >  100*period ) 
                &&  (systime - gs_pulsech[i].dealtime  >  10*avgperiod ) 
                  )  //×î´óÊ±¼ä0.15s Ö´ÐÐÒ»´Î
                {
                    gs_pulsech[i].dealtime  = systime;

                    gs_pulsech[i].noinputtimes++;

                    if(gs_pulsech[i].noinputtimes > NO_PLUSE_CIRCLE_NUM/2 )             //0.5È¦
                    {   
                        if((gs_pulsech[i].flg & (0x01 << CH_IN)))
                        {
                            gs_pulsech[i].flg &= ~(0x01 << CH_IN);  //±ê¼ÇÍ¨µÀÎÞÐÅºÅ 
                            gs_runpara.flg      |= (0x01 << PLUSE_STORE);           //×´Ì¬±£´æ
                            
                            printfcom1("\r\n Í¨µÀ¡¾%d¡¿ÎÞÊäÈë %x",i, gs_pulsech[i].flg);
                        }                        
                        
                        u8 tmp = 0;
                        for(u8 j = 0;j < 8;j++)                     //Í£³µ²¹³ä
                        {
                            if(gs_pulsech[j].noinputtimes  > 1)     //´óÓÚ1
                                tmp++;  
                        }
                        
                        if(tmp == 8)
                        {
                            if(gs_runpara.flg & (0x01<<RUN_FLG))
                            {
                                gs_runpara.flg     &= ~(0x01<<RUN_FLG);             //»ú³µÍ£³µÔËÐÐ
                                printfcom1("\r\n»ú³µÍ£³µ");
                            }
                            
                            for(u8 j = 0;j < 8;j++ )
                            {                        
                               if((gs_pulsech[j].flg & (0x01 << CH_IN)))
                                {
                                    gs_runpara.flg      |= (0x01 << PLUSE_STORE);           //×´Ì¬±£´æ
                                    gs_pulsech[j].flg   &= ~(0x01 << CH_IN);  //±ê¼ÇÍ¨µÀÎÞÐÅºÅ  
                                    printfcom1("\r\n Í¨µÀ¡¾ %d ¡¿ÎÞÊäÈë %x",j, gs_pulsech[j].flg);
                                }   
                            }
                            
                            static  uint64_t  no_pluse_time = 0;
                            if(       l_allplusecnt > 200*100                   //ÖÁÉÙÒªÓÐ10/8È¦»òÕß
                                   || (systime - no_pluse_time) > 120*1000*1000   // (9*2 / 12) 5sÇåÁãÒ»´Î
                                )
                            {
                                no_pluse_time = systime;
                                printfcom1("\r\n ÎÞÐÅºÅ%d£¡£¡%lld",l_allplusecnt,no_pluse_time);

                                l_allplusecnt = 0;
                                Initchexcptflg();//ÇåÍ¨µÀ²ÎÊý£¬³ýflgÍâ
                                addspeedtimes = 0;
                                subspeedtimes = 0;
                                gs_runpara.flg &= ~(0x01<<RUN_FLG);
                                //printfcom1("\r\n ÎÞÐÅºÅ£¡£¡");
                            }
                        }                        
                    }
////////////////////////////////////////////////////////////////////////////////////////////////////  
                  
                     //ÎÞÐÅºÅ¹ÊÕÏÅÐ¶Ï
                    if(workchnum > 0)
                    {
                        ///////////////////////////////////////////////////////
                       ///if( gs_pulsech[i].noinputtimes > NO_PLUSE_CIRCLE_NUM)                           //1È¦£¬»ò1.5s
                    
                        u8 movenulflg = 1;          //¿Õ×ª¡£»¬ÐÐ±ê¼Ç
                        if(gs_runpara.flg &(0x01 <<SUB_SPEED_FLG))
                            movenulflg = 3;         //¿Õ×ª¡¢»¬ÐÐÊ±£¬3±¶¹ÊÕÏÅÐ¶ÏÊ±¼ä
                
                        if( gs_pulsech[i].noinputtimes > NO_PLUSE_CIRCLE_NUM * movenulflg  )                           //1È¦£¬»ò1.5s
                        {
                            if((gs_pulsech[i].flg   & (0x01 << CH_ERR_LOSE)) == 0)
                            {
                                gs_pulsech[i].flg   |= (0x01 << CH_ERR_LOSE);           //Í¨µÀ¶ªÂö³å
                                gs_runpara.flg      |= (0x01 << PLUSE_STORE);           //×´Ì¬±£´æ
                                
                                u16 flgtmp;
                                flgtmp = gs_pulsech[i].flg;
                                memset(&gs_pulsech[i],0,sizeof(gs_pulsech[i]));         //ÇåÊý¾Ý
                                gs_pulsech[i].flg = flgtmp; 
                                
                                printfcom1("\r\nIÍ¨µÀ%dÎÞÐÅºÅ¹ÊÕÏ",i);
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
    //ÐÅºÅÕý³£ÅÐ¶¨
    for( u8 i   = 0; i < 8; i++ )   //È·ÈÏÕý³£¹¤×÷Í¨µÀÊý     
    {            
        if(     ( gs_pulsech[i].flg & (0x01<<CH_IN)) &&       //Í¨µÀÓÐÐ§£¬ÇÒÃ»¹ÊÕÏ
                ((gs_pulsech[i].flg & (0x01<<CH_ERR_LOSE)) == 0) &&
                ((gs_pulsech[i].flg & (0x01<<CH_ERR_FRQ))  ==0)   
           )
        {   
            gs_runpara.flg     |= (0x01<<RUN_FLG);             //»ú³µÔËÐÐ
            gs_pulsech[i].flg  |= (0x01<<CH_OK);               //Í¨µÀÕý³£
        }else{
            gs_pulsech[i].flg  &= ~(0x01<<CH_OK);
        }
    }  
}


void    JudgePulse(void)
{
    JudgeonechPulse();

    static  uint64_t    dealtime = 0;
    if(GetSysBaseTick() < dealtime + 1000*100)   //10msÅÐ¶ÏÒ»´Î9/84mhz
        return;
    dealtime = GetSysBaseTick();
    
    JudgePulseok();  
    
    JudgemultichPulse();        //¶àÍ¨µÀ´¦Àí
}



stcselch    gs_selch;       //Êä³öÍ¨µ½

extern u8  g_ch1outnum ;    //Ä¬ÈÏÎª0+1Í¨µÀ
extern u8  g_ch2outnum ;    //Ä¬ÈÏÎª1+1Í¨µÀ

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
    //Í¨µÀÕý³££¬Ñ¡ÔñµÄÐÅºÅÍ¨µÀÕý³£ 
    //Ôö¼ÓÓÐÍ¬×éÊä³ö£¬Á¢¼´ÇÐ»»¹¦ÄÜ
    
//    if(     (gs_pulsech[ch1].flg & (0x01 << CH_OK)) &&
//            (gs_pulsech[ch2].flg & (0x01 << CH_OK)) &&
//            (ch1 != ch2)
//    ){
//    ¸ü¸ÄÇÐ»»Ìõ¼þ£¬Í¨µÀ²»¹ÊÕÏ²»ÇÐ»»¡£±ÜÃâ¸ÕÔËÐÐ£¬Î´Ñ¡ÖÐÍ¨µÀÏÈÈ·ÈÏºÏ¸ñ¶øÇÐ»»Í¨µÀ      
    if(     ((gs_pulsech[ch1].flg & (0x01 << CH_ERR_LOSE))  == 0) &&    //ÎÞ¶ªÂö³å¹ÊÕÏ
            ((gs_pulsech[ch1].flg & (0x01 << CH_ERR_FRQ))   == 0) &&    //ÎÞÆµÂÊ¹ÊÕÏ
            ((gs_pulsech[ch2].flg & (0x01 << CH_ERR_LOSE))  == 0) &&    //ÎÞ¶ªÂö³å¹ÊÕÏ
            ((gs_pulsech[ch2].flg & (0x01 << CH_ERR_FRQ))   == 0) &&    //ÎÞÆµÂÊ¹ÊÕÏ
            (ch1 != ch2)
      ){
        if(     ((ch1%2==0) && ((ch1+1)==ch2))
            ||  ((ch1%2==1) && ((ch1-1)==ch2))
           ) 
        {    //Í¬×éÊä³ö£¬²»ÕÒ±¸ÓÃÍ¨µÀ
        //  if( abs(ch1-ch2) == 1) {    //Í¬×éÊä³ö£¬²»ÕÒ±¸ÓÃÍ¨µÀ
            //printfcom1("ch1%2==0) %d£¬%d",ch1,ch2);
            if(    ((gs_pulsech[ch1].flg & (0x01 << CH_IN)) && (gs_pulsech[ch2].flg & (0x01 << CH_IN)))   //Í¨µÀÊäÈëÕý³£
                ||  (gs_pulsech[ch1].period > ONE_KILM_PERIOD /10 &&  gs_pulsech[ch2].period > ONE_KILM_PERIOD /10) //ËÙ¶ÈÐ¡ÓÚ10km/h
                )   
            {
                return;
            }else{
                //¸Õ¿ª»úÊ±£¬Ñ¡ÔñÍ¨µ½ÎÞÐÅºÅ£¬Êä³ö£¬±ÜÃâ°Î²åÍ·Çé¿ö
    ////////////////////////////////////////////////////////////////////////////////////
                
                if(
                    (gs_pulsech[ch1].flg & (0x01 << FIVE_PLUSE) ) &&
                    (gs_pulsech[ch2].flg & (0x01 << FIVE_PLUSE) )       
                )
                {return;}//ÓÐÐÅºÅÊäÈë£¬²»´¦Àí
                
                //ÎÞÐÅºÅÊäÈë£¬ÕÒÆäËûÍ¨µÀÊÇ·ñÓÐÊý¾Ý£¬
                group = 0;
                for(u8 i = 0;i<4;i++,group++ )  
                {
                    ch1 = (( group % 4)*2)%8;
                    ch2 = ( ch1 + 1 )% 8;
                    if(                         //ÆäËûÍ¨µÀÓÐÊý¾ÝÊäÈë
                        (gs_pulsech[ch1].flg & (0x01 << TEN_PLUSE)) &&
                        (gs_pulsech[ch2].flg & (0x01 << TEN_PLUSE))
                       )
                     {
                        gs_selch.selchgroup     = group % 4;
                        gs_selch.selch1num      = gs_selch.selchgroup * 2; 
                        gs_selch.selch2num      = gs_selch.selch1num  + 1;

                        if(gs_runpara.chmodel & (01<<0))//´«¸ÐÆ÷°²×°ÓëÒì²à
                        {
                            if(gs_selch.selchgroup > 1)//½«5¡¢6¡¢7¡¢8Í¨µÀ·´Ïà
                            {
                                gs_selch.selch1num = gs_selch.selch2num;      //Í¨µÀ·´Ïò 
                                gs_selch.selch2num = gs_selch.selch1num - 1;  //Í¨µÀ·´Ïò       
                            }
                            
                            printfcom1("\r\n Í¨µÀ·´Ïò");
                        }
                        printfcom1("\r\n¿ª³µÇÐ»» gs_selch.selchgroup %d£¬%d£¬%d",gs_selch.selchgroup,gs_selch.selch1num,gs_selch.selch2num);

                        gs_selch.crc16 = GetCrc16Check((u8 *)&gs_selch,sizeof(gs_selch)-2 );            
                        WriteFM24CL64(FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));//´æÍ¨µÀ1 

                        BSP_PulseSel( gs_selch.selch1num, gs_selch.selch2num );//Ñ¡ÔñÐÅºÅÊä³ö
                        gs_runpara.flg      |= (0x01 << PLUSE_STORE);    


                        //×´Ì¬±£´æ                        
                    }
                }                               
            }
        }
    }
      
////////////////////////////////////////////////////////////////////////////////////////   
//    printfcom1("\r\ngroup %d£¬%d£¬%x£¬%x",gs_selch.selch1num,gs_selch.selch2num,
//                            gs_pulsech[gs_selch.selch1num].flg,gs_pulsech[gs_selch.selch2num].flg);
//    
    static  uint64_t   changetick = 0;
      
    if(GetSysBaseTick() - changetick < 1000*1000*8*2)  //s ÇÐ»»ÄÚ£¬²»½øÐÐÇÐ»»
        return;
    changetick = GetSysBaseTick();
    
    //·Ö×éÇÐ»»  
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
            
            if(gs_runpara.chmodel & (01<<0))//´«¸ÐÆ÷°²×°ÓëÒì²à
            {
                if(gs_selch.selchgroup > 1)//½«5¡¢6¡¢7¡¢8Í¨µÀ·´Ïà
                {
                    gs_selch.selch1num = gs_selch.selch2num;      //Í¨µÀ·´Ïò 
                    gs_selch.selch2num = gs_selch.selch1num - 1;  //Í¨µÀ·´Ïò       
                }
                
                printfcom1("\r\n Í¨µÀ·´Ïò");
            }
            printfcom1("\r\n 121 gs_selch.selchgroup %d£¬%d£¬%d",gs_selch.selchgroup,gs_selch.selch1num,gs_selch.selch2num);
            
            gs_selch.crc16 = GetCrc16Check((u8 *)&gs_selch,sizeof(gs_selch)-2 );            
            WriteFM24CL64(FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));//´æÍ¨µÀ1 
            
            BSP_PulseSel( gs_selch.selch1num, gs_selch.selch2num );//Ñ¡ÔñÐÅºÅÊä³ö
            gs_runpara.flg      |= (0x01 << PLUSE_STORE);           //×´Ì¬±£´æ

            changetick = GetSysBaseTick();
            //printfcom1("\r\n 131 gs_selch.selchgroup %d£¬%d£¬%d",gs_selch.selchgroup,gs_selch.selch1num,gs_selch.selch2num);
            printfflg();    
            return;
        }
    }

    
    
 //////////////////////////////////////////////////////////////////////
 //µ¥ÐÅºÅÇÐ»»    
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
        
        if(gs_pulsech[ch1].flg & (0x01 << CH_OK)){ //ÐÅºÅÓÐÐ§
        
            gs_selch.selch1num  = ch1;              //ÕÒµ½µÚÒ»¸öÓÐÐ§Í¨µÀ
            flg = 1;
            for(u8 j = 0; j < 8; j++ ) {
                ch2 = (ch1+1+j)%8;
                
                if(gs_pulsech[ch2].flg & (0x01 << CH_OK)){//ÐÅºÅÓÐÐ§
                    gs_selch.selch2num  = ch2;
                    flg = 2;
                    break;
                }
            }
        }
        
        if(flg == 2)
            break;
    }

    if(flg)                 //ÕÒµ½ÓÐÐ§Í¨µÀ£¬½øÐÐÍ¨µÀÇÐ»» 
    {
        if(flg == 1)
        {
            gs_selch.selch2num = gs_selch.selch1num;
        }
        
       //printfcom1("\r\ncurch1 %d,%d,%d,%d",gs_selch.selch1num,gs_selch.selch2num,curch1,curch2);
        
        if(     gs_selch.selch1num == curch1
            &&  gs_selch.selch2num == curch2
          )
            return;        //Í¨µÀÎ´ÇÐ»»£¬²»´¦Àí
        
        //printfcom1("\r\ncurch1 %d,%d",gs_selch.selch1num,gs_selch.selch2num);

        gs_selch.selchgroup     = 5;
        gs_selch.selchgroupbak  = 5;

//        gs_selch.sumcheck   = gs_selch.selchgroup  
//                            + gs_selch.selch1num  
//                            + gs_selch.selch2num;  
        gs_selch.crc16 = GetCrc16Check((u8 *)&gs_selch,sizeof(gs_selch)-2 );

        WriteFM24CL64(FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));//´æÍ¨µÀ1 
        ReadFM24CL64( FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));//¶ÁÍ¨µÀÊý¾Ý
        
        BSP_PulseSel( gs_selch.selch1num, gs_selch.selch2num );//Ñ¡ÔñÐÅºÅÊä³ö
        gs_runpara.flg      |= (0x01 << PLUSE_STORE);           //×´Ì¬±£´æ

        changetick = GetSysBaseTick();
        printfcom1("\r\n µ¥Í¨µÀÇÐ»» selch1num %d£¬%d£¬",gs_selch.selch1num,gs_selch.selch2num);
        printfflg();    
    }
}
