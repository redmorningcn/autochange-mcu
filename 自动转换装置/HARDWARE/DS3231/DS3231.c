//#include    "config.h"
//#include    "string.h"
//#include    "CpuDrive.h"
//#include    "HardDrive.h"

//#include <includes.h>
#include "DS3231.h"

#define     DS3231_ADDR     		0xd0
#define     DS3231_SEC      		0x00
#define     DS3231_MIN      		0x01
#define     DS3231_HOUR     		0x02
#define     DS3231_DAY      		0x03
#define     DS3231_DATA     		0x04
#define     DS3231_MONTH    		0x05
#define     DS3231_YEAR     		0x06

#define     DS3231_CONTROL      	0x0E
#define     DS3231_STATUS       	0x0F
#define     DS3231_AGING_OFFSET     0x10
#define     DS3231_TEMP_MSB     	0x11
#define     DS3231_TEMP_LSB     	0x12

//-------------------------------------------------------------------------------------------------------
//��������:         uint8 BCD2HEX(uint8 Bcd)  
//��    ��:         ��BCD���Ϊ��������
//��ڲ���:         BCD������  
//���ڲ���:         ����������
//˵����            
//--------------------------------------------------------------------------------------------------------
uint8 BCD2HEX(uint8 Bcd)  
{
    uint8   Hex;
    Hex = (Bcd & 0x0f) + ((Bcd>>4) & 0x0f)*10;
    
    return Hex;
}

//-------------------------------------------------------------------------------------------------------
//��������:         uint8 BCD2HEX(uint8 Bcd)  
//��    ��:         ��BCD���Ϊ��������
//��ڲ���:         BCD������  
//���ڲ���:         ����������
//˵����            
//--------------------------------------------------------------------------------------------------------
uint8 HEX2BCD(uint8 Hex)  
{
    uint8   Bcd;
    
    Bcd = Hex %10 + (((Hex/10)%10)<<4);
    
    return Bcd;
}

//-------------------------------------------------------------------------------------------------------
//��������:         ReadDS3231Byte()
//��    ��:         ��DS3231�ж���һ���ֽ�
//��ڲ���:         DS3231�豸�ţ� ��ȡ���ݵĵ�ַ
//���ڲ���:         ���ض�����ֵ
//˵����            
//--------------------------------------------------------------------------------------------------------
uint8 ReadDS3231Byte(uint8 addr)  
{
    uint8   SlaveAddr;
    uint8   RecData;	
    
    SlaveAddr = DS3231_ADDR & (~(0x01));            //�ӻ���ַ
    
    INTX_DISABLE();             //��ȫ���ж�

    
    StartI2C();                                     //����IIC����
   
    WriteByteWithI2C(SlaveAddr);                    //дDS3231��ȡ
    
    CheckSlaveAckI2C();                             //���ӻ�Ӧ���ź�
    
    WriteByteWithI2C(addr);                         //дDS3231��ȡ

    CheckSlaveAckI2C();                             //���ӻ�Ӧ���ź�

    StartI2C();                                     //����IIC����
    
    SlaveAddr = DS3231_ADDR | 0x01;             	//�ӻ���ַ

    WriteByteWithI2C(SlaveAddr);                    //дDS3231�豸��ַ

    CheckSlaveAckI2C();                             //���ӻ�Ӧ���ź�

    RecData = ReadByteWithI2C();                	//���յ�������
    
    MasterNoAckI2C();                               //����Ӧ��ӻ�
    
    StopI2C();                          			//����IIC����
    
    INTX_ENABLE();             //��ȫ���ж�

    
    return  RecData;
}    

//-------------------------------------------------------------------------------------------------------
//��������:         void WriteDS3231Byte(uint8 addr,uint8   Data)  
//��    ��:         дһ���ֽڵ�DS3231��
//��ڲ���:         addr:DS3231��ַ�� Data:Ҫд�������
//���ڲ���:         ���ض�����ֵ
//--------------------------------------------------------------------------------------------------------
void WriteDS3231Byte(uint8 Addr,uint8   Data)  
{
    uint8   SlaveAddr;
    
    StartI2C();                                     //����IIC����
    
    SlaveAddr = DS3231_ADDR & (~(0x01));            //�ӻ���ַ
   
    WriteByteWithI2C(SlaveAddr);                    //дDS3231��ȡ
    
    CheckSlaveAckI2C();                             //���ӻ�Ӧ���ź�
    
    WriteByteWithI2C(Addr);                         //дDS3231������ַ
    
    CheckSlaveAckI2C();                             //���ӻ�Ӧ���ź�
    
    WriteByteWithI2C(Data);                         //дDS3231����
    
    CheckSlaveAckI2C();                             //���ӻ�Ӧ���ź�
    
    StopI2C();                          			//����IIC����
}      

//-------------------------------------------------------------------------------------------------------
//��������:         stcTime     ReadTime(void)  
//��    ��:         ��DS3231�ж���ʱ��
//��ڲ���:         ���Ƕ�������ʱ������
//���ڲ���:         ���ض�����ʱ��
//--------------------------------------------------------------------------------------------------------
uint8	ReadTime(stcTime  *  sTime)  
{
    uint8   	Sec;
    uint8   	Min;
    uint8       Hour;	
    uint8       Data;
    uint8       Month;
    uint8       Year;
    	
    Sec     = ReadDS3231Byte(DS3231_SEC); 
    Min     = ReadDS3231Byte(DS3231_MIN);  
    Hour    = ReadDS3231Byte(DS3231_HOUR);  
    Data    = ReadDS3231Byte(DS3231_DATA); 
    Month   = ReadDS3231Byte(DS3231_MONTH); 
    Year    = ReadDS3231Byte(DS3231_YEAR);    
    	

    sTime->Sec   = BCD2HEX(Sec); 				
    sTime->Min   = BCD2HEX(Min); 				
    sTime->Hour  = BCD2HEX(Hour);    			
    sTime->Date  = BCD2HEX(Data);    			
    sTime->Month = BCD2HEX(Month);   			
    sTime->Year  = BCD2HEX(Year);		
        		
    return  1;
}

//-------------------------------------------------------------------------------------------------------
//��������:         stcTime     ReadTime(void)  
//��    ��:         ��DS3231�ж���ʱ��
//��ڲ���:         ��
//���ڲ���:         ���ض�����ʱ��
//˵����            
//--------------------------------------------------------------------------------------------------------
uint8   ReadTemp(void)  
{
    int16       Temp;
    int8        TempMsb;
    uint8       TempLsb;
    	
    TempMsb     = ReadDS3231Byte(DS3231_TEMP_MSB);      //����λ
    TempLsb     = ReadDS3231Byte(DS3231_TEMP_LSB);      //����λ
    
    Temp = TempMsb * 4 + (TempLsb>>6);

    printf("\r\nTemp = %d,TempMsb = %d,TempLsb = %d,",Temp,TempMsb,TempLsb);
    		
    return  Temp;
}

//-------------------------------------------------------------------------------------------------------
//��������:         stcTime     ReadTime(void)  
//��    ��:         дDS3231ʱ��
//��ڲ���:         дʱ��
//���ڲ���:         
//--------------------------------------------------------------------------------------------------------
void    WriteTime(stcTime  sTime)  
{
    uint8   	Sec;
    uint8   	Min;
    uint8       Hour;	
    uint8       Data;
    uint8       Month;
    uint8       Year;
    	
    Sec     = HEX2BCD(sTime.Sec);		
    Min     = HEX2BCD(sTime.Min);		
    Hour    = HEX2BCD(sTime.Hour);	
    Data    = HEX2BCD(sTime.Date);		
    Month   = HEX2BCD(sTime.Month);		
    Year    = HEX2BCD(sTime.Year);				

    WriteDS3231Byte(DS3231_SEC,Sec);     
    WriteDS3231Byte(DS3231_MIN,Min);        
    WriteDS3231Byte(DS3231_HOUR,Hour);
    WriteDS3231Byte(DS3231_DATA,Data);    
    WriteDS3231Byte(DS3231_MONTH,Month);  
    WriteDS3231Byte(DS3231_YEAR,Year);         

	DisplayTime();
	PrintfTime();
}

//-------------------------------------------------------------------------------------------------------
//��������:         void    SetTime(uint8 Year,uint8 Month,uint8 Data,uint8 Hour,uint8 Min,uint8 Sec)   
//��    ��:         ����ʱ��
//��ڲ���:         
//���ڲ���:         
//˵����            
//--------------------------------------------------------------------------------------------------------
void    SetTime(uint8 Year,uint8 Month,uint8 Date,uint8 Hour,uint8 Min,uint8 Sec)  
{
    stcTime  sTime;

    sTime.Sec   = Sec;  				
    sTime.Min   = Min;  				
    sTime.Hour  = Hour; 			
    sTime.Date  = Date; 			
    sTime.Month = Month;    			
    sTime.Year  = Year;	
    
    WriteTime(sTime);	
}

//-------------------------------------------------------------------------------------------------------
//��������:         void    InitDS3231(void)  
//��    ��:         ��ʼ��DS3231�豸ģʽ
//��ڲ���:         ��
//���ڲ���:         ��
//--------------------------------------------------------------------------------------------------------
void    InitDS3231(void)  
{
    InitI2CIO();
    
    WriteDS3231Byte(DS3231_CONTROL,0);     
    
    WriteDS3231Byte(DS3231_STATUS,0);       

    PrintfTime();    
}   

//-------------------------------------------------------------------------------------------------------
//��������:  	void    PrintfTime(void)  
//��    ��:  	��ʱ������-��-�� ʱ���֣��� ����
//��ڲ���:         
//���ڲ���:         
//--------------------------------------------------------------------------------------------------------
void    PrintfTime(void)  
{
	static uint32	Time = 0;
	stcTime		sTime;
	
//	if(GetSysTime() - Time > 100)
	{
//		Time = GetSysTime();
		
		ReadTime((stcTime *)&sTime);
		
		printfcom1("\r\n%u-%u-%u  %u:%u:%u",
		sTime.Year,sTime.Month,sTime.Date,sTime.Hour,sTime.Min,sTime.Sec);
	}
}

//-------------------------------------------------------------------------------------------------------
//��������:  	void    DisplayTime(void)  
//��    ��:  	��ʱ������-��-�� ʱ���֣��� ����
//��ڲ���:         
//���ڲ���:         						DataDea
//--------------------------------------------------------------------------------------------------------
void    DisplayTime(void)  
{
	stcTime		sTime;

	ReadTime((stcTime *)&sTime);
	//DisplaySet(60,1,"   ");

	//DisplaySet(100,1,"%02u-%02u-%02u",sTime.Year,sTime.Month,sTime.Date);
//	DelayX10ms(100);

	//DisplaySet(150,1,"%02u-%02u-%02u.",sTime.Hour,sTime.Min,sTime.Sec );	//+2�ȴ�ʱ�䲹��
	printf("%2u-%2u-%2u.",sTime.Hour,sTime.Min,sTime.Sec );			//+2�ȴ�ʱ�䲹��

	//DisplaySet(60,1,"    ");	
//	DelayX10ms(100);	
}
