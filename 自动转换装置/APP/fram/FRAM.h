#ifndef  _FRAM_h_
#define  _FRAM_h_

//�������
#define		FRAM_SIZE               8192						         	//2408*8�ռ�����
#define		FRAM_START_ADDR	        0x0000	                                //�������ʼ��ַ
#define		FRAM_DIVICE_ADDR        0xa0	                                //������豸��ַ
#define		FRAM_END_ADDR	      	(FRAM_START_ADDR + FRAM_SIZE)	        //����Ľ�����ַ


#define		CUR_FLSH_REC_NUM				0		//CPU��ǰ��¼��ˮ����������׵�ַ 
#define		LST_RED_FLSH_REC_NUM			12		//CPU����״̬��������׵�ַ
#define		OIL_REVISE_HIGHT_ADDR			24		//�ͳ������߶�
#define		OIL_DENSITY_ADDR				32		//�ͳ��ܶ�
#define		CALC_PARA_ADDR				    64		//��������ϵ��
#define		ADD_DIP_VAL_ADDR				72		//��������ֵ
#define		SELECT_MODEL_ADDR				90		//����ģ��ѡ���ַ
#define		HOST_DEVICE_INFO_ADDR			100		//����������Ϣ���������ַ
#define		FIX_INFO_ADDR					150		//�豸��װ����������׵�ַ
#define		CUR_REC_ADDR					200		//���¼�¼��ַ
#define		LST_RED_FLSH_REC_NUM_GPRS		350		//���FLASH��ַ
#define		SYS_RUN_PARA_ADDR				370		//���в�����ַ
//#define		DEBUG_INFO_ADDR					350		//���Ե�ַ
//#define		LOG_INFO_ADDR					500		//��־�����׵�ַ
//#define		LOG_INFO_ADDR				600		//��־�����׵�ַ
#define		DEN_FRAM_ADD					600									
#define		DEN_FRAM_ADD_BCK 				DEN_FRAM_ADD + sizeof(stcDist)		//CPU��ǰ��¼��ˮ����������׵�ַ

//#define		
 

#define		OIL_DEN_CHAGE_TIMES_ADDR		2000-4	//�ܶ��޸Ĵ�����ַ
#define		OIL_BOX_MODEL_ADDR				2000	//����ģ�͵�ַ	

#define		FRAM_ERR_EVT					33

//------------------------------------------------------------------------
//  ��  �� ��void void InitI2CIO(void)
//  ��  �� ������ SDA SCL Ϊ��ͨIO�� ����ģ�����߲���
// ��ڲ�������
// ���ڲ�������
//------------------------------------------------------------------------
extern	void InitI2CIO(void);

//-------------------------------------------------------------------------------------------------------
//��������:         WriteFM24CL16()
//��    ��:         ��FM24CL16ָ����ַ����д����
//��ڲ���:         ADDR:       ������ַ    ��ַ��0 ~ sizeof(FM24CL16)
//                  *DataBuf:   ���ݻ���
//                  Len:        ���ݳ���
//���ڲ���:         ��
//˵����            
//--------------------------------------------------------------------------------------------------------
extern	uint8 WriteFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen) ; 

//-------------------------------------------------------------------------------------------------------
//��������:         ReadFM24CL16()
//��    ��:         ��ȡFM24CL16ָ����ַ��������
//��ڲ���:         ADDR:       ������ַ   ��ַ��0 ~ sizeof(FM24CL16)
//                  *DataBuf:   ���ݻ��� 
//                  Len:        ���ݳ���
//���ڲ���:         ��
//˵����            
//--------------------------------------------------------------------------------------------------------
extern	uint8 ReadFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen) ;

//-------------------------------------------------------------------------------------------------------
//��������:         TestFM24CL64()
//��    ��:         ��ȡFM24CL16ָ����ַ��������
//��ڲ���:         ADDR:       ������ַ   ��ַ��0 ~ sizeof(FM24CL16)
//                  *DataBuf:   ���ݻ��� 
//                  Len:        ���ݳ���
//���ڲ���:         ��
//˵����            
//--------------------------------------------------------------------------------------------------------
extern	uint8 TestFM24CL64(void)  ;
#endif
