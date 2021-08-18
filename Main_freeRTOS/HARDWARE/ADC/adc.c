#include "adc.h"
#include "delay.h"		 
#include "stm32f4xx_adc.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dma.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//ADC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

__IO uint16_t ADC_ConvertedValue[RHEOSTAT_NOFCHANEL]={0};		
//��ʼ��ADC															   
void  Adc_Init(void)
{
//��ʼ��ADC1	
  GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ʹ��ADC1ʱ��

  //�ȳ�ʼ��ADC1ͨ��5 IO��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5;//PA5 ͨ��5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 
 
	
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;//ɨ��ģʽ����
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//����ת��
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;//������������
  ADC_InitStructure.ADC_NbrOfConversion = RHEOSTAT_NOFCHANEL;//ת��6��ͨ�� 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC��ʼ��
	
	ADC_RegularChannelConfig(ADC1, 0, 1, ADC_SampleTime_480Cycles );	//ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			    
//  ADC_RegularChannelConfig(ADC1, 1, 2, ADC_SampleTime_480Cycles );
////	ADC_RegularChannelConfig(ADC1, 2, 3, ADC_SampleTime_480Cycles );
////	ADC_RegularChannelConfig(ADC1, 3, 4, ADC_SampleTime_480Cycles );
//	ADC_RegularChannelConfig(ADC1, 4, 3, ADC_SampleTime_480Cycles );
//	ADC_RegularChannelConfig(ADC1, 5, 4, ADC_SampleTime_480Cycles );
	
   // ʹ��DMA���� after last transfer (Single-ADC mode)
  ADC_DMARequestAfterLastTransferCmd(RHEOSTAT_ADC, ENABLE);
  // ʹ��ADC DMA
  ADC_DMACmd(RHEOSTAT_ADC, ENABLE);
	
	ADC_Cmd(ADC1, ENABLE);//����ADת����	
	ADC_SoftwareStartConv(ADC1);		//ʹ��ָ����ADC1�����ת����������	
//��ʼ��DMA

		
		 //ʹ��DMA2��Ӧ��ʱ��
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 
		
		DMA_DeInit(DMA2_Stream0);
		
		// ѡ�� DMA ͨ����ͨ������������
		//����ΪDMA2��ͨ��0
		DMA_InitStructure.DMA_Channel = RHEOSTAT_ADC_DMA_CHANNEL; 
		// �����ַΪ��ADC ���ݼĴ�����ַ
		//�������ַΪADC3�Ļ���+0x4c
		DMA_InitStructure.DMA_PeripheralBaseAddr = RHEOSTAT_ADC_DR_ADDR;	
		
		//ע������Ӳ���&
		// �洢����ַ��ʵ���Ͼ���һ���ڲ�SRAM�ı���
		DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_ConvertedValue;  
		
		
		// ���ݴ��䷽��Ϊ���赽�洢��	
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	
		// ��������СΪ��ָһ�δ����������
		DMA_InitStructure.DMA_BufferSize = RHEOSTAT_NOFCHANEL;	
		
		// ����Ĵ���ֻ��һ������ַ���õ���
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		// �洢����ַ����
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
		// // �������ݴ�СΪ���֣��������ֽ� 
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 
		//	�洢�����ݴ�СҲΪ���֣����������ݴ�С��ͬ
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	
		// ѭ������ģʽ
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		// DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		// ��ֹDMA FIFO	��ʹ��ֱ��ģʽ
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  
		// FIFO ��С��FIFOģʽ��ֹʱ�������������	
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
		
		//��ʼ��DMA�������൱��һ����Ĺܵ����ܵ������кܶ�ͨ��
		DMA_Init(RHEOSTAT_ADC_DMA_STREAM, &DMA_InitStructure);
		// ʹ��DMA��
		DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, ENABLE);
}				  
//���ADCֵ
//ch: @ref ADC_channels 
//ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_Channel_0~ADC_Channel_16
//����ֵ:ת�����
u16 Get_Adc(u8 ch)   
{
	  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	
	ADC_SoftwareStartConv(ADC1);		//ʹ��ָ����ADC1�����ת����������	
	 
//	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}
//��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
//ch:ͨ�����
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
	}
	return temp_val/times;
} 
	 









