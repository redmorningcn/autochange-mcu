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

					  
//铁电参数
#define		FRAM_SIZE               8192						         	//1024*8空间容量
#define		FRAM_START_ADDR	        0x0000	                                //铁电的起始地址
#define		FRAM_DIVICE_ADDR        0xa0	                                //铁电的设备地址
#define		FRAM_END_ADDR	      	(FRAM_START_ADDR + FRAM_SIZE)	

#define		FRMA_PROTECT			121//P121


//#define     SCL                     206		//P0.15
//#define     SDA                     207		//P0.18

#define  BSP_GPIO_SDA                        GPIO_Pin_10
#define  BSP_GPIO_SCL                        GPIO_Pin_11


//------------------------------------------------------------------------
//  ??  3? ￡ovoid void InitI2CIO(void)
//  1|  ?ü ￡oéè?? SDA SCL ?a??í¨IO?ú ??DD?￡?a×ü??2ù×÷
// è??ú2?êy￡o?T
// 3??ú2?êy￡o?T
//------------------------------------------------------------------------
void InitI2CIO(void)
{
//    SETFUN_P00_15X( GPIO,SCL );         //??I2C???úéè?aGPIO?ú
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
//  ??  3? ￡ovoid WriteSCL( uint8 temp )
//  1|  ?ü ￡oéè?? SCL
// è??ú2?êy￡o?T
// 3??ú2?êy￡o?T
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
//  ??  3? ￡ouint8 ReadSCL(void)
//  1|  ?ü ￡o?áè? SCL μ??μ
// è??ú2?êy￡o?T
// 3??ú2?êy￡o?T
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
//  ??  3? ￡ovoid WriteSDA( uint8 temp )
//  1|  ?ü ￡oéè?? SDA
// è??ú2?êy￡o?T
// 3??ú2?êy￡o?T
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
//  ??  3? ￡ouint8 ReadSDA(void)
//  1|  ?ü ￡o?áè? SDA μ??μ
// è??ú2?êy￡o?T
// 3??ú2?êy￡o?T
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
//oˉêy??3?:         StartI2C()
//1|    ?ü:         I2C×ü???aê?ì??t
//è??ú2?êy:         ?T
//3??ú2?êy:         ?T
//?μ?÷￡o            I2C×ü??μ??aê?ì??t￡o
//                  SCL??μ???ê±SDAμ????μ??
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
//oˉêy??3?:         StopI2C()
//1|    ?ü:         I2C×ü???áê?ì??t
//è??ú2?êy:         ?T
//3??ú2?êy:         ?T
//?μ?÷￡o            I2C×ü???áê?ì??t￡oSCL?a??μ???ê±SDAμ?é?éy??
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
//oˉêy??3?:         MasterAckI2C()
//1|    ?ü:         ?÷?ú?óê?ó|′e￡??ú?óê?μ?1×??úêy?Yoó￡?·μ??ó|′e￡?????′ó?ú?óê?íê3é
//è??ú2?êy:         ?T
//3??ú2?êy:         ?T
//?μ?÷￡o            I2C×ü??ó|′e￡o?áíêò???×??úoó??SDA??à-μí￡??′ó|′e0
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
//oˉêy??3?:         MasterNoAckI2C()
//1|    ?ü:         ?÷?ú?óê?ó|′e￡?????í¨D?′ó?ú?óê?íê3é
//è??ú2?êy:         ?T
//3??ú2?êy:         ?T
//?μ?÷￡o            I2C×ü??ó|′e￡o
//                  ?áíê?ùòa?óμ?×??úêy?Yoó??SDA??à-??￡??a?áê?IIC×ü??2ù×÷×?×?±?
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
//oˉêy??3?:         CheckSlaveAckI2C()
//1|    ?ü:         ?áI2C′ó?úó|′e
//è??ú2?êy:         ?T
//3??ú2?êy:         ackflag
//                  è?1?′ó?ú2úéúó|′e￡??ò·μ??0￡?
//                  ′ó?ú??óD2úéúó|′e,·??ò·μ??1
//?μ?÷￡o            ?ì2éI2C?÷?t(′ó?ú)ó|′e?é??,ó|′e·μ??1,·??ò?aá?
//these waw a bug befor 2008/05/17.
//------------------------------------------------------------------------------------
uint8 CheckSlaveAckI2C(void)
{
    uint8 ackflag;                                  //??3yó|′e±ê??;
    
    ackflag = ReadSDA();
    WriteSCL(1);                                    //SCL=1;
    WriteSCL(0);                                    //SCL=0;
    
    return ackflag;
}

//------------------------------------------------------------------------------------
//oˉêy??3?:         WriteByteWithI2C()
//1|    ?ü:         ?÷?ú??I2C×ü??D′12ù×÷
//è??ú2?êy:         Data:òaD′3?μ?êy?Y
//3??ú2?êy:         ?T
//?μ?÷￡o            D′ò?×??úêy?YData
//------------------------------------------------------------------------------------
void WriteByteWithI2C(uint8 Data)  
{
    uint8 i;
    for(i=0;i<8;i++)
    {
        if(Data & 0x80)                             //′?ê???′ó????μ?μí??,?è′?×????? 
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
        Data <<= 1;                                 //êy?Y×óò?  
    //    delay_us(1);
    }
}

//-------------------------------------------------------------------------------------------------------
//oˉêy??3?:         ReadByteWithI2C()
//1|    ?ü:         ?÷?ú??I2C×ü??D′12ù×÷
//è??ú2?êy:         ?T
//3??ú2?êy:         ?T
//?μ?÷￡o            ?áò?×??úêy?Y￡o·μ???μ?′?áμ?êy?Y
//--------------------------------------------------------------------------------------------------------
uint8 ReadByteWithI2C(void)            
{
    uint8 readdata=0;                           // Read_Data?a?áè?μ?êy?Y￡?0
    uint8 j;
    
    for (j=0;j<8;j++)
    {
        readdata <<=1;
                                                //êy?Y?á3?￡?è?ê?0?òò???￡?è?ê?1?òó?0x01?ò
        if(ReadSDA())
        {
            readdata =(readdata | 0x01);               
        }
        WriteSCL(1);                              //SCL=1;
        WriteSCL(0);                              //SCL=0;
    //    delay_us(1);
    }

    
    return   readdata ;                         //·μ???á3?μ?×??ú
}




//-------------------------------------------------------------------------------------------------------
//函数名称:         WriteFM24CL16()
//功    能:         对FM24CL16指定地址进行写数据
//入口参数:         ADDR:       操作地址    地址：0 ~ sizeof(FM24CL16)
//                  *DataBuf:   数据缓冲
//                  Len:        数据长度
//出口参数:         无
//说明：            
//--------------------------------------------------------------------------------------------------------
uint8 WriteFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen)            
{
    uint32  i = 0;
    uint8   SlaveAddr;                                      //从机地址
    uint16  AddrTemp = 0;                                   //地址缓存
    
    INTX_DISABLE();                                         //关全局中断

    
//    WriteFRAM_PROTECT(0);									//关写保护
    
    AddrTemp = Addr;
    AddrTemp += FRAM_START_ADDR;                            //计算地址

    if( (uint16)AddrTemp + DataBufLen > FRAM_END_ADDR ) 
    {
        return  0;                                          //地址未超阶
    }
        
    SlaveAddr = FRAM_DIVICE_ADDR & (~(0x01));               //求FM24CL64设备写地址
    
    StartI2C();                                             //启动IIC总线
    
    WriteByteWithI2C(SlaveAddr);                            //写FM24CL64地址
    CheckSlaveAckI2C();
 
    WriteByteWithI2C(Addr >> 8);                            //写FM24CL64高位地址
    CheckSlaveAckI2C();
    
    WriteByteWithI2C( Addr );                               //写FM24CL64低位地址
    CheckSlaveAckI2C();     
  
    for(i = 0; i < DataBufLen; i++)                         //发送数据
    {
        WriteByteWithI2C(DataBuf[i]);                       //写数据
        CheckSlaveAckI2C();                                 //读从机应答信号
    }
    
    StopI2C();                                              //停止IIC操作   

    INTX_ENABLE();                                         //关全局中断
    
    return  1;                                              //正确
}

//-------------------------------------------------------------------------------------------------------
//函数名称:         ReadFM24CL16()
//功    能:         读取FM24CL16指定地址进行数据
//入口参数:         ADDR:       操作地址   地址：0 ~ sizeof(FM24CL16)
//                  *DataBuf:   数据缓冲 
//                  Len:        数据长度
//出口参数:         无
//说明：            
//--------------------------------------------------------------------------------------------------------
uint8 ReadFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen)            
{
    uint32  i = 0;
    uint8   SlaveAddr;                                      //从机地址
    uint16  AddrTemp;                                       //地址缓存
    
//	WriteFRAM_PROTECT(0);									//关写保护

    INTX_DISABLE();                                         //关全局中断

    AddrTemp = Addr;
    
    AddrTemp += FRAM_START_ADDR;                            //计算地址
    
    if( (uint16)AddrTemp + DataBufLen > FRAM_END_ADDR ) 
    {
        return  0;                                      //地址未超阶
    }
    
    SlaveAddr = FRAM_DIVICE_ADDR & (~(0x01));               //求FM24CL64设备写地址
    
    StartI2C();                                             //启动IIC总线
    
    WriteByteWithI2C(SlaveAddr);                            //写FM24CL64设备写地址
    CheckSlaveAckI2C();                                     //读从机应答信号
    
    WriteByteWithI2C(Addr >> 8 );                           //写数据高位地址
    CheckSlaveAckI2C();                                     //读从机应答信号
    
    WriteByteWithI2C( Addr );                               //写数据低位地址
    CheckSlaveAckI2C();                                     //读从机应答信号
        
    SlaveAddr = (FRAM_DIVICE_ADDR)|(0x01);                  //求FM24CL64设备读地址
    
    StartI2C();                                             //启动IIC总线
    
    WriteByteWithI2C(SlaveAddr);                            //写FM24CL16地址
    CheckSlaveAckI2C();                                     //读从机应答信号
   
    for(i = 0; i < (DataBufLen -1); i++)                    //发送数据
    {
        DataBuf[i] = ReadByteWithI2C();                     //写数据
        MasterAckI2C();                                     //主机应答从机
    }
    DataBuf[i] = ReadByteWithI2C();                         //写数据
    
    MasterNoAckI2C();                                       //主机应答从机
    
    StopI2C();
    
    INTX_ENABLE();                                         //关全局中断
    
    return  1;                                           //正确
}

//#endif
    
