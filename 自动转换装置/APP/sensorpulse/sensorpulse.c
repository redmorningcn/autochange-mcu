#include "sensorpulse.h"
#include "delay.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//����������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

  stcchpulse  gs_pulsech[8];        //ͨ��8
  stcpulse    gs_storepluse[8];     //�洢��Ϣ  
  stcrunpara  gs_runpara;           //����
  stcproinfo  gs_proinfo;           //��Ʒ��Ϣ

