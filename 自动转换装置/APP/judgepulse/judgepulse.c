//#include "judgepulse.h"
#include "delay.h" 
#include "sensorpulse.h" 

#define     BASE_PLUSE_NUM          200
#define     PER_PERIOD              2       /*����ƫ��ٷֱ� 50%    */
#define     ONE_KILM_PERIOD         659400  /*1 km/h �ٶȶ�Ӧ������ */
#define     NO_PLUSE_CIRCLE_NUM     16      /*16*10 ���������źţ���Ϊͨ�����źŹ���*/


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

extern  uint64_t    GetSysBaseTick(void);

static  u32         l_allplusecnt = 0;          //���������

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

    printfcom1("\r\nƵ�� ");
    for(u8 i = 0 ;i < 8;i++)
    {
        printfcom1("\t%5d",84000000/(gs_pulsech[i].period*9));
    }
}


//ͨ������������,����1�������巵��0
uint8  GetchPulseInitFlg(void)
{
    for(u8 i = 0;i < 8;i++)
    {
        if(gs_pulsech[i].flg & (0x01<< PLUSE_NEW))//��������
            return  1;
    }
    return 0;
}


//��ͨ���Ա�
void    JudgemultichPulse(void)
{
    uint8           workchnum = 0;                  //��������ͨ��
    static  u32     lst_allplusecnt = 0;            //�ϴμ�����������
    
    if((lst_allplusecnt  > l_allplusecnt))          //�����жϴ���
    {
        lst_allplusecnt = l_allplusecnt;
    }
    
    if(l_allplusecnt - lst_allplusecnt < 8)         //ͨ����
        return;

    lst_allplusecnt = l_allplusecnt;
    
    for(u8 i   = 0; i < 8; i++)
        if(     (gs_pulsech[i].flg & (0x01<<CH_IN))
            &&  (gs_pulsech[i].flg & (0x01<<CH_ERR_LOSE)) == 0  )    //ͨ����Ч
            workchnum++;
        
    if(workchnum < 2 )                              //ͨ����С��2�����Ƚ�
       return;
    
    int period;
    u8  sametimes;                                  //��ͬ����
    
    for( u8 i   = 0; i < 8; i++ )                   //ȷ����������ͨ���� 
    {    
        if(     (gs_pulsech[i].flg & (0x01<<CH_IN))                 //ͨ����Ч
            &&  (gs_pulsech[i].flg & (0x01<<CH_ERR_LOSE)) == 0  )   //û�ж�����
        {      
            period = gs_pulsech[i].period;
            sametimes = 0;
            
            for(u8 j = 0; j < 8; j++ )
            {
                if(
                            (gs_pulsech[j].flg & (0x01<<CH_IN))             //ͨ����Ч
                        &&  (gs_pulsech[j].flg & (0x01<<CH_ERR_LOSE)) == 0  //û�ж�����
                  ) 
                {   
                    if(j != i)
                    {
                        u32 period1 = gs_pulsech[j].period;
                        if((abs(period - period1) < period/PER_PERIOD))     //�ж��Ƿ������ڳ���
                            sametimes++;
                    }
                }
            }
            
            if(sametimes < ((workchnum -1) / 2))                            //ͨ���������С�ڰ�������Ϊ����
            {
                gs_pulsech[i].frqerrtimes++;
                
                //if( gs_pulsech[i].frqerrtimes > 400)                      //2Ȧ
                if( gs_pulsech[i].frqerrtimes > 800)                        //Ȧ��
                {
                    if((gs_pulsech[i].flg & (0x01 << CH_ERR_FRQ)) == 0){ 
                        printfcom1("\r\nͨ��Ƶ�ʹ���CH%d��err %d",i,gs_pulsech[i].frqerrtimes);
                        gs_pulsech[i].flg |= (0x01 << CH_ERR_FRQ);      //Ƶ�ʹ��
                        gs_runpara.flg |= (0x01<<PLUSE_STORE);          //״̬����
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
                        printfcom1("\r\nͨ��Ƶ�ʹ���CH%d���",i);
                        gs_pulsech[i].flg   &= ~(0x01 << CH_ERR_FRQ);   //Ƶ�����
                        gs_runpara.flg      |= (0x01<<PLUSE_STORE);     //״̬����
                    }
                }                
            }
        }
    }
}

static  u16 addspeedtimes = 0;
static  u16 subspeedtimes = 0;


//��ͨ������
void    JudgeonechPulse(void)
{
    int         period;
    int         lstperiod;
    u8          flg;
    uint64_t    risetime; 
    uint64_t    systime;   

    uint8       workchnum = 0;                          //��������ͨ��
    for(u8 i   = 0; i < 8; i++)
        if( (gs_pulsech[i].flg & (0x01<<CH_IN)))    //ͨ����Ч
            workchnum++;

    for( u8 i   = 0; i < 8; i++ )                       //ѭ������8��ͨ��
    {
        period      = gs_pulsech[i].period;
        lstperiod   = gs_pulsech[i].lstperiod;
        flg         = gs_pulsech[i].flg;
        risetime    = gs_pulsech[i].risetime;
        systime     = GetSysBaseTick();
        
        if( flg &(0x01<<PLUSE_NEW) )                                //�ź�ͨ����������
        {                   
            gs_pulsech[i].flg &= ~(0x01<<PLUSE_NEW);                //���������ʶ
            l_allplusecnt++;                                        //�������ź����룬������++
            
            if(period < ONE_KILM_PERIOD/1000)                                //�ٶȴ���1000km/h����Ϊ�����˳�
            {
                printfcom1("\r\nͨ����%d��Ƶ�ʹ���");
                break;
            }
//�����ж�,�������ѡ��ͨ�����ϣ��л�ͨ��
////////////////////////////////////////////////////////////////////////////////////////////            
            if(gs_pulsech[i].worktimes > 5)
                gs_pulsech[i].flg |= (0x01<<FIVE_PLUSE); 
                        
            if(gs_pulsech[i].worktimes > 50)
  
            gs_pulsech[i].flg |= (0x01<<TEN_PLUSE);            

//ͬ�࣬����ж�            
/////////////////////////////////////////////////////////////////////////////////////////////
//           
            static  u16 samesidetimes = 0;
            static  u16 difsidetimes  = 0;
            
            static  u8  sinsor1phase   = 0;     //1��ǰ��2�ͺ�
            static  u8  sinsor2phase   = 0;     //1��ǰ��2�ͺ�
            if(i == 1 || i == 5)
            {
                
               if(        gs_pulsech[0].flg & (0x01<<CH_OK)
                      &&  gs_pulsech[1].flg & (0x01<<CH_OK)
               )            //�ͺ������ж�
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
               )            //�ͺ������ж�
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
               

                if(     (sinsor1phase == 1 && sinsor2phase == 1)    //ͬʱ��ǰ
                    ||  (sinsor1phase == 2 && sinsor2phase == 2)    //ͬʱ�ͺ�
                   )
                {
                    samesidetimes++;    //ͬ��
                    
                    if(samesidetimes > 2000)
                    {
                        difsidetimes = 0;
                        if(gs_runpara.chmodel & (0x01<<0))//��������װ�����
                        {
                           gs_runpara.chmodel &=  ~(0x01<<0);
                           
                           gs_runpara.crc16 = GetCrc16Check((u8 *)&gs_runpara,sizeof(gs_runpara)-2);

                           WriteFM24CL64(FRAM_RUN_PARA_ADDR,(u8 *)&gs_runpara,sizeof(gs_runpara));

                           BSP_Sel_GPIO_Init();

                           printfcom1("\r\n ��������Ϊͬ��");
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
                        if((gs_runpara.chmodel & (0x01<<0)) == 0)//��������װ��ͬ��
                        {
                            gs_runpara.chmodel |=  (0x01<<0);
                           
                            gs_runpara.crc16 = GetCrc16Check((u8 *)&gs_runpara,sizeof(gs_runpara)-2);

                            WriteFM24CL64(FRAM_RUN_PARA_ADDR,(u8 *)&gs_runpara,sizeof(gs_runpara));
                            
                            BSP_Sel_GPIO_Init();

                            printfcom1("\r\n ��������Ϊ���");
                        }
                    }                    
                }
                
                //printfcom1("\r\n %d,%d,%d,%d",sinsor1phase,sinsor2phase,samesidetimes,difsidetimes);
            }
/////////////////////////////////////////////////////////////////////////////////////////////            

////////////////////////////////////////////////////////////////////////////////////////////
                //���ӻ��� ����ת �ж�,�Ӽ���
                
                //if(gs_runpara.chmodel & (01<<0))//��������װ�����
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
                            printfcom1("\r\n ����%d",subspeedtimes);
                            //movenulflg = 2;
                            gs_runpara.flg |= (0x01 <<SUB_SPEED_FLG);
                        }
                        
                    }else if(gs_pulsech[i].period + 200 < gs_pulsech[i].lstperiod )
                    {
                        addspeedtimes++;
                        
                        if(subspeedtimes)
                            subspeedtimes--; 
                        
                        if(addspeedtimes > (workchnum + 4) * 20)
                            printfcom1("\r\n  ����%d",addspeedtimes);
                    }else{
                        
                        if(subspeedtimes)
                            subspeedtimes--; 
                        
                        if(addspeedtimes)    
                            addspeedtimes--;
                    }
                      
                   // printfcom1("\r\n addspeedtimes %d��%d",addspeedtimes,subspeedtimes);
                }
                        ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



            //ͨ����������
            if(( abs(period - lstperiod) ) < period/PER_PERIOD)         //����������ƫ��С��%30
            {       
                gs_pulsech[i].worktimes++; 
                
                if(gs_pulsech[i].worktimes > 150)                       
                {
                    if((gs_pulsech[i].flg & (0x01 << CH_IN)) == 0){
                        gs_pulsech[i].flg |= (0x01<<CH_IN);             //����100�����壬��Ϊ���ź�����
                        printfcom1("\r\nͨ��[%d],���ź�����",i);
                    }                        
                }

                if(gs_pulsech[i].worktimes > 400)                       //��������1Ȧ������ϼ�����
                //if(gs_pulsech[i].worktimes > 300)                       //��������1Ȧ������ϼ�����
                {
                    gs_pulsech[i].errtimes = 0;                         //����210������

                    if(gs_pulsech[i].flg &(0x01 << CH_ERR_LOSE) )
                    {
                        gs_pulsech[i].flg   &= ~(0x01 << CH_ERR_LOSE);  //|(0x01 << CH_ERR_FRQ)); //ͨ���ָ�
                        gs_runpara.flg      |= (0x01<<PLUSE_STORE);             //״̬����
                        printfcom1("\r\n������ͨ��%d���źŻָ�",i);
                    }                

                    gs_pulsech[i].worktimes = 1000;                     
                }
            }
            else
            {
                //20161221 ȡ������Ŵ�ʩ
                
                //if(gs_pulsech[i].errtimes > 1)                           //��˳�����㣬���˸���
                //if(gs_pulsech[i].errtimes > 0)                           //��˳�����㣬���˸���
                //if((gs_runpara.flg &(0x01 <<SUB_SPEED_FLG)))    
                    gs_pulsech[i].worktimes = 0;                            //�ź����⣬���¼���������������
                 //printfcom1("\r\n %d����ƫ��ֵ%d��%d�����������%d,�������� %d",i,period,lstperiod,gs_pulsech[i].errtimes,l_allplusecnt);
                
                //if((gs_pulsech[i].flg & (0x01<<CH_IN)))                 //�ź��ȶ����ٽ��ж�����
                {
                    if(l_allplusecnt > 2*200)                               //�տ����������������������
                        gs_pulsech[i].errtimes++;                           //��������ϴ���                  
                }
            }
            
///////////////////////////////////////////////////////////////////////////////////////////////
            //�������ж�
            //if(gs_pulsech[i].errtimes > 10)                               //����10�Σ�5Ȧ����Ϊ������
            if(         gs_pulsech[i].errtimes > 20 
                //&&    (gs_pulsech[i].flg & (0x01<<CH_IN))                 //
              )                              //����10�Σ�5Ȧ����Ϊ������
            {               
                if((gs_pulsech[i].flg   & (0x01 << CH_ERR_LOSE)) == 0)
                {
                    gs_pulsech[i].flg   |= (0x01 << CH_ERR_LOSE);       //ͨ��������
                    gs_runpara.flg      |= (0x01<<PLUSE_STORE);         //״̬����

                    if((gs_pulsech[i].flg &     (0x01 << CH_IN)) )
                        gs_pulsech[i].flg &=    ~(0x01 << CH_IN);          ////���ͨ�����ź�  
                    
                    printfcom1("\r\nͨ��%d���������",i);
                }
            }
            
            if(gs_pulsech[i].worktimes > 10)                              //���˻ζ�
                gs_pulsech[i].noinputtimes = 0;                           //���ź�����
        }
////////////////////////////////////////////////////////////////////////////////////////////////////  
        //û��������,�ж��Ƿ�ʱ�����ź�        
        else                                                            
        {
            if(period > ONE_KILM_PERIOD)                             //���ڳ���//1km/h
                period = ONE_KILM_PERIOD/10;
            
//            if(period == 0)                                
//                period = ONE_KILM_PERIOD/100;                        //���ڳ���//100km/h
//            
//////////////////////////////////////////////////////////////////////////////////////////////////
//�ж��Ƿ�û���ź�
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
//                            gs_pulsech[j].flg &= ~(0x01 << CH_IN);  //���ͨ�����ź�  
//                            printfcom1("\r\n ͨ��>�� %d �������� %x",j, (u8)gs_pulsech[j].flg);
//                        }   
//                    }
//                    
//                    Initchexcptflg();//��ͨ����������flg��
//                }                                                
            }
            
            if(avgperiod > ONE_KILM_PERIOD)                     //1km/h 
                avgperiod = ONE_KILM_PERIOD/50;                 //��ƽ��50km/h���㡣
                
            if(     (systime - risetime  >  ONE_KILM_PERIOD  )       //���ʱ��0.02s
                //&&  (systime - risetime  >  100*period ))   //�źŰ�Ȧ���ź�
            &&  (systime - risetime  >  10*avgperiod ))   //�źŰ�Ȧ���ź�
             {
                 if(systime < gs_pulsech[i].dealtime)
                     gs_pulsech[i].dealtime = systime;
                 
                if(     systime - gs_pulsech[i].dealtime > ONE_KILM_PERIOD
                   //||  (systime - gs_pulsech[i].dealtime  >  100*period ) 
                &&  (systime - gs_pulsech[i].dealtime  >  10*avgperiod ) 
                  )  //���ʱ��0.15s ִ��һ��
                {
                    gs_pulsech[i].dealtime  = systime;

                    gs_pulsech[i].noinputtimes++;

                    if(gs_pulsech[i].noinputtimes > NO_PLUSE_CIRCLE_NUM/2 )             //0.5Ȧ
                    {   
                        if((gs_pulsech[i].flg & (0x01 << CH_IN)))
                        {
                            gs_pulsech[i].flg &= ~(0x01 << CH_IN);  //���ͨ�����ź� 
                            gs_runpara.flg      |= (0x01 << PLUSE_STORE);           //״̬����
                            
                            printfcom1("\r\n ͨ����%d�������� %x",i, gs_pulsech[i].flg);
                        }                        
                        
                        u8 tmp = 0;
                        for(u8 j = 0;j < 8;j++)                     //ͣ������
                        {
                            if(gs_pulsech[j].noinputtimes  > 1)     //����1
                                tmp++;  
                        }
                        
                        if(tmp == 8)
                        {
                            if(gs_runpara.flg & (0x01<<RUN_FLG))
                            {
                                gs_runpara.flg     &= ~(0x01<<RUN_FLG);             //����ͣ������
                                printfcom1("\r\n����ͣ��");
                            }
                            
                            for(u8 j = 0;j < 8;j++ )
                            {                        
                               if((gs_pulsech[j].flg & (0x01 << CH_IN)))
                                {
                                    gs_runpara.flg      |= (0x01 << PLUSE_STORE);           //״̬����
                                    gs_pulsech[j].flg   &= ~(0x01 << CH_IN);  //���ͨ�����ź�  
                                    printfcom1("\r\n ͨ���� %d �������� %x",j, gs_pulsech[j].flg);
                                }   
                            }
                            
                            static  uint64_t  no_pluse_time = 0;
                            if(       l_allplusecnt > 200*100                   //����Ҫ��10/8Ȧ����
                                   || (systime - no_pluse_time) > 120*1000*1000   // (9*2 / 12) 5s����һ��
                                )
                            {
                                no_pluse_time = systime;
                                printfcom1("\r\n ���ź�%d����%lld",l_allplusecnt,no_pluse_time);

                                l_allplusecnt = 0;
                                Initchexcptflg();//��ͨ����������flg��
                                addspeedtimes = 0;
                                subspeedtimes = 0;
                                gs_runpara.flg &= ~(0x01<<RUN_FLG);
                                //printfcom1("\r\n ���źţ���");
                            }
                        }                        
                    }
////////////////////////////////////////////////////////////////////////////////////////////////////  
                  
                     //���źŹ����ж�
                    if(workchnum > 0)
                    {
                        ///////////////////////////////////////////////////////
                       ///if( gs_pulsech[i].noinputtimes > NO_PLUSE_CIRCLE_NUM)                           //1Ȧ����1.5s
                    
                        u8 movenulflg = 1;          //��ת�����б��
                        if(gs_runpara.flg &(0x01 <<SUB_SPEED_FLG))
                            movenulflg = 3;         //��ת������ʱ��3�������ж�ʱ��
                
                        if( gs_pulsech[i].noinputtimes > NO_PLUSE_CIRCLE_NUM * movenulflg  )                           //1Ȧ����1.5s
                        {
                            if((gs_pulsech[i].flg   & (0x01 << CH_ERR_LOSE)) == 0)
                            {
                                gs_pulsech[i].flg   |= (0x01 << CH_ERR_LOSE);           //ͨ��������
                                gs_runpara.flg      |= (0x01 << PLUSE_STORE);           //״̬����
                                
                                u16 flgtmp;
                                flgtmp = gs_pulsech[i].flg;
                                memset(&gs_pulsech[i],0,sizeof(gs_pulsech[i]));         //������
                                gs_pulsech[i].flg = flgtmp; 
                                
                                printfcom1("\r\nIͨ��%d���źŹ���",i);
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
    //�ź������ж�
    for( u8 i   = 0; i < 8; i++ )   //ȷ����������ͨ����     
    {            
        if(     ( gs_pulsech[i].flg & (0x01<<CH_IN)) &&       //ͨ����Ч����û����
                ((gs_pulsech[i].flg & (0x01<<CH_ERR_LOSE)) == 0) &&
                ((gs_pulsech[i].flg & (0x01<<CH_ERR_FRQ))  ==0)   
           )
        {   
            gs_runpara.flg     |= (0x01<<RUN_FLG);             //��������
            gs_pulsech[i].flg  |= (0x01<<CH_OK);               //ͨ������
        }else{
            gs_pulsech[i].flg  &= ~(0x01<<CH_OK);
        }
    }  
}


void    JudgePulse(void)
{
    JudgeonechPulse();

    static  uint64_t    dealtime = 0;
    if(GetSysBaseTick() < dealtime + 1000*100)   //10ms�ж�һ��9/84mhz
        return;
    dealtime = GetSysBaseTick();
    
    JudgePulseok();  
    
    JudgemultichPulse();        //��ͨ������
}



stcselch    gs_selch;       //���ͨ��

extern u8  g_ch1outnum ;    //Ĭ��Ϊ0+1ͨ��
extern u8  g_ch2outnum ;    //Ĭ��Ϊ1+1ͨ��

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
    //ͨ��������ѡ����ź�ͨ������ 
    //������ͬ������������л�����
    
//    if(     (gs_pulsech[ch1].flg & (0x01 << CH_OK)) &&
//            (gs_pulsech[ch2].flg & (0x01 << CH_OK)) &&
//            (ch1 != ch2)
//    ){
//    �����л�������ͨ�������ϲ��л�����������У�δѡ��ͨ����ȷ�Ϻϸ���л�ͨ��      
    if(     ((gs_pulsech[ch1].flg & (0x01 << CH_ERR_LOSE))  == 0) &&    //�޶��������
            ((gs_pulsech[ch1].flg & (0x01 << CH_ERR_FRQ))   == 0) &&    //��Ƶ�ʹ���
            ((gs_pulsech[ch2].flg & (0x01 << CH_ERR_LOSE))  == 0) &&    //�޶��������
            ((gs_pulsech[ch2].flg & (0x01 << CH_ERR_FRQ))   == 0) &&    //��Ƶ�ʹ���
            (ch1 != ch2)
      ){
        if(     ((ch1%2==0) && ((ch1+1)==ch2))
            ||  ((ch1%2==1) && ((ch1-1)==ch2))
           ) 
        {    //ͬ����������ұ���ͨ��
        //  if( abs(ch1-ch2) == 1) {    //ͬ����������ұ���ͨ��
            //printfcom1("ch1%2==0) %d��%d",ch1,ch2);
            if(    ((gs_pulsech[ch1].flg & (0x01 << CH_IN)) && (gs_pulsech[ch2].flg & (0x01 << CH_IN)))   //ͨ����������
                ||  (gs_pulsech[ch1].period > ONE_KILM_PERIOD /10 &&  gs_pulsech[ch2].period > ONE_KILM_PERIOD /10) //�ٶ�С��10km/h
                )   
            {
                return;
            }else{
                //�տ���ʱ��ѡ��ͨ�����źţ����������β�ͷ���
    ////////////////////////////////////////////////////////////////////////////////////
                
                if(
                    (gs_pulsech[ch1].flg & (0x01 << FIVE_PLUSE) ) &&
                    (gs_pulsech[ch2].flg & (0x01 << FIVE_PLUSE) )       
                )
                {return;}//���ź����룬������
                
                //���ź����룬������ͨ���Ƿ������ݣ�
                group = 0;
                for(u8 i = 0;i<4;i++,group++ )  
                {
                    ch1 = (( group % 4)*2)%8;
                    ch2 = ( ch1 + 1 )% 8;
                    if(                         //����ͨ������������
                        (gs_pulsech[ch1].flg & (0x01 << TEN_PLUSE)) &&
                        (gs_pulsech[ch2].flg & (0x01 << TEN_PLUSE))
                       )
                     {
                        gs_selch.selchgroup     = group % 4;
                        gs_selch.selch1num      = gs_selch.selchgroup * 2; 
                        gs_selch.selch2num      = gs_selch.selch1num  + 1;

                        if(gs_runpara.chmodel & (01<<0))//��������װ�����
                        {
                            if(gs_selch.selchgroup > 1)//��5��6��7��8ͨ������
                            {
                                gs_selch.selch1num = gs_selch.selch2num;      //ͨ������ 
                                gs_selch.selch2num = gs_selch.selch1num - 1;  //ͨ������       
                            }
                            
                            printfcom1("\r\n ͨ������");
                        }
                        printfcom1("\r\n�����л� gs_selch.selchgroup %d��%d��%d",gs_selch.selchgroup,gs_selch.selch1num,gs_selch.selch2num);

                        gs_selch.crc16 = GetCrc16Check((u8 *)&gs_selch,sizeof(gs_selch)-2 );            
                        WriteFM24CL64(FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));//��ͨ��1 

                        BSP_PulseSel( gs_selch.selch1num, gs_selch.selch2num );//ѡ���ź����
                        gs_runpara.flg      |= (0x01 << PLUSE_STORE);    


                        //״̬����                        
                    }
                }                               
            }
        }
    }
      
////////////////////////////////////////////////////////////////////////////////////////   
//    printfcom1("\r\ngroup %d��%d��%x��%x",gs_selch.selch1num,gs_selch.selch2num,
//                            gs_pulsech[gs_selch.selch1num].flg,gs_pulsech[gs_selch.selch2num].flg);
//    
    static  uint64_t   changetick = 0;
      
    if(GetSysBaseTick() - changetick < 1000*1000*8*2)  //s �л��ڣ��������л�
        return;
    changetick = GetSysBaseTick();
    
    //�����л�  
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
            
            if(gs_runpara.chmodel & (01<<0))//��������װ�����
            {
                if(gs_selch.selchgroup > 1)//��5��6��7��8ͨ������
                {
                    gs_selch.selch1num = gs_selch.selch2num;      //ͨ������ 
                    gs_selch.selch2num = gs_selch.selch1num - 1;  //ͨ������       
                }
                
                printfcom1("\r\n ͨ������");
            }
            printfcom1("\r\n 121 gs_selch.selchgroup %d��%d��%d",gs_selch.selchgroup,gs_selch.selch1num,gs_selch.selch2num);
            
            gs_selch.crc16 = GetCrc16Check((u8 *)&gs_selch,sizeof(gs_selch)-2 );            
            WriteFM24CL64(FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));//��ͨ��1 
            
            BSP_PulseSel( gs_selch.selch1num, gs_selch.selch2num );//ѡ���ź����
            gs_runpara.flg      |= (0x01 << PLUSE_STORE);           //״̬����

            changetick = GetSysBaseTick();
            //printfcom1("\r\n 131 gs_selch.selchgroup %d��%d��%d",gs_selch.selchgroup,gs_selch.selch1num,gs_selch.selch2num);
            printfflg();    
            return;
        }
    }

    
    
 //////////////////////////////////////////////////////////////////////
 //���ź��л�    
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
        
        if(gs_pulsech[ch1].flg & (0x01 << CH_OK)){ //�ź���Ч
        
            gs_selch.selch1num  = ch1;              //�ҵ���һ����Чͨ��
            flg = 1;
            for(u8 j = 0; j < 8; j++ ) {
                ch2 = (ch1+1+j)%8;
                
                if(gs_pulsech[ch2].flg & (0x01 << CH_OK)){//�ź���Ч
                    gs_selch.selch2num  = ch2;
                    flg = 2;
                    break;
                }
            }
        }
        
        if(flg == 2)
            break;
    }

    if(flg)                 //�ҵ���Чͨ��������ͨ���л� 
    {
        if(flg == 1)
        {
            gs_selch.selch2num = gs_selch.selch1num;
        }
        
       //printfcom1("\r\ncurch1 %d,%d,%d,%d",gs_selch.selch1num,gs_selch.selch2num,curch1,curch2);
        
        if(     gs_selch.selch1num == curch1
            &&  gs_selch.selch2num == curch2
          )
            return;        //ͨ��δ�л���������
        
        //printfcom1("\r\ncurch1 %d,%d",gs_selch.selch1num,gs_selch.selch2num);

        gs_selch.selchgroup     = 5;
        gs_selch.selchgroupbak  = 5;

//        gs_selch.sumcheck   = gs_selch.selchgroup  
//                            + gs_selch.selch1num  
//                            + gs_selch.selch2num;  
        gs_selch.crc16 = GetCrc16Check((u8 *)&gs_selch,sizeof(gs_selch)-2 );

        WriteFM24CL64(FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));//��ͨ��1 
        ReadFM24CL64( FRAM_SEL_CH_ADDR,(uint8 *)&gs_selch,sizeof(gs_selch));//��ͨ������
        
        BSP_PulseSel( gs_selch.selch1num, gs_selch.selch2num );//ѡ���ź����
        gs_runpara.flg      |= (0x01 << PLUSE_STORE);           //״̬����

        changetick = GetSysBaseTick();
        printfcom1("\r\n ��ͨ���л� selch1num %d��%d��",gs_selch.selch1num,gs_selch.selch2num);
        printfflg();    
    }
}
