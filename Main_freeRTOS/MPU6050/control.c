#include "control.h"	
#include "filter.h"	
#include "MPU6050.h"
#include "inv_mpu.h"
#include "sys.h"
#include "math.h"
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
u8 Flag_Target;
u32 Flash_R_Count;
int Voltage_Temp,Voltage_Count,Voltage_All;
/**************************************************************************
�������ܣ����еĿ��ƴ��붼��������
         5ms��ʱ�ж���MPU6050��INT���Ŵ���
         �ϸ�֤���������ݴ����ʱ��ͬ��				 
**************************************************************************/
//void TIM3_IRQHandler(void)
//{    
//	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
//	{   
//		   TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ

//} 
//}

void Get_Angle(u8 way)
{ 
	    float Accel_Y,Accel_Angle_X,Accel_Angle_Y,Accel_X,Accel_Z,Gyro_X,Gyro_Y,Gyro_Z;
//	   	Temperature=Read_Temperature();      //===��ȡMPU6050�����¶ȴ��������ݣ����Ʊ�ʾ�����¶ȡ�
	    if(way==1)                           //===DMP�Ķ�ȡ�����ݲɼ��ж϶�ȡ���ϸ���ѭʱ��Ҫ��
			{		
					Read_DMP();                      //===��ȡ���ٶȡ����ٶȡ����
					Angle_Balance=-Roll;             //===����ƽ�����
					Gyro_Balance=-gyro[0];            //===����ƽ����ٶ�
					Gyro_Turn=gyro[2];               //===����ת����ٶ�
					Gyro_Pitch=gyro[1]/100.0;  
					Gyro_Roll=gyro[0]/100.0; 
//				  Acceleration_Z=accel[2];         //===����Z����ٶȼ�
			}			
      else
      {
			Gyro_X=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_XOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_XOUT_L);    //��ȡY��������
			Gyro_Y=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_L);    //��ȡY��������
			Gyro_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_L);    //��ȡZ��������
			
		  Accel_X=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_L); //��ȡX����ٶȼ�				
		  Accel_Y=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_YOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_YOUT_L); //��ȡX����ٶȼ�
			Accel_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_L); //��ȡZ����ٶȼ�

			if(Gyro_X>32768)  Gyro_X-=65536;                       //��������ת��  Ҳ��ͨ��shortǿ������ת��
			if(Gyro_Y>32768)  Gyro_Y-=65536;                       //��������ת��				
			if(Gyro_Z>32768)  Gyro_Z-=65536;                       //��������ת��
				
	  	if(Accel_X>32768) Accel_X-=65536;                      //��������ת��
			if(Accel_Y>32768) Accel_Y-=65536;                      //��������ת��
		  if(Accel_Z>32768) Accel_Z-=65536;                      //��������ת��
				
			Gyro_Balance=Gyro_X;                                  //����ƽ����ٶ�
	   	Accel_Angle_X=atan2(Accel_Y,Accel_Z)*180/PI;                 //�������	
	   	Accel_Angle_Y=-atan2(Accel_X,Accel_Z)*180/PI;                 //�������	
				
			Gyro_X=Gyro_X/16.4f;                                    //����������ת��	
			Gyro_Y=Gyro_Y/16.4f;
      if(Way_Angle==2)
			{
//				Kalman_Filter_x(Accel_Angle_X,Gyro_X);//�������˲�	
				Roll=angle_x;
//				Kalman_Filter_y(Accel_Angle_Y,Gyro_Y);//�������˲�	
				Pitch=angle_y;
			}
			else if(Way_Angle==3)
			{	
				Yijielvbo_x(Accel_Angle_X,Gyro_X);    //�����˲�
				Roll=angle_x;
				Yijielvbo_y(Accel_Angle_Y,Gyro_Y);    //�����˲�
				Pitch=angle_y;				
			}
		}
}
/**************************************************************************
�������ܣ�����ֵ����
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;

}

