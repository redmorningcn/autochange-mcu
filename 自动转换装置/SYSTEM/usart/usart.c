#include "sys.h"
#include "usart.h"	
#include "stdarg.h"	

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F4̽���߿�����
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/6/10
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 
void printfcom1(const char *format, ...);

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	

#define EnableUartSend()        GPIO_SetBits(GPIOA,GPIO_Pin_15)
#define EnableUartRec()         GPIO_ResetBits(GPIOA,GPIO_Pin_15)
uint64_t    GetSysBaseTick(void);

void EnUartContrlIO_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOFʱ��

  //GPIOF9,F10��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 ;       //LED0��LED1��ӦIO��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //����
  GPIO_Init(GPIOA, &GPIO_InitStructure);            //��ʼ��GPIO
	
  //GPIO_SetBits(GPIOA,GPIO_Pin_15);                  //GPIOF9,F10���øߣ�����
  EnableUartSend();
  //EnableUartRec();
    
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIOFʱ��

//  //GPIOD14,F15��ʼ������
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;//LED0��LED1��ӦIO��
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
//  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIO
//	
//  GPIO_SetBits(GPIOD,GPIO_Pin_14 | GPIO_Pin_15);//GPIOD14,D15���øߣ�����
}   

typedef struct  _stcmemcontrol 
{
    u8      *pbuf;      //mem�ռ�ָ��
    u16     buflen;     
    u16     pwrite;     //дαָ��
    u16     pread;      //��αָ��
    u16     pwriteflg;   //����ʶʱ��дָ��λ�á�
    u16     flg;        //��ǣ�b[0] = 1��ǣ� �[
}stcmemcontrol;

u8      uartbuf[1024];
stcmemcontrol   g_uart;
extern  uint64_t g_systime;

void    initmemcontrol(stcmemcontrol *memcontrl, u8 *buf,u32 len)
{
    memcontrl->flg      = 0;
    memcontrl->pbuf     = buf;
    memcontrl->pread    = 0;
    memcontrl->pwrite   = 0;
    memcontrl->pwriteflg=0;
    memcontrl->buflen   = len;
}

//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound){
   //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
    USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
    initmemcontrol((stcmemcontrol  *)&g_uart,uartbuf,sizeof(uartbuf));           //��ʼ�����ڽ��ջ���
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif
	
}


uint64_t l_uartrectime = 0;                 //���ڽ���ʱ��

#define REC_FLG     1                       /*���ձ�ʶ*/

#define END_FLG     0                       /*������ʶ*/


void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
    
    l_uartrectime =  GetSysBaseTick();

    g_uart.flg |= (0x01<< REC_FLG);
    
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)   //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
        Res =USART_ReceiveData(USART1);//(USART1->DR);	    //��ȡ���յ�������
        //printfcom1("%c",Res);
        uartbuf[ g_uart.pwrite ] =  Res;         //���ݱ���

        g_uart.pwrite = (g_uart.pwrite + 1)%g_uart.buflen;
        
    } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
#endif	


void    HoldUartRecSta(void)                                        //ά������״̬
{
    static  uint64_t systime;
    systime = GetSysBaseTick();
    if(systime >  (l_uartrectime + (84000000/9)/100))         //10ms�����ݣ���ΪͨѶ����
    {
        if(     (  (g_uart.flg &(0x01 << END_FLG)) == 0 &&  (g_uart.flg &(0x01 << REC_FLG)))     //��ʱ
            ||  (  (g_uart.pwrite < g_uart.pread)       &&  (g_uart.pwrite+10 == g_uart.pread))  //д��  ��10�ռ� 
          )   
        {
            g_uart.flg   |= (0x01 << END_FLG);           //���ݽ��ս���
            g_uart.flg   &= ~(0x01 << REC_FLG);          //�����ݽ���

            g_uart.pwriteflg = g_uart.pwrite; //��ʶ������ʱ��ʶ
            //printfcom1("\r\n pwrite %d",g_uart.pwrite);
        }
    }
}    

u32 ReadUartBuf(u8 *buf,u32 len)        //�Ӵ��ڻ�����������
{
    if((g_uart.flg & (0x01<< END_FLG)) == 0)   //�޽�����ʶ���˳�
        return 0;
    
    g_uart.flg &= ~(0x01<< END_FLG);           //�������ʶ
    
    u32 noreadlen  = (g_uart.pwriteflg + g_uart.buflen - g_uart.pread) % g_uart.buflen; //������ܻ�����������Ϊ������
    
    u32 readlen    = (noreadlen>len)?len:noreadlen;    //ȡ��������

    for(u32 i=0;i<readlen;i++)
        buf[i] = g_uart.pbuf[(g_uart.pread + i)%g_uart.buflen];
    
    g_uart.pread = (g_uart.pread + readlen)%g_uart.buflen;
        
    //printfcom1("\r\n uartbuflen %d",g_uart.buflen);
    
    return  readlen;                            //���ض�ȡ�����ݳ���
}
 
u32 SendDataCom1(u8 *buf,u32 len)
{
    EnableUartSend();       //IO�˿ڱ�Ϊ���Ͷ˿�

    for(u32 i = 0;i < len;i++)
    {
        uint32  times = 10000;
        USART_SendData(USART1,buf[i]);
        
        while(  USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET
            &&  times-- );  //�����˳�����
    }
    
    EnableUartRec();        //IO�˿ڱ�Ϊ���ܶ˿�

    return  len;
}

void printfcom1(const char *format, ...) 
{ 
   va_list  argptr;
//   uint32   cnt;
   char     a[255];
   
   strlen(format);
   if(strlen(format) > (sizeof(a) - 55))
   {
//        return FALSE;
		return	;
   }
   va_start(argptr, format);
   
//   cnt = vsprintf(a, format, argptr);
   vsprintf(a, format, argptr);
   
   va_end(argptr);
   
   SendDataCom1((u8 *)a,strlen(a));
//   return(cnt);
}

void    TestUart(void)
{
    printfcom1("\r\nstart\r\n");
    while(1)
    {
        u32 len;
        u8 buf[256];

        HoldUartRecSta();
        
        len = ReadUartBuf(buf,sizeof(buf));
        if( len )
            SendDataCom1(buf,len);
        
    }
}
    

