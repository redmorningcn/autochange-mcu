#include "delay.h" 
#include "sensorpulse.h" 
#include  "csnr_package_deal.h"
//#include  "CrcCheck.h"


//���ݴ��������stcCsnrProtocolPara��Ϣ�������ݴ����CNSR����
//
void	DataPackage_CSNR(stcCsnrProtocolPara sprotocolpara,unsigned char 	*csnrbuf,unsigned char 	*csnrlen)
{
	unsigned char		SndBuf[256] = {0};
	unsigned char		SumCheck8;
	unsigned short	    Crc16;
	unsigned short	    AddHeadNum;
	unsigned short	    i;
	unsigned char		ByteStation;
	unsigned char 	    FramNum =0;						//���
	unsigned char		DataLen;
	
	ByteStation = 0;
	SndBuf[ByteStation++] = FRAM_HERD0;
	SndBuf[ByteStation++] = FRAM_HERD1;
	SndBuf[ByteStation++] = sprotocolpara.sourceaddr;	 
	SndBuf[ByteStation++] = sprotocolpara.destaddr;				//��ַ
	SndBuf[ByteStation++] = sprotocolpara.framnum;
	SndBuf[ByteStation++] = (RS485_CHK_CRC<<4) + sprotocolpara.framcode;	
	SndBuf[ByteStation++] = sprotocolpara.datalen;
	
	DataLen = sprotocolpara.datalen;
	
	SumCheck8 = GetCheckSumNR(&SndBuf[2],5);		   	//������У��

	SndBuf[ByteStation++] = SumCheck8;

	memcpy(&SndBuf[8],sprotocolpara.databuf,DataLen);

	Crc16 = GetCrc16Check(&SndBuf[2],6+sprotocolpara.datalen);
	
	SndBuf[2+6+DataLen+1] = Crc16;
	SndBuf[2+6+DataLen]   = Crc16>>8;

	csnrbuf[0] = SndBuf[0];						//׼�����ݷ���
	csnrbuf[1] = SndBuf[1];
	
	AddHeadNum = 0;
	for(i = 2; i< 2+6+DataLen + 2;i++ )				//���ݣ�����λ FRAM_HERD0
	{
		csnrbuf[i+AddHeadNum] = SndBuf[i];
		
		if(SndBuf[i] == FRAM_HERD0)
		{
			csnrbuf[i+AddHeadNum+1] = FRAM_HERD0;
			AddHeadNum++;
		}
	}

	csnrbuf[2+6+DataLen + 2 +AddHeadNum] 		= FRAM_END0;
	csnrbuf[2+6+DataLen + 2 +AddHeadNum+1] 	= FRAM_END1;      
	
	*csnrlen =  2+6+DataLen + 2 + AddHeadNum +2;
}


//���ݽ���������ܵ������ݽ������stcCsnrProtocolPara��Ϣ
//�ܼ���������1�����򣬷���0
unsigned char   DataUnpack_CSNR(stcCsnrProtocolPara *sprotocolpara,unsigned char	*RecBuf,unsigned char RecLen)
{
    unsigned short		i,j,k;
    unsigned char			SumCheck8;
    unsigned char			Tmp;
    unsigned short		RecCRC16,Check16;
		unsigned char			DataLen;
		unsigned int			herdtimes = 0;
		

  if(RecLen < 10)
		return 0;		
	
	for(j = 0;j < RecLen - herdtimes;j++ )														//ȥ�ظ�ͨѶ���ֽ�
	{
		if(	 	
				RecBuf[j] 	== FRAM_HERD0
			&& 	RecBuf[j+1] == FRAM_HERD0
		   )
		{
			for(k = j;k < RecLen - herdtimes; k++)
			{
				Tmp	 = RecBuf[k+1];
				
				RecBuf[k] = Tmp;
			}
			
			herdtimes++;
		}
	}
		

    for(i = 0; i < RecLen - herdtimes - 10;i++)												//ѭ������
    {
		if(		((RecBuf[i+0]) == FRAM_HERD0) 
			&& 	((RecBuf[i+1]) == FRAM_HERD1)		
		  )																		//��ͷ
		{
			DataLen = RecBuf[i+6];
			if(		
					(i+10+DataLen)  > RecLen							||
			 		(RecBuf[i+10+DataLen] 	!= FRAM_END0)	||
			 	 	(RecBuf[i+11+DataLen] 	!= FRAM_END1)
			 )
			 {	
			 //i = i + 10 + DataLen;											//��ַ���Ի�֡β����������֡
			 	continue;
			 }
			SumCheck8 = GetCheckSumNR((unsigned char *)&RecBuf[i+2],5);		   				//������У��
			
			if(	SumCheck8 != 	RecBuf[i+7] )
			{	
				continue;
			}
			 
			DataLen = RecBuf[i+6];
						
			RecCRC16 = ((unsigned short)RecBuf[i+8+DataLen]) *256 + RecBuf[i+9+DataLen];	//У��	

			switch(RecBuf[i+5]>>4)													//У����㷨�ж�
			{
					case	RS485_CHK_SUM:
									//Check16 = GetCheckSum16(&RecBuf[i+2],6+DataLen);
									////PrintfCOM0(("\r\n 16λУ��ͣ�");
									break;
					case	RS485_CHK_CRC:
									Check16 = GetCrc16Check(&RecBuf[i+2],6+DataLen);
									////PrintfCOM0(("\r\n CRCУ�飡");
									break;
					case	RS485_CHK_RESUM:
									break;
			}
			
			if(			RecCRC16  ==	Check16 	 )
			{
				sprotocolpara->sourceaddr 	    = RecBuf[i+2];	
				sprotocolpara->destaddr 		= RecBuf[i+3];	
				sprotocolpara->framnum   		= RecBuf[i+4];
				sprotocolpara->framcode  		= RecBuf[i+5]&0x0f;
				sprotocolpara->datalen     	= DataLen; 
				
				memcpy(sprotocolpara->databuf,&RecBuf[i+8],DataLen);				//���ݿ���
				 
				return	1;
			}
		}
	}
  
    return	0;
}
