/****************************************Copyright (c)**************************************************
**                               Guangzou ZLG-MCU Development Co.,LTD.
**                                      graduate school
**                                 http://www.zlgmcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:       MX25L1602.c
** Descriptions:    SST25VF016B����
**
**------------------------------------------------------------------------------------------------------
** Created by:      Litiantian
** Created date:    2007-04-16
** Version:         1.0
** Descriptions:    The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:     
** Modified date:   
** Version:         
** Descriptions:    
********************************************************************************************************/
#include   "MX25L1602Drv.h"
//#include   "includes.h"
/*
 * ��ֲ�������ʱ��Ҫ�޸����µĺ������ 
 */
//#define     CE_Low()    LPC_GPIO0->FIOCLR |= SPI_CS;
//#define     CE_High()   LPC_GPIO0->FIOSET |= SPI_CS;


extern	void WriteSoftSpiCSF(uint8 temp);

void	CE_Low(void)
{
	WriteSoftSpiCSF(0);
}

void	CE_High(void)
{
	WriteSoftSpiCSF(1);
}

//#define			SCKF					204    	    //p1.20  
//#define			MISOF					203			//p1.23  
//#define			MOSIF					205			//p1.24 
//#define   		SOFT_SPI_CSF			202			//p1.22

#define   		BSP_GPIO_SOFT_SPI_CSF			GPIO_Pin_3			//pe
#define			BSP_GPIO_MISOF					GPIO_Pin_4			//pe
#define			BSP_GPIO_SCKF					GPIO_Pin_5         //pe
#define			BSP_GPIO_MOSIF					GPIO_Pin_6			//pe

#define	uint8	uint8_t

void	InitFlashIO(void)
{
    GPIO_InitTypeDef  gpio_init;

    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_25MHz;
    
    gpio_init.GPIO_Pin   = BSP_GPIO_SCKF		 
                         | BSP_GPIO_MISOF		 
                         | BSP_GPIO_MOSIF		
                         | BSP_GPIO_SOFT_SPI_CSF;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);                          /* Configure GPIOG for LED1 and LED2                    */
    GPIO_Init(GPIOE, &gpio_init);    
}

//------------------------------------------------------------------------
//  ��  �� ��void WriteSCKF( uint8 temp )
//  ��  �� ������ SCKF
// ��ڲ�������
// ���ڲ�������
//------------------------------------------------------------------------
void WriteSCKF(uint8 temp)
{
	
    GPIO_InitTypeDef GPIO_InitStruct;
 
    GPIO_InitStruct.GPIO_Pin     = BSP_GPIO_SCKF;
    GPIO_InitStruct.GPIO_Mode    = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_PuPd    = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed   = GPIO_Speed_25MHz;

    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    delay_us(1);
    
	if(temp)		
	{
        GPIO_WriteBit(GPIOE, BSP_GPIO_SCKF, Bit_SET);    
	}
	else
	{
		//GPIO_SetOrClearValue(SCL,0);		
        GPIO_WriteBit(GPIOE, BSP_GPIO_SCKF, Bit_RESET);    
	}	
}

//------------------------------------------------------------------------
//  ��  �� ��uint8 ReadSCKF(void)
//  ��  �� ����ȡ SCKF ��ֵ
// ��ڲ�������
// ���ڲ�������
//------------------------------------------------------------------------
uint8 ReadSCKF(void)
{
	uint8 temp;

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = BSP_GPIO_SCKF;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    delay_us(1);
    
    temp = GPIO_ReadInputDataBit( GPIOE,  BSP_GPIO_SCKF); 

	return temp;
}

//------------------------------------------------------------------------
//  ��  �� ��void WriteMOSIF( uint8 temp )
//  ��  �� ������ MOSIF
// ��ڲ�������
// ���ڲ�������
//------------------------------------------------------------------------
void WriteMOSIF(uint8 temp)
{	
    GPIO_InitTypeDef GPIO_InitStruct;
 
    GPIO_InitStruct.GPIO_Pin     = BSP_GPIO_MOSIF;
    GPIO_InitStruct.GPIO_Mode    = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_PuPd    = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed   = GPIO_Speed_25MHz;

    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    delay_us(1);
    
	if(temp)		
	{
        GPIO_WriteBit(GPIOE, BSP_GPIO_MOSIF, Bit_SET);    
	}
	else
	{
		//GPIO_SetOrClearValue(SCL,0);		
        GPIO_WriteBit(GPIOE, BSP_GPIO_MOSIF, Bit_RESET);    
	}		
}

//------------------------------------------------------------------------
//  ��  �� ��uint8 ReadMOSIF(void)
//  ��  �� ����ȡ MOSIF ��ֵ
// ��ڲ�������
// ���ڲ�������
//------------------------------------------------------------------------
uint8 ReadMOSIF(void)
{
	uint8 temp;
	
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = BSP_GPIO_MOSIF;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    delay_us(1);
    
    temp = GPIO_ReadInputDataBit( GPIOE,  BSP_GPIO_MOSIF); 

	return temp;
}

//------------------------------------------------------------------------
//  ��  �� ��void WriteMOSIF( uint8 temp )
//  ��  �� ������ MOSIF
// ��ڲ�������
// ���ڲ�������
//------------------------------------------------------------------------
void WriteMISOF(uint8 temp)
{	
    GPIO_InitTypeDef GPIO_InitStruct;
 
    GPIO_InitStruct.GPIO_Pin     = BSP_GPIO_MISOF;
    GPIO_InitStruct.GPIO_Mode    = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_PuPd    = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed   = GPIO_Speed_25MHz;

    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    delay_us(1);
    
	if(temp)		
	{
        GPIO_WriteBit(GPIOE, BSP_GPIO_MISOF, Bit_SET);    
	}
	else
	{
		//GPIO_SetOrClearValue(SCL,0);		
        GPIO_WriteBit(GPIOE, BSP_GPIO_MISOF, Bit_RESET);    
	}		
}

//------------------------------------------------------------------------
//  ��  �� ��uint8 ReadMOSIF(void)
//  ��  �� ����ȡ MOSIF ��ֵ
// ��ڲ�������
// ���ڲ�������
//------------------------------------------------------------------------
uint8 ReadMISOF(void)
{
	uint8 temp;

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = BSP_GPIO_MISOF;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    delay_us(1);

    temp = GPIO_ReadInputDataBit( GPIOE,  BSP_GPIO_MISOF); 

	return temp;
}

//------------------------------------------------------------------------
//  ��  �� ��void WriteMOSIF( uint8 temp )
//  ��  �� ������ MOSIF
// ��ڲ�������
// ���ڲ�������
//------------------------------------------------------------------------
void WriteSoftSpiCSF(uint8 temp)
{
    GPIO_InitTypeDef GPIO_InitStruct;
 
    GPIO_InitStruct.GPIO_Pin     = BSP_GPIO_SOFT_SPI_CSF;
    GPIO_InitStruct.GPIO_Mode    = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_PuPd    = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed   = GPIO_Speed_25MHz;

    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    delay_us(1);
    
	if(temp)		
	{
        GPIO_WriteBit(GPIOE, BSP_GPIO_SOFT_SPI_CSF, Bit_SET);    
	}
	else
	{
		//GPIO_SetOrClearValue(SCL,0);		
        GPIO_WriteBit(GPIOE, BSP_GPIO_SOFT_SPI_CSF, Bit_RESET);    
	}		
}

//---------------------------------------------------------------------------------
//��������:             SentByte_SPI()
//��    ��:             SPI����һ�ֽ�����
//��ڲ���:             Data8
//���ڲ���:             ��
//˵    ��:				��ģ��SPI���߷�ʽ��������
//----------------------------------------------------------------------------------
uint8 SendOrRecByte_SPIF(uint8 ch)
{
	uint8 i,temp;   
	
    for (i=0;i<8;i++)
    {
		temp=ch&0x80;
    	ch=ch<<1;
      if(temp)
      {
      	WriteMOSIF( 1 );	
      	//WriteMOSIF( 1 );	

      	WriteSCKF ( 0 );
      	//WriteSCKF ( 0 );
          
      	WriteSCKF ( 1 );	 
        //WriteSCKF ( 1 );	        
          
      }
      else
      {
      	WriteMOSIF( 0 );	
      	//WriteMOSIF( 0 );	

        WriteSCKF ( 0 );
        //WriteSCKF ( 0 );
          
      	WriteSCKF ( 1 );
      	//WriteSCKF ( 1 );
      }
      
      ch |= ReadMISOF();
      
      delay_us(1);
    }
    
    return	ch;
}

void	SendByte_SPIF(uint8	ch)
{
	SendOrRecByte_SPIF(ch);
}

uint8	RecByte_SPIF(void)
{
	uint8		ch =0;
	return	SendOrRecByte_SPIF(ch);
}

/*********************************************************************************************************
** �������ƣ�SPIInit
** ����˵������ʼ��SPI
** �����������
** �����������
*********************************************************************************************************/
void  SPIInit (void)
{

}

/*********************************************************************************************************
** ��������: Send_Byte
** �������ܣ�SPI����һ���ֽ�����
** �������: data
** �������: ��
** �� �� ֵ����
*********************************************************************************************************/
void Send_Byte (INT8U data)
{
	SendByte_SPIF(data);    
}

/*********************************************************************************************************
** ��������:Get_Byte
** ��������:SPI�ӿڽ���һ���ֽ�����
** �������:��
** �������:��
*********************************************************************************************************/
INT8U Get_Byte (void)
{
	return	RecByte_SPIF();
}
 
/*********************************************************************************************************
** �û��ӿڲ�
** ���º�������ֲʱ�����޸�
*********************************************************************************************************/
/********************************************************************************************************
** ��������:MX25L1602_RD
** ��������:MX25L1602�Ķ�����,��ѡ���ID�Ͷ����ݲ���
** �������:
**          INT32U Dst��Ŀ���ַ,��Χ 0x0 - MAX_ADDR��MAX_ADDR = 0x1FFFFF��
**          INT32U NByte:  Ҫ��ȡ�������ֽ���
**          INT8U* RcvBufPt:���ջ����ָ��
** �������:�����ɹ��򷵻�OK,ʧ���򷵻�ERROR_0
** ����˵��:��ĳ������,ĳһ��ڲ�����Ч,���ڸ���ڲ�������Invalid���ò�����������
**********************************************************************************************************/
INT8U MX25L1602_RD(INT32U Dst, INT32U NByte,INT8U* RcvBufPt)
{
      INTX_DISABLE();             //��ȫ���ж�
    
      INT32U i = 0;
      if ((Dst+NByte > MAX_ADDR)||(NByte == 0))  return (ERROR_0);            /*  �����ڲ���                */
      
      CE_Low();
      Send_Byte(0x0B);                                                      /* ���Ͷ�����                   */
      Send_Byte(((Dst & 0xFFFFFF) >> 16));                                  /* ���͵�ַ��Ϣ:�õ�ַΪ3���ֽ� */
      Send_Byte(((Dst & 0xFFFF) >> 8));
      Send_Byte(Dst & 0xFF);
      Send_Byte(0xFF);                                                      /* ����һ�����ֽ��Զ�ȡ����     */
      for (i = 0; i < NByte; i++) {
        RcvBufPt[i] = Get_Byte();
      }
      CE_High();

      INTX_ENABLE();             //��ȫ���ж�
      
      return (OK);
}

/*********************************************************************************************************
** ��������:MX25L1602_RdID
** ��������:MX25L1602�Ķ�ID����,��ѡ���ID�Ͷ����ݲ���
** �������:
**          idtype IDType:ID���͡��û�����Jedec_ID,Dev_ID,Manu_ID������ѡ��
**          INT32U* RcvbufPt:�洢ID������ָ��
** �������:�����ɹ��򷵻�OK,ʧ���򷵻�ERROR_0
** ����˵��:������Ĳ���������Ҫ���򷵻�ERROR_0
*********************************************************************************************************/
INT8U MX25L1602_RdID(idtype IDType, INT32U* RcvbufPt)
{
      INT32U temp = 0;
      if (IDType == Jedec_ID) {
        CE_Low();
        Send_Byte(0x9F);                                                    /* ���Ͷ�JEDEC ID����(9Fh)      */
        temp = (temp | Get_Byte()) << 8;                                    /* ��������                     */
        temp = (temp | Get_Byte()) << 8;
        temp = (temp | Get_Byte());                                         /* �ڱ�����,temp��ֵӦΪ0xBF2541*/
        CE_High();
        *RcvbufPt = temp;
        return (OK);
      }
    
      if ((IDType == Manu_ID) || (IDType == Dev_ID) ) {
        CE_Low();
        Send_Byte(0x90);                                                    /* ���Ͷ�ID���� (90h or ABh)    */
        Send_Byte(0x00);                                                    /* ���͵�ַ                     */
        Send_Byte(0x00);                                                    /* ���͵�ַ                     */
        Send_Byte(IDType);                                                  /* ���͵�ַ - ����00H����01H    */
        temp = Get_Byte();                                                  /* ���ջ�ȡ�������ֽ�           */
        CE_High();
        *RcvbufPt = temp;
        return (OK);
      } else {
        return (ERROR_0);
      }
}

/*********************************************************************************************************
** ��������:MX25L1602_WR
** ��������:MX25L1602��д��������д1���Ͷ�����ݵ�ָ����ַ
** �������:
**          INT32U Dst��Ŀ���ַ,��Χ 0x0 - MAX_ADDR��MAX_ADDR = 0x1FFFFF��
**          INT8U* SndbufPt:���ͻ�����ָ��
**          INT32U NByte:Ҫд�������ֽ���
** �������:�����ɹ��򷵻�OK,ʧ���򷵻�ERROR_0
** ����˵��:��ĳ������,ĳһ��ڲ�����Ч,���ڸ���ڲ�������Invalid���ò�����������
*********************************************************************************************************/
INT8U MX25L1602_WR(INT32U Dst, INT8U* SndbufPt, INT32U NByte)
{

    INTX_DISABLE();             //��ȫ���ж�

    INT32U temp = 0,i = 0,StatRgVal = 0;
      if (( (Dst+NByte-1 > MAX_ADDR)||(NByte == 0) )) {
        return (ERROR_0);                                                     /*  �����ڲ���                */
      }
      CE_Low();
      Send_Byte(0x05);                                                      /* ���Ͷ�״̬�Ĵ�������         */
      temp = Get_Byte();                                                    /* ������õ�״̬�Ĵ���ֵ       */
      CE_High();
    
      CE_Low();
     Send_Byte(0x50);                                                      /* ʹ״̬�Ĵ�����д             */
//         Send_Byte(0x06);                                                    /* ����дʹ������               */
      CE_High();
      CE_Low();
      Send_Byte(0x01);                                                      /* ����д״̬�Ĵ���ָ��         */
      Send_Byte(0);                                                         /* ��0BPxλ��ʹFlashоƬȫ����д*/
      CE_High();
    
      for(i = 0; i < NByte; i++) {
////����20131126  ��������
//		printf(" %d,%d, ",Dst+i,Dst+i % 4096);
		if((Dst+i) % 4096 == 0)
		{
			MX25L1602_Erase((Dst+i)/4096,(Dst+i)/4096);
			printf("\r\n EraseEraseEraseErase");
		}
//////		
 
        CE_Low();
        Send_Byte(0x06);                                                    /* ����дʹ������               */
        CE_High();
    
        CE_Low();
        Send_Byte(0x02);                                                    /* �����ֽ�������д����         */
        Send_Byte((((Dst+i) & 0xFFFFFF) >> 16));                            /* ����3���ֽڵĵ�ַ��Ϣ        */
        Send_Byte((((Dst+i) & 0xFFFF) >> 8));
        Send_Byte((Dst+i) & 0xFF);
        Send_Byte(SndbufPt[i]);                                             /* ���ͱ���д������             */
        CE_High();
        
        u32 times = 10000;
        do {
          CE_Low();
          Send_Byte(0x05);                                                  /* ���Ͷ�״̬�Ĵ�������         */
          StatRgVal = Get_Byte();                                           /* ������õ�״̬�Ĵ���ֵ       */
          CE_High();
        } while ((StatRgVal == 0x03) && times--);                             /* һֱ�ȴ���ֱ��оƬ����       */
      }
    
      CE_Low();
      Send_Byte(0x06);                                                      /* ����дʹ������               */
      CE_High();
    
      CE_Low();
      Send_Byte(0x50);                                                      /* ʹ״̬�Ĵ�����д             */
      CE_High();
      CE_Low();
      Send_Byte(0x01);                                                      /* ����д״̬�Ĵ���ָ��         */
      Send_Byte(temp);                                                      /* �ָ�״̬�Ĵ���������Ϣ       */
      CE_High();
    
     INTX_ENABLE();             //��ȫ���ж�
      
      return (OK);
}

/*********************************************************************************************************
** ��������:MX25L1602_Erase
** ��������:����ָ����������ѡȡ���Ч���㷨����
** �������:
**          INT32U sec1����ʼ������,��Χ(0~499)
**          INT32U sec2����ֹ������,��Χ(0~499)
** �������:�����ɹ��򷵻�OK,ʧ���򷵻�ERROR_0
*********************************************************************************************************/
INT8U MX25L1602_Erase(INT32U sec1, INT32U sec2)
{
  INT8U  temp1 = 0,temp2 = 0,StatRgVal = 0;
  INT32U SecnHdAddr = 0;
  INT32U no_SecsToEr = 0;                                               /* Ҫ������������Ŀ             */
  INT32U CurSecToEr = 0;                                                /* ��ǰҪ������������           */

  /*
   *  �����ڲ���
   */
  if ((sec1 > SEC_MAX)||(sec2 > SEC_MAX)) {
    return (ERROR_0);
  }

  CE_Low();
  Send_Byte(0x05);                                                     /* ���Ͷ�״̬�Ĵ�������          */
  temp1 = Get_Byte();                                                  /* ������õ�״̬�Ĵ���ֵ        */
  CE_High();

  CE_Low();
//  Send_Byte(0x50);                                                     /* ʹ״̬�Ĵ�����д              */

  Send_Byte(0x06);
  CE_High();

  CE_Low();
  Send_Byte(0x01);                                                     /* ����д״̬�Ĵ���ָ��          */
  Send_Byte(0);                                                        /* ��0BPxλ��ʹFlashоƬȫ����д */
  CE_High();

  CE_Low();
  Send_Byte(0x06);                                                      /* ����дʹ������               */
  CE_High();

  /*
   * ����û��������ʼ�����Ŵ�����ֹ�����ţ������ڲ���������
   */
  if (sec1 > sec2)
  {
    temp2 = sec1;
    sec1  = sec2;
    sec2  = temp2;
  }
  /*
   * ����ֹ����������������������
   */
  if (sec1 == sec2)
  {
    SecnHdAddr = SEC_SIZE * sec1;                                       /* ������������ʼ��ַ           */
    CE_Low();
    Send_Byte(0x20);                                                    /* ������������ָ��             */
    Send_Byte(((SecnHdAddr & 0xFFFFFF) >> 16));                         /* ����3���ֽڵĵ�ַ��Ϣ        */
    Send_Byte(((SecnHdAddr & 0xFFFF) >> 8));
    Send_Byte(SecnHdAddr & 0xFF);
    CE_High();
      
    u32 times=10000;
    do {
      CE_Low();
      Send_Byte(0x05);                                                  /* ���Ͷ�״̬�Ĵ�������        */
      StatRgVal = Get_Byte();                                           /* ������õ�״̬�Ĵ���ֵ      */
      CE_High();
    } while ((StatRgVal & 0x01)  && times--);                                         /* һֱ�ȴ���ֱ��оƬ����      */
    return (OK);
  }

  /*
   * ������ʼ��������ֹ��������������ٵĲ�������
   */

  if (sec2 - sec1 == SEC_MAX) {
    CE_Low();
    Send_Byte(0x60);                                                    /* ����оƬ����ָ��(60h or C7h) */
    CE_High();
    u32 times = 10000;
    do {
      CE_Low();
      Send_Byte(0x05);                                                  /* ���Ͷ�״̬�Ĵ�������         */
      StatRgVal = Get_Byte();                                           /* ������õ�״̬�Ĵ���ֵ       */
      CE_High();
    } while ((StatRgVal & 0x01) && times--);                                         /* һֱ�ȴ���ֱ��оƬ����       */
    return (OK);
  }

  no_SecsToEr = sec2 - sec1 +1;                                         /* ��ȡҪ������������Ŀ         */
  CurSecToEr  = sec1;                                                   /* ����ʼ������ʼ����           */

  /*
   * ����������֮��ļ���������ȡ16���������㷨
   */
  while (no_SecsToEr >= 16)
  {
    CE_Low();
    Send_Byte(0x06);                                                     /* ��������д����              */
    CE_High();

    SecnHdAddr = SEC_SIZE * CurSecToEr;                                  /* ������������ʼ��ַ          */
    CE_Low();
    Send_Byte(0xD8);                                                     /* ����64KB�����ָ��          */
    Send_Byte(((SecnHdAddr & 0xFFFFFF) >> 16));                          /* ����3���ֽڵĵ�ַ��Ϣ       */
    Send_Byte(((SecnHdAddr & 0xFFFF) >> 8));
    Send_Byte(SecnHdAddr & 0xFF);
    CE_High();
    
    u32 times=10000;
    do {
      CE_Low();
      Send_Byte(0x05);                                                  /* ���Ͷ�״̬�Ĵ�������        */
      StatRgVal = Get_Byte();                                           /* ������õ�״̬�Ĵ���ֵ      */
      CE_High();
    } while ((StatRgVal & 0x01) && times--);                                         /* һֱ�ȴ���ֱ��оƬ����      */
    CurSecToEr  += 16;                                                  /* ���������16��������Ͳ�����*/
                                                                        /* �����ڵĴ�����������        */
    no_SecsToEr -=  16;                                                 /* �������������������������  */
  }

  /*
   * �������������㷨����ʣ�������
   */
  while (no_SecsToEr >= 1) {
    CE_Low();
    Send_Byte(0x06);                                                   /* ��������д����              */
    CE_High();

    SecnHdAddr = SEC_SIZE * CurSecToEr;                                /* ������������ʼ��ַ          */
    CE_Low();
    Send_Byte(0x20);                                                   /* ������������ָ��            */
    Send_Byte(((SecnHdAddr & 0xFFFFFF) >> 16));                        /* ����3���ֽڵĵ�ַ��Ϣ       */
    Send_Byte(((SecnHdAddr & 0xFFFF) >> 8));
    Send_Byte(SecnHdAddr & 0xFF);
    CE_High();
    
    u32 times = 10000;
    do {
      CE_Low();
      Send_Byte(0x05);                                                 /* ���Ͷ�״̬�Ĵ�������        */
      StatRgVal = Get_Byte();                                          /* ������õ�״̬�Ĵ���ֵ      */
      CE_High();
    } while ((StatRgVal & 0x01 ) && times--);                                       /* һֱ�ȴ���ֱ��оƬ����      */
    CurSecToEr  += 1;
    no_SecsToEr -=  1;
  }
  /*
   * ��������,�ָ�״̬�Ĵ�����Ϣ
   */
  CE_Low();
  Send_Byte(0x06);                                                    /* ����дʹ������               */
  CE_High();

  CE_Low();
  Send_Byte(0x50);                                                    /* ʹ״̬�Ĵ�����д             */
  CE_High();
  CE_Low();
  Send_Byte(0x01);                                                    /* ����д״̬�Ĵ���ָ��         */
  Send_Byte(temp1);                                                   /* �ָ�״̬�Ĵ���������Ϣ       */
  CE_High();
  return (OK);
}

#define		FLSH_ERR  36
uint8	flsherrflg = 0xff;
uint8 JudgeFlashIDErrFlg(void)   
{
	uint32_t 	GulChipID = 0;

//	if(GetSysTime() %100*30==1 || GetSysTime() < 60*100)
	{
	    MX25L1602_RdID(Jedec_ID, &GulChipID);		                        /* �������е��˴�ʱ,��IAR��鿴 */
        
        printf("\r\n 0x00C22017 Jedec_ID ==  %x",GulChipID);
	    GulChipID &= ~0xFF000000;                                           /* ��������24λ����             */            
	    if (GulChipID != 0x00C22017)//c22015 
		{
			flsherrflg = FLSH_ERR;
	   	}
		else
		{
			flsherrflg = 0xff;
		}
	}

	return 	flsherrflg;
}

/*********************************************************************************************************
** End Of File
*********************************************************************************************************/

