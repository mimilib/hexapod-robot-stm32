#ifndef _PID_H
#define _PID_H
#include "sys.h"
typedef struct
{
 float Setvalue;   //�û��趨ֵ
 float Presentvale;//��ǰ���ֵ
 float Kp;               
 float T;    //��������
 //float Ti;   //���ֳ���
 float Ki;
 //float Td;   //΢�ֳ���
 float Kd;
 float Ek;   //����ƫ��
 float Ek_1; //�ϴ�ƫ��
 float SumEk;//��ʷƫ��֮��
 float OUT_0;//����ֵΪ0ʱ��ά�ֵ�ǰ���
 float OUT;  //���μ�������Ŀ�����
}PID;
void PID_Calculate(PID *pid);//PID ���㣬���������ڵ���ʱ���� �����ؼ���ֵ��
//��ʼ���趨ֵ���õ�ǰֵ ����Kp Ki Kd ���ò������ڣ��ò�����������OUT_0
//��Ҫ����ֵ Pre
#endif
