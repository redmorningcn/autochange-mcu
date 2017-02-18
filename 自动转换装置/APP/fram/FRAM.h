#ifndef  _FRAM_h_
#define  _FRAM_h_

//铁电参数
#define		FRAM_SIZE               8192						         	//2408*8空间容量
#define		FRAM_START_ADDR	        0x0000	                                //铁电的起始地址
#define		FRAM_DIVICE_ADDR        0xa0	                                //铁电的设备地址
#define		FRAM_END_ADDR	      	(FRAM_START_ADDR + FRAM_SIZE)	        //铁电的结束地址


#define		CUR_FLSH_REC_NUM				0		//CPU当前记录流水号铁电存贮首地址 
#define		LST_RED_FLSH_REC_NUM			12		//CPU运行状态铁电存贮首地址
#define		OIL_REVISE_HIGHT_ADDR			24		//油尺修正高度
#define		OIL_DENSITY_ADDR				32		//油尺密度
#define		CALC_PARA_ADDR				    64		//计算运算系数
#define		ADD_DIP_VAL_ADDR				72		//增加油量值
#define		SELECT_MODEL_ADDR				90		//油箱模型选择地址
#define		HOST_DEVICE_INFO_ADDR			100		//主机出厂信息铁电存贮地址
#define		FIX_INFO_ADDR					150		//设备初装车铁电存贮首地址
#define		CUR_REC_ADDR					200		//最新记录地址
#define		LST_RED_FLSH_REC_NUM_GPRS		350		//最后FLASH地址
#define		SYS_RUN_PARA_ADDR				370		//运行参数地址
//#define		DEBUG_INFO_ADDR					350		//调试地址
//#define		LOG_INFO_ADDR					500		//日志存贮首地址
//#define		LOG_INFO_ADDR				600		//日志存贮首地址
#define		DEN_FRAM_ADD					600									
#define		DEN_FRAM_ADD_BCK 				DEN_FRAM_ADD + sizeof(stcDist)		//CPU当前记录流水号铁电存贮首地址

//#define		
 

#define		OIL_DEN_CHAGE_TIMES_ADDR		2000-4	//密度修改次数地址
#define		OIL_BOX_MODEL_ADDR				2000	//油箱模型地址	

#define		FRAM_ERR_EVT					33

//------------------------------------------------------------------------
//  名  称 ：void void InitI2CIO(void)
//  功  能 ：设置 SDA SCL 为普通IO口 进行模拟总线操作
// 入口参数：无
// 出口参数：无
//------------------------------------------------------------------------
extern	void InitI2CIO(void);

//-------------------------------------------------------------------------------------------------------
//函数名称:         WriteFM24CL16()
//功    能:         对FM24CL16指定地址进行写数据
//入口参数:         ADDR:       操作地址    地址：0 ~ sizeof(FM24CL16)
//                  *DataBuf:   数据缓冲
//                  Len:        数据长度
//出口参数:         无
//说明：            
//--------------------------------------------------------------------------------------------------------
extern	uint8 WriteFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen) ; 

//-------------------------------------------------------------------------------------------------------
//函数名称:         ReadFM24CL16()
//功    能:         读取FM24CL16指定地址进行数据
//入口参数:         ADDR:       操作地址   地址：0 ~ sizeof(FM24CL16)
//                  *DataBuf:   数据缓冲 
//                  Len:        数据长度
//出口参数:         无
//说明：            
//--------------------------------------------------------------------------------------------------------
extern	uint8 ReadFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen) ;

//-------------------------------------------------------------------------------------------------------
//函数名称:         TestFM24CL64()
//功    能:         读取FM24CL16指定地址进行数据
//入口参数:         ADDR:       操作地址   地址：0 ~ sizeof(FM24CL16)
//                  *DataBuf:   数据缓冲 
//                  Len:        数据长度
//出口参数:         无
//说明：            
//--------------------------------------------------------------------------------------------------------
extern	uint8 TestFM24CL64(void)  ;
#endif
