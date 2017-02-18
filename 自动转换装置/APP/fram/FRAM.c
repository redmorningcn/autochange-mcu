//#include    "config.h"   
//#include    "CpuDrive.h"

//#include <includes.h>
#include "stdint.h"

#include  <stdarg.h>
#include  <stdio.h>
#include  <math.h>
#if defined(USE_HAL_DRIVER)
#include  <stm32f4xx_hal.h>
#else
#include  <stm32f4xx.h>
#endif

					  
//�������
#define		FRAM_SIZE               8192						         	//1024*8�ռ�����
#define		FRAM_START_ADDR	        0x0000	                                //�������ʼ��ַ
#define		FRAM_DIVICE_ADDR        0xa0	                                //������豸��ַ
#define		FRAM_END_ADDR	      	(FRAM_START_ADDR + FRAM_SIZE)	

#define		FRMA_PROTECT			121//P121


//#define     SCL                     206		//P0.15
//#define     SDA                     207		//P0.18

#define  BSP_GPIO_SDA                        GPIO_Pin_10
#define  BSP_GPIO_SCL                        GPIO_Pin_11


//------------------------------------------------------------------------
//  ??  3? ��ovoid void InitI2CIO(void)
//  1|  ?�� ��o����?? SDA SCL ?a??����IO?�� ??DD?��?a����??2������
// ��??��2?��y��o?T
// 3??��2?��y��o?T
//------------------------------------------------------------------------
void InitI2CIO(void)
{
//    SETFUN_P00_15X( GPIO,SCL );         //??I2C???������?aGPIO?��
//    SETFUN_P00_15X( GPIO,SDA ); 
}

void	InitFramIO(void)
{
//	GPIO_PinselConfig(SCL,0);
//	GPIO_PinselConfig(SCL,0);
//	GPIO_PinselConfig(SDA,0);
//	GPIO_PinselConfig(SDA,0);
    
    GPIO_InitTypeDef  gpio_init;

    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_25MHz;
    
    gpio_init.GPIO_Pin   = BSP_GPIO_SCL 
                         | BSP_GPIO_SDA; 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);;                          /* Configure GPIOG for LED1 and LED2                    */
    GPIO_Init(GPIOC, &gpio_init);
    
}

//------------------------------------------------------------------------
//  ??  3? ��ovoid WriteSCL( uint8 temp )
//  1|  ?�� ��o����?? SCL
// ��??��2?��y��o?T
// 3??��2?��y��o?T
//------------------------------------------------------------------------
void WriteSCL(uint8 temp)
{	
    GPIO_InitTypeDef GPIO_InitStruct;
 
    GPIO_InitStruct.GPIO_Pin     = BSP_GPIO_SCL;
    GPIO_InitStruct.GPIO_Mode    = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_PuPd    = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed   = GPIO_Speed_100MHz;

    GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    delay_us(1);

	if(temp)		
	{
        GPIO_WriteBit(GPIOC, BSP_GPIO_SCL, Bit_SET);    
	}
	else
	{
		//GPIO_SetOrClearValue(SCL,0);		
        GPIO_WriteBit(GPIOC, BSP_GPIO_SCL, Bit_RESET);    
	}
    //delay_us(10);
	//nop();	
	  
//    IO0DIR_OUT(SCL);    
//    IO0PIN_W(temp,SCL);
}

//------------------------------------------------------------------------
//  ??  3? ��ouint8 ReadSCL(void)
//  1|  ?�� ��o?����? SCL ��??��
// ��??��2?��y��o?T
// 3??��2?��y��o?T
//------------------------------------------------------------------------
uint8 ReadSCL(void)
{
    uint8 temp;
//    
//    IO0DIR_IN(SCL);    
//    temp = IO0PIN_R(SCL);
//    

//	temp = 	GPIO_ReadIoVal(SCL);
//	nop();
    
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = BSP_GPIO_SCL;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    delay_us(1);

    temp = GPIO_ReadInputDataBit( GPIOC,  BSP_GPIO_SCL); 
    
    //delay_us(10);

    return temp;
}

//------------------------------------------------------------------------
//  ??  3? ��ovoid WriteSDA( uint8 temp )
//  1|  ?�� ��o����?? SDA
// ��??��2?��y��o?T
// 3??��2?��y��o?T
//------------------------------------------------------------------------
void WriteSDA(uint8 temp)
{
//    IO0DIR_OUT(SDA);    
//    IO0PIN_W(temp,SDA);
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin     = BSP_GPIO_SDA;
    GPIO_InitStruct.GPIO_Mode    = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_PuPd    = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed   = GPIO_Speed_100MHz;

    GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    delay_us(1);
    
	if(temp)		
	{
        GPIO_WriteBit(GPIOC, BSP_GPIO_SDA, Bit_SET);    
	}
	else
	{
        GPIO_WriteBit(GPIOC, BSP_GPIO_SDA, Bit_RESET);    
	}	
    
    //delay_us(10);
}

//------------------------------------------------------------------------
//  ??  3? ��ouint8 ReadSDA(void)
//  1|  ?�� ��o?����? SDA ��??��
// ��??��2?��y��o?T
// 3??��2?��y��o?T
//------------------------------------------------------------------------
uint8 ReadSDA(void)
{
    uint8 temp;
    
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = BSP_GPIO_SDA;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    delay_us(1);

    temp = GPIO_ReadInputDataBit( GPIOC,  BSP_GPIO_SDA); 
    
   // delay_us(10);

    return temp;
}

//------------------------------------------------------------
//o����y??3?:         StartI2C()
//1|    ?��:         I2C����???a��?��??t
//��??��2?��y:         ?T
//3??��2?��y:         ?T
//?��?�¡�o            I2C����??��??a��?��??t��o
//                  SCL??��???����SDA��????��??
//------------------------------------------------------------
void StartI2C(void)
{
    WriteSCL(0);                                // SCL=0;
    
    WriteSDA(1);                                //SDA=1;
    WriteSCL(1);                                //SCL=1;
    WriteSDA(0);                                //SDA=0;
    WriteSCL(0);                                // SCL=0;
}

//---------------------------------------------------------------
//o����y??3?:         StopI2C()
//1|    ?��:         I2C����???����?��??t
//��??��2?��y:         ?T
//3??��2?��y:         ?T
//?��?�¡�o            I2C����???����?��??t��oSCL?a??��???����SDA��?��?��y??
//--------------------------------------------------------------------
void StopI2C(void)
{
    WriteSCL(0);                                // SCL=0;
    
    WriteSDA(0);                                //SDA=0;
    WriteSCL(1);                                //SCL=1;
    WriteSDA(1);                                //SDA=1;
    WriteSCL(0);                                //SCL=0;
}

//-------------------------------------------------------------------------------
//o����y??3?:         MasterAckI2C()
//1|    ?��:         ?��?��?����?��|��e��??��?����?��?1��??����y?Yo����?����??��|��e��?????�䨮?��?����?����3��
//��??��2?��y:         ?T
//3??��2?��y:         ?T
//?��?�¡�o            I2C����??��|��e��o?��������???��??��o��??SDA??��-�̨���??�䨮|��e0
//---------------------------------------------------------------------------------
void MasterAckI2C(void)
{
    WriteSCL(0);                                // SCL=0;

    WriteSDA(0);                                //SDA=0;
    WriteSCL(1);                                //SCL=1;
    WriteSCL(0);                                  //SCL=0;
    WriteSDA(0);                                //SDA=0;
}

//-----------------------------------------------------------------------------------
//o����y??3?:         MasterNoAckI2C()
//1|    ?��:         ?��?��?����?��|��e��?????����D?�䨮?��?����?����3��
//��??��2?��y:         ?T
//3??��2?��y:         ?T
//?��?�¡�o            I2C����??��|��e��o
//                  ?������?����a?����?��??����y?Yo��??SDA??��-??��??a?����?IIC����??2�����¡�?��?��?
//-------------------------------------------------------------------------------------
void MasterNoAckI2C(void)
{
    WriteSCL(0);                                    // SCL=0;
    
    WriteSDA(1);                                    //SDA=1;
    WriteSCL(1);                                    //SCL=1;
    WriteSCL(0);                                    //SCL=0;
    WriteSDA(0);                                    //SDA=0;
}

//------------------------------------------------------------------------------------
//o����y??3?:         CheckSlaveAckI2C()
//1|    ?��:         ?��I2C�䨮?����|��e
//��??��2?��y:         ?T
//3??��2?��y:         ackflag
//                  ��?1?�䨮?��2��������|��e��??������??0��?
//                  �䨮?��??��D2��������|��e,��??������??1
//?��?�¡�o            ?��2��I2C?��?t(�䨮?��)��|��e?��??,��|��e����??1,��??��?a��?
//these waw a bug befor 2008/05/17.
//------------------------------------------------------------------------------------
uint8 CheckSlaveAckI2C(void)
{
    uint8 ackflag;                                  //??3y��|��e����??;
    
    ackflag = ReadSDA();
    WriteSCL(1);                                    //SCL=1;
    WriteSCL(0);                                    //SCL=0;
    
    return ackflag;
}

//------------------------------------------------------------------------------------
//o����y??3?:         WriteByteWithI2C()
//1|    ?��:         ?��?��??I2C����??D��12������
//��??��2?��y:         Data:��aD��3?��?��y?Y
//3??��2?��y:         ?T
//?��?�¡�o            D�䨰?��??����y?YData
//------------------------------------------------------------------------------------
void WriteByteWithI2C(uint8 Data)  
{
    uint8 i;
    for(i=0;i<8;i++)
    {
        if(Data & 0x80)                             //��?��???�䨮????��?�̨�??,?����?��????? 
        {
            WriteSDA(1);                            //SDA=1;
            WriteSCL(1);                            //SCL=1;
        }
        else
        {  
            WriteSDA(0);                            //SDA=0;
            WriteSCL(1);                            //SCL=1;
        }   
        WriteSCL(0);                                //SCL=0;
        WriteSDA(0);                                //SDA=0;
        Data <<= 1;                                 //��y?Y������?  
    //    delay_us(1);
    }
}

//-------------------------------------------------------------------------------------------------------
//o����y??3?:         ReadByteWithI2C()
//1|    ?��:         ?��?��??I2C����??D��12������
//��??��2?��y:         ?T
//3??��2?��y:         ?T
//?��?�¡�o            ?����?��??����y?Y��o����???��?��?����?��y?Y
//--------------------------------------------------------------------------------------------------------
uint8 ReadByteWithI2C(void)            
{
    uint8 readdata=0;                           // Read_Data?a?����?��?��y?Y��?0
    uint8 j;
    
    for (j=0;j<8;j++)
    {
        readdata <<=1;
                                                //��y?Y?��3?��?��?��?0?����???��?��?��?1?����?0x01?��
        if(ReadSDA())
        {
            readdata =(readdata | 0x01);               
        }
        WriteSCL(1);                              //SCL=1;
        WriteSCL(0);                              //SCL=0;
    //    delay_us(1);
    }

    
    return   readdata ;                         //����???��3?��?��??��
}




//-------------------------------------------------------------------------------------------------------
//��������:         WriteFM24CL16()
//��    ��:         ��FM24CL16ָ����ַ����д����
//��ڲ���:         ADDR:       ������ַ    ��ַ��0 ~ sizeof(FM24CL16)
//                  *DataBuf:   ���ݻ���
//                  Len:        ���ݳ���
//���ڲ���:         ��
//˵����            
//--------------------------------------------------------------------------------------------------------
uint8 WriteFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen)            
{
    uint32  i = 0;
    uint8   SlaveAddr;                                      //�ӻ���ַ
    uint16  AddrTemp = 0;                                   //��ַ����
    
    INTX_DISABLE();                                         //��ȫ���ж�

    
//    WriteFRAM_PROTECT(0);									//��д����
    
    AddrTemp = Addr;
    AddrTemp += FRAM_START_ADDR;                            //�����ַ

    if( (uint16)AddrTemp + DataBufLen > FRAM_END_ADDR ) 
    {
        return  0;                                          //��ַδ����
    }
        
    SlaveAddr = FRAM_DIVICE_ADDR & (~(0x01));               //��FM24CL64�豸д��ַ
    
    StartI2C();                                             //����IIC����
    
    WriteByteWithI2C(SlaveAddr);                            //дFM24CL64��ַ
    CheckSlaveAckI2C();
 
    WriteByteWithI2C(Addr >> 8);                            //дFM24CL64��λ��ַ
    CheckSlaveAckI2C();
    
    WriteByteWithI2C( Addr );                               //дFM24CL64��λ��ַ
    CheckSlaveAckI2C();     
  
    for(i = 0; i < DataBufLen; i++)                         //��������
    {
        WriteByteWithI2C(DataBuf[i]);                       //д����
        CheckSlaveAckI2C();                                 //���ӻ�Ӧ���ź�
    }
    
    StopI2C();                                              //ֹͣIIC����   

    INTX_ENABLE();                                         //��ȫ���ж�
    
    return  1;                                              //��ȷ
}

//-------------------------------------------------------------------------------------------------------
//��������:         ReadFM24CL16()
//��    ��:         ��ȡFM24CL16ָ����ַ��������
//��ڲ���:         ADDR:       ������ַ   ��ַ��0 ~ sizeof(FM24CL16)
//                  *DataBuf:   ���ݻ��� 
//                  Len:        ���ݳ���
//���ڲ���:         ��
//˵����            
//--------------------------------------------------------------------------------------------------------
uint8 ReadFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen)            
{
    uint32  i = 0;
    uint8   SlaveAddr;                                      //�ӻ���ַ
    uint16  AddrTemp;                                       //��ַ����
    
//	WriteFRAM_PROTECT(0);									//��д����

    INTX_DISABLE();                                         //��ȫ���ж�

    AddrTemp = Addr;
    
    AddrTemp += FRAM_START_ADDR;                            //�����ַ
    
    if( (uint16)AddrTemp + DataBufLen > FRAM_END_ADDR ) 
    {
        return  0;                                      //��ַδ����
    }
    
    SlaveAddr = FRAM_DIVICE_ADDR & (~(0x01));               //��FM24CL64�豸д��ַ
    
    StartI2C();                                             //����IIC����
    
    WriteByteWithI2C(SlaveAddr);                            //дFM24CL64�豸д��ַ
    CheckSlaveAckI2C();                                     //���ӻ�Ӧ���ź�
    
    WriteByteWithI2C(Addr >> 8 );                           //д���ݸ�λ��ַ
    CheckSlaveAckI2C();                                     //���ӻ�Ӧ���ź�
    
    WriteByteWithI2C( Addr );                               //д���ݵ�λ��ַ
    CheckSlaveAckI2C();                                     //���ӻ�Ӧ���ź�
        
    SlaveAddr = (FRAM_DIVICE_ADDR)|(0x01);                  //��FM24CL64�豸����ַ
    
    StartI2C();                                             //����IIC����
    
    WriteByteWithI2C(SlaveAddr);                            //дFM24CL16��ַ
    CheckSlaveAckI2C();                                     //���ӻ�Ӧ���ź�
   
    for(i = 0; i < (DataBufLen -1); i++)                    //��������
    {
        DataBuf[i] = ReadByteWithI2C();                     //д����
        MasterAckI2C();                                     //����Ӧ��ӻ�
    }
    DataBuf[i] = ReadByteWithI2C();                         //д����
    
    MasterNoAckI2C();                                       //����Ӧ��ӻ�
    
    StopI2C();
    
    INTX_ENABLE();                                         //��ȫ���ж�
    
    return  1;                                           //��ȷ
}

//#endif
    
