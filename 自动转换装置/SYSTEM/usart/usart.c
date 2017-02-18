#include "sys.h"
#include "usart.h"	
#include "stdarg.h"	

////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F4探索者开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/6/10
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 
void printfcom1(const char *format, ...);

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	

#define EnableUartSend()        GPIO_SetBits(GPIOA,GPIO_Pin_15)
#define EnableUartRec()         GPIO_ResetBits(GPIOA,GPIO_Pin_15)
uint64_t    GetSysBaseTick(void);

void EnUartContrlIO_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOF时钟

  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 ;       //LED0和LED1对应IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);            //初始化GPIO
	
  //GPIO_SetBits(GPIOA,GPIO_Pin_15);                  //GPIOF9,F10设置高，灯灭
  EnableUartSend();
  //EnableUartRec();
    
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOF时钟

//  //GPIOD14,F15初始化设置
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;//LED0和LED1对应IO口
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIO
//	
//  GPIO_SetBits(GPIOD,GPIO_Pin_14 | GPIO_Pin_15);//GPIOD14,D15设置高，灯灭
}   

typedef struct  _stcmemcontrol 
{
    u8      *pbuf;      //mem空间指针
    u16     buflen;     
    u16     pwrite;     //写伪指针
    u16     pread;      //读伪指针
    u16     pwriteflg;   //最后标识时，写指针位置。
    u16     flg;        //标记，b[0] = 1标记， 
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

//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound){
   //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
    USART_Cmd(USART1, ENABLE);  //使能串口1 
	
    initmemcontrol((stcmemcontrol  *)&g_uart,uartbuf,sizeof(uartbuf));           //初始化串口接收缓存
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif
	
}


uint64_t l_uartrectime = 0;                 //串口接受时间

#define REC_FLG     1                       /*接收标识*/

#define END_FLG     0                       /*结束标识*/


void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
    
    l_uartrectime =  GetSysBaseTick();

    g_uart.flg |= (0x01<< REC_FLG);
    
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)   //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
        Res =USART_ReceiveData(USART1);//(USART1->DR);	    //读取接收到的数据
        //printfcom1("%c",Res);
        uartbuf[ g_uart.pwrite ] =  Res;         //数据保存

        g_uart.pwrite = (g_uart.pwrite + 1)%g_uart.buflen;
        
    } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
#endif	


void    HoldUartRecSta(void)                                        //维护串口状态
{
    static  uint64_t systime;
    systime = GetSysBaseTick();
    if(systime >  (l_uartrectime + (84000000/9)/100))         //10ms无数据，认为通讯错误
    {
        if(     (  (g_uart.flg &(0x01 << END_FLG)) == 0 &&  (g_uart.flg &(0x01 << REC_FLG)))     //超时
            ||  (  (g_uart.pwrite < g_uart.pread)       &&  (g_uart.pwrite+10 == g_uart.pread))  //写满  差10空间 
          )   
        {
            g_uart.flg   |= (0x01 << END_FLG);           //数据接收结束
            g_uart.flg   &= ~(0x01 << REC_FLG);          //清数据接收

            g_uart.pwriteflg = g_uart.pwrite; //标识区结束时标识
            //printfcom1("\r\n pwrite %d",g_uart.pwrite);
        }
    }
}    

u32 ReadUartBuf(u8 *buf,u32 len)        //从串口缓存区读数据
{
    if((g_uart.flg & (0x01<< END_FLG)) == 0)   //无结束标识，退出
        return 0;
    
    g_uart.flg &= ~(0x01<< END_FLG);           //清结束标识
    
    u32 noreadlen  = (g_uart.pwriteflg + g_uart.buflen - g_uart.pread) % g_uart.buflen; //计算接受缓冲区的数据为读长度
    
    u32 readlen    = (noreadlen>len)?len:noreadlen;    //取读数长度

    for(u32 i=0;i<readlen;i++)
        buf[i] = g_uart.pbuf[(g_uart.pread + i)%g_uart.buflen];
    
    g_uart.pread = (g_uart.pread + readlen)%g_uart.buflen;
        
    //printfcom1("\r\n uartbuflen %d",g_uart.buflen);
    
    return  readlen;                            //返回读取的数据长度
}
 
u32 SendDataCom1(u8 *buf,u32 len)
{
    EnableUartSend();       //IO端口变为发送端口

    for(u32 i = 0;i < len;i++)
    {
        uint32  times = 10000;
        USART_SendData(USART1,buf[i]);
        
        while(  USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET
            &&  times-- );  //增加退出机制
    }
    
    EnableUartRec();        //IO端口变为接受端口

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
    

