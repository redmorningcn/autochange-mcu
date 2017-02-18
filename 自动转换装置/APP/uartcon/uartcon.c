#include "delay.h" 
#include "sensorpulse.h" 
#include  "csnr_package_deal.h"
//#include  "CrcCheck.h"
void printfcom1(const char *format, ...);


//#define		DATA_CARD           0x0001			//数据卡
//#define		DENSITY_CARD		0x0020			//密度卡
//#define		MODEL_CARD			0x0040			//参数模型卡
//#define		TIME_CARD           0x0100			//时间卡
//#define		COM_MODEL_CARD	    0x0100		    //参数复杂邮箱模型
//#define		HIGHT_CARD			0x0200			//高度调整卡
//#define		COPY_CARD           0x0400			//FLASH复制卡
//#define		MODEL_SELECT_CARD	0x1000			//模型选择卡
//#define		FIX_CARD            0x2000			//装车卡
//#define		COPY_FRAM_CARD	    0x4000			//铁电拷贝卡

//#define		SET_DATA_CARD		0x0080			//置数卡
//#define		DEBUG_CARD			0x0800			//调试复制卡

//#define		EXPAND_CARD			0x8000			//扩展卡

//#define		DATA_CARD_DIS		0x10000			//数据卡
//#define		DATA_CARD_FIN		0x20000			//数据卡
//#define		SYS_RUN_PARA		0x40000			//设置运行参数
#define		    CLEAR_ALL			0x80000			//设置运行参数

#define		    RST_SYS				0x100000		//系统复位
#define		    CALC_PARA           0x200000		//油量计算系数

#define		    TIME_CARD           0x000100        //时间卡
#define		    PRO_CARD            0x400000        //产品参数
#define		    READ_DATA_CARD      0x800000        //产品读数

void    UartSendRec(u32    recnum);

void UartCodeOperate(unsigned char *buf,unsigned int Len)		
{
	unsigned int   datalen = 0;
	unsigned char  resflg = 1;
		
	if(Len < 4)             //小于最小长度，退出
		return	;
	
    u32 codetype;
    
    datalen = 0;

	memcpy((unsigned char *)&codetype,&buf[datalen],sizeof(codetype));
	datalen += sizeof(codetype);
			
	if(codetype == 0)       //操作码未定义
        return;
        
    switch (codetype)
    {
        stcTime     sTime;   
                                            
        case TIME_CARD:     //设置时间
                        memcpy((unsigned char *)&sTime,&buf[datalen],sizeof(sTime));
                        printfcom1("\r\n 时间：%d-%d-%d %d-%d-%d",sTime.Year,sTime.Month,sTime.Date,sTime.Hour,sTime.Min,sTime.Sec);
                        WriteTime(sTime);
                        break; 
        case PRO_CARD:      //设置产品信息    
                        memcpy((unsigned char *)&gs_proinfo.pdtype,&buf[datalen],sizeof(gs_proinfo.pdtype));
                        datalen += sizeof(gs_proinfo.pdtype);

                        memcpy((unsigned char *)&gs_proinfo.pdid,&buf[datalen],sizeof(gs_proinfo.pdid));
                        datalen += sizeof(gs_proinfo.pdid);

                        memcpy((unsigned char *)&gs_proinfo.version,&buf[datalen],sizeof(gs_proinfo.version));
                        datalen += sizeof(gs_proinfo.version);

                        printfcom1("\r\n产品编号：%s",gs_proinfo.pdid);

                        StoreProInfo_Fram();

                        break; 
        case CLEAR_ALL:     //请参数	
                        printfcom1("\r\n 清零"); 
                        //  //DisplaySet(250,1,"C_ALL");
                        break;
        case READ_DATA_CARD:
                        printfcom1("\r\n 记录："); 
                        //
                        PrintfRecord(gs_runpara.writerecnum-1);
                        printfcom1("\r\n");

                        UartSendRec(gs_runpara.writerecnum-1);  //发送当前记录数据
                        printfcom1("\r\n");
                        break;
        case RST_SYS:       
                          //ResetSys();	//重启
                        while(1);
                        break;
        default: ;
    }
}

#define MAX_ADDR            0x7FFFFF                                         /* 定义芯片内部最大地址         */
#define MAX_REC_NUM         ((MAX_ADDR )/sizeof(stcrecord))

//串口发送数据记录
void    UartSendRec(u32    recnum)
{
    stcrecord   srecord;
        
    uint32  addr  = sizeof(stcrecord)*((recnum) % MAX_REC_NUM);//取存储地址
    MX25L1602_RD(addr,sizeof(stcrecord),(u8 *)&srecord );       //取数据
    
    stcCsnrProtocolPara 	sprotocol;	                        //准备打包
    sprotocol.sourceaddr    = 0x80;                             //源地址
    sprotocol.destaddr      = 0xc1;                             //目标地址
    sprotocol.framcode      = 0x00;                             //故障
    sprotocol.databuf       = (u8 *)&srecord;                   //数据区
    sprotocol.datalen       = sizeof(srecord);                  //数据区长度
    
    u8  probuf[256];
    u8  probuflen;

    DataPackage_CSNR(sprotocol,probuf,(u8 *)&probuflen);          //数据打包
    
    SendDataCom1(probuf,probuflen);                        //发送数据包
}


//串口通讯
void  UartConTask(void)     									
{
	u8  recbuf[256]     = {0};						
	u8	databuf[256]    = {0};
    stcCsnrProtocolPara 	sprotocol;	

    HoldUartRecSta();           //串口状态查询
    
    u32 reclen = ReadUartBuf(recbuf,sizeof(recbuf));
    
    if( reclen == 0 )
        return ;                //无通讯数据，退出
    
    if( reclen < 10 )           //通讯数据小于最小帧长度，退出。
        return ;
    
    sprotocol.databuf = databuf;        
    u8  recflg = DataUnpack_CSNR((stcCsnrProtocolPara *)&sprotocol,recbuf,reclen);  //协议解析
    
    if(recflg == 0)                                 //协议解析失败,退出
    {
        return ;
    }
    
    u8  souraddr = sprotocol.sourceaddr;    //源地址
    
    switch(souraddr)                               
    {
        case 0xCA: 
            
            break;
            
        case 0xC1:      //进行参数设置
            UartCodeOperate(sprotocol.databuf,sprotocol.datalen); 			//读卡器对接收的信息进行处理

            break;
            
        case 0xC2:      //进行参数设置
            UartCodeOperate(sprotocol.databuf,sprotocol.datalen); 			//读卡器对接收的信息进行处理

            break;
            
        case 0xC3: 

            break;	
            
        default: ;
                                                                //′í?óD??￠
    }
}