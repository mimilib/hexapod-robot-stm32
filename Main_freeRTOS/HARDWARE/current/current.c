#include "current.h" 
extern 	float current[18];
extern  u16 adcx;
float get_current_init(void)//��ʼ��������ȡ����	
{
	int i_jiaozhun;
	float current,current_val;

	float temp;
	
	 for(i_jiaozhun=0;i_jiaozhun<10;i_jiaozhun++)
		{
		////printf("�Զ�У׼��.......");
		adcx=Get_Adc_Average(ADC_Channel_5,20);
//		LCD_ShowxNum(156,130,adcx,4,16,0);//��ʾADC��ֵ
//		//printf("adcx1=%d\n",adcx);
		temp=(float)adcx*(3.3/4096);
		////printf("temp=%f\n",temp);
		current=((temp-(3.3/2))/0.185);
		////printf("current=%f\n",current);
			current_val+=current;

		}

	 ////printf("У׼���");

	return current_val;

}

float get_current(u8 ch,float current_val)//��ȡ���ε���ֵ
{
	float current_simple;

	float temp;
		ch_74LS138(ch);//ѡ���ȡ������ͨ��
	adcx=Get_Adc_Average(ADC_Channel_5,20);
//		LCD_ShowxNum(156,130,adcx,4,16,0);//��ʾADC��ֵ
//		//printf("adcx1=%d\n",adcx);
		temp=(float)adcx*(3.3/4096);
		//printf("temp=%f\n",temp);
		current_simple=((temp-(3.3/2))/0.185)-current_val/10;
		//printf("current_simple=%f\n",current_simple);
//		adcx=temp;
////		LCD_ShowxNum(156,150,adcx,1,16,0);//��ʾ��ѹֵ
////		//printf("adcx2=%d\n",adcx);
//		temp-=adcx;
//		temp*=1000;
//		LCD_ShowxNum(172,150,temp,3,16,0X80);
//		LED0=!LED0;
	return current_simple;
}


void get_All_current(float current_val)//��ȡ���е���ֵ
{
	int i;
			for(i=1;i<19;i++)
		{
			current[i-1]=get_current(i,current_val);	
		}
}

void ch_74LS138(u8 ch)
{
	switch(ch)
	{
		
		case 1:A2_1=0,A1_1=0,A0_1=0,A2_2=0,A1_2=0,A0_2=0;break;
		case 2:A2_1=0,A1_1=0,A0_1=1,A2_2=0,A1_2=0,A0_2=0;break;
		case 3:A2_1=0,A1_1=1,A0_1=0,A2_2=0,A1_2=0,A0_2=0;break;
		case 4:A2_1=0,A1_1=1,A0_1=1,A2_2=0,A1_2=0,A0_2=0;break;
		case 5:A2_1=1,A1_1=0,A0_1=0,A2_2=0,A1_2=0,A0_2=0;break;
		case 6:A2_1=1,A1_1=0,A0_1=1,A2_2=0,A1_2=0,A0_2=0;break;
		case 7:A2_1=1,A1_1=1,A0_1=0,A2_2=0,A1_2=0,A0_2=0;break;
		case 8:A2_1=1,A1_1=1,A0_1=1,A2_2=0,A1_2=0,A0_2=0;break;
		
		
		case 9:A2_1=0,A1_1=0,A0_1=0,A2_2=0,A1_2=0,A0_2=0;break;
		case 10:A2_1=0,A1_1=0,A0_1=0,A2_2=0,A1_2=0,A0_2=1;break;
		case 11:A2_1=0,A1_1=0,A0_1=0,A2_2=0,A1_2=1,A0_2=0;break;
		case 12:A2_1=0,A1_1=0,A0_1=0,A2_2=0,A1_2=1,A0_2=1;break;
		case 13:A2_1=0,A1_1=0,A0_1=0,A2_2=1,A1_2=0,A0_2=0;break;
		case 14:A2_1=0,A1_1=0,A0_1=0,A2_2=1,A1_2=0,A0_2=1;break;
		case 15:A2_1=0,A1_1=0,A0_1=0,A2_2=1,A1_2=1,A0_2=0;break;
		case 16:A2_1=0,A1_1=0,A0_1=0,A2_2=1,A1_2=1,A0_2=1;break;
		
		
		case 17:CD4053_c=0,CD4053_b=1,CD4053_a=1;break;
		case 18:CD4053_c=1,CD4053_b=0,CD4053_a=1;break;

				
	}
	
	
}

