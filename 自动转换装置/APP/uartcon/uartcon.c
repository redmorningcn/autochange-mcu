#include "delay.h" 
#include "sensorpulse.h" 
#include  "csnr_package_deal.h"
//#include  "CrcCheck.h"
void printfcom1(const char *format, ...);


//#define		DATA_CARD           0x0001			//���ݿ�
//#define		DENSITY_CARD		0x0020			//�ܶȿ�
//#define		MODEL_CARD			0x0040			//����ģ�Ϳ�
//#define		TIME_CARD           0x0100			//ʱ�俨
//#define		COM_MODEL_CARD	    0x0100		    //������������ģ��
//#define		HIGHT_CARD			0x0200			//�߶ȵ�����
//#define		COPY_CARD           0x0400			//FLASH���ƿ�
//#define		MODEL_SELECT_CARD	0x1000			//ģ��ѡ��
//#define		FIX_CARD            0x2000			//װ����
//#define		COPY_FRAM_CARD	    0x4000			//���翽����

//#define		SET_DATA_CARD		0x0080			//������
//#define		DEBUG_CARD			0x0800			//���Ը��ƿ�

//#define		EXPAND_CARD			0x8000			//��չ��

//#define		DATA_CARD_DIS		0x10000			//���ݿ�
//#define		DATA_CARD_FIN		0x20000			//���ݿ�
//#define		SYS_RUN_PARA		0x40000			//�������в���
#define		    CLEAR_ALL			0x80000			//�������в���

#define		    RST_SYS				0x100000		//ϵͳ��λ
#define		    CALC_PARA           0x200000		//��������ϵ��

#define		    TIME_CARD           0x000100        //ʱ�俨
#define		    PRO_CARD            0x400000        //��Ʒ����
#define		    READ_DATA_CARD      0x800000        //��Ʒ����

void    UartSendRec(u32    recnum);

void UartCodeOperate(unsigned char *buf,unsigned int Len)		
{
	unsigned int   datalen = 0;
	unsigned char  resflg = 1;
		
	if(Len < 4)             //С����С���ȣ��˳�
		return	;
	
    u32 codetype;
    
    datalen = 0;

	memcpy((unsigned char *)&codetype,&buf[datalen],sizeof(codetype));
	datalen += sizeof(codetype);
			
	if(codetype == 0)       //������δ����
        return;
        
    switch (codetype)
    {
        stcTime     sTime;   
                                            
        case TIME_CARD:     //����ʱ��
                        memcpy((unsigned char *)&sTime,&buf[datalen],sizeof(sTime));
                        printfcom1("\r\n ʱ�䣺%d-%d-%d %d-%d-%d",sTime.Year,sTime.Month,sTime.Date,sTime.Hour,sTime.Min,sTime.Sec);
                        WriteTime(sTime);
                        break; 
        case PRO_CARD:      //���ò�Ʒ��Ϣ    
                        memcpy((unsigned char *)&gs_proinfo.pdtype,&buf[datalen],sizeof(gs_proinfo.pdtype));
                        datalen += sizeof(gs_proinfo.pdtype);

                        memcpy((unsigned char *)&gs_proinfo.pdid,&buf[datalen],sizeof(gs_proinfo.pdid));
                        datalen += sizeof(gs_proinfo.pdid);

                        memcpy((unsigned char *)&gs_proinfo.version,&buf[datalen],sizeof(gs_proinfo.version));
                        datalen += sizeof(gs_proinfo.version);

                        printfcom1("\r\n��Ʒ��ţ�%s",gs_proinfo.pdid);

                        StoreProInfo_Fram();

                        break; 
        case CLEAR_ALL:     //�����	
                        printfcom1("\r\n ����"); 
                        //  //DisplaySet(250,1,"C_ALL");
                        break;
        case READ_DATA_CARD:
                        printfcom1("\r\n ��¼��"); 
                        //
                        PrintfRecord(gs_runpara.writerecnum-1);
                        printfcom1("\r\n");

                        UartSendRec(gs_runpara.writerecnum-1);  //���͵�ǰ��¼����
                        printfcom1("\r\n");
                        break;
        case RST_SYS:       
                          //ResetSys();	//����
                        while(1);
                        break;
        default: ;
    }
}

#define MAX_ADDR            0x7FFFFF                                         /* ����оƬ�ڲ�����ַ         */
#define MAX_REC_NUM         ((MAX_ADDR )/sizeof(stcrecord))

//���ڷ������ݼ�¼
void    UartSendRec(u32    recnum)
{
    stcrecord   srecord;
        
    uint32  addr  = sizeof(stcrecord)*((recnum) % MAX_REC_NUM);//ȡ�洢��ַ
    MX25L1602_RD(addr,sizeof(stcrecord),(u8 *)&srecord );       //ȡ����
    
    stcCsnrProtocolPara 	sprotocol;	                        //׼�����
    sprotocol.sourceaddr    = 0x80;                             //Դ��ַ
    sprotocol.destaddr      = 0xc1;                             //Ŀ���ַ
    sprotocol.framcode      = 0x00;                             //����
    sprotocol.databuf       = (u8 *)&srecord;                   //������
    sprotocol.datalen       = sizeof(srecord);                  //����������
    
    u8  probuf[256];
    u8  probuflen;

    DataPackage_CSNR(sprotocol,probuf,(u8 *)&probuflen);          //���ݴ��
    
    SendDataCom1(probuf,probuflen);                        //�������ݰ�
}


//����ͨѶ
void  UartConTask(void)     									
{
	u8  recbuf[256]     = {0};						
	u8	databuf[256]    = {0};
    stcCsnrProtocolPara 	sprotocol;	

    HoldUartRecSta();           //����״̬��ѯ
    
    u32 reclen = ReadUartBuf(recbuf,sizeof(recbuf));
    
    if( reclen == 0 )
        return ;                //��ͨѶ���ݣ��˳�
    
    if( reclen < 10 )           //ͨѶ����С����С֡���ȣ��˳���
        return ;
    
    sprotocol.databuf = databuf;        
    u8  recflg = DataUnpack_CSNR((stcCsnrProtocolPara *)&sprotocol,recbuf,reclen);  //Э�����
    
    if(recflg == 0)                                 //Э�����ʧ��,�˳�
    {
        return ;
    }
    
    u8  souraddr = sprotocol.sourceaddr;    //Դ��ַ
    
    switch(souraddr)                               
    {
        case 0xCA: 
            
            break;
            
        case 0xC1:      //���в�������
            UartCodeOperate(sprotocol.databuf,sprotocol.datalen); 			//�������Խ��յ���Ϣ���д���

            break;
            
        case 0xC2:      //���в�������
            UartCodeOperate(sprotocol.databuf,sprotocol.datalen); 			//�������Խ��յ���Ϣ���д���

            break;
            
        case 0xC3: 

            break;	
            
        default: ;
                                                                //�䨪?��D??��
    }
}