#include "key.h"
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

//�ⲿ�ж�0�������
void EXTI3_IRQHandler(void)
{
	delay_xms(10);	//����
	 
	 EXTI_ClearITPendingBit(EXTI_Line3); //���LINE0�ϵ��жϱ�־λ 
}	

//�ⲿ�ж�9_5�������
void EXTI9_5_IRQHandler(void)
{
	delay_xms(10);	//����
 
 if(EXTI_GetITStatus(EXTI_Line5)!=RESET)     
		{
				 //�жϴ����������
				 EXTI_ClearITPendingBit(EXTI_Line5);
		}
 if(EXTI_GetITStatus(EXTI_Line6)!=RESET)     
		{
				 //�жϴ����������
				 EXTI_ClearITPendingBit(EXTI_Line6);
		}
 if(EXTI_GetITStatus(EXTI_Line7)!=RESET)     
		{
				 //�жϴ����������
				 EXTI_ClearITPendingBit(EXTI_Line7);
		}
 if(EXTI_GetITStatus(EXTI_Line9)!=RESET)     
		{
				 //�жϴ����������
				 EXTI_ClearITPendingBit(EXTI_Line9);
		}		
}
//�ⲿ�ж�15_10�������
void EXTI15_10_IRQHandler(void)
{
	delay_xms(10);	//����
	 if(EXTI_GetITStatus(EXTI_Line11)!=RESET)     
		{
				 //�жϴ����������
				 EXTI_ClearITPendingBit(EXTI_Line11);
		}	

}

#define USE_EXTI 0
//������ʼ������
void KEY_EXTI_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;
#if USE_EXTI	
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
#endif
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��GPIOA,GPIOEʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_7; //KEY0 KEY1 KEY2��Ӧ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //KEY0 KEY1 KEY2��Ӧ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11; //KEY0 KEY1 KEY2��Ӧ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4
		 		 
#if USE_EXTI
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	
	SYSCFG_EXTILineConfig(RCC_AHB1Periph_GPIOB, EXTI_PinSource3);
	SYSCFG_EXTILineConfig(RCC_AHB1Periph_GPIOB, EXTI_PinSource5);
	SYSCFG_EXTILineConfig(RCC_AHB1Periph_GPIOB, EXTI_PinSource7);	
	
	SYSCFG_EXTILineConfig(RCC_AHB1Periph_GPIOD, EXTI_PinSource6);

	SYSCFG_EXTILineConfig(RCC_AHB1Periph_GPIOG, EXTI_PinSource9);
	SYSCFG_EXTILineConfig(RCC_AHB1Periph_GPIOG, EXTI_PinSource11);
	
	/* ����EXTI_Line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line3|EXTI_Line5|EXTI_Line6|EXTI_Line7|EXTI_Line9|EXTI_Line11;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�����ش��� 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE0
  EXTI_Init(&EXTI_InitStructure);//����

 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//�ⲿ�ж�0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x06;//��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//�ⲿ�ж�2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x06;//��ռ���ȼ�3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//�ⲿ�ж�3
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x06;//��ռ���ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
#endif
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//WK_UP��Ӧ����PA0
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;//����
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA0

} 
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY2���� 
//4��WKUP���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY0==0)return 1;
		else if(KEY1==0)return 2;
		else if(KEY2==0)return 3;
//		else if(WK_UP==1)return 4;
	}else if(KEY0==1&&KEY1==1&&KEY2==1)key_up=1; 	    
 	return 0;// �ް�������
}




















