#include "balance_task.h"
#include "FreeRTOS.h"
#include "action_task.h"
#include "task.h"

struct _lobot_servo_ servos_array[18];

float Pitch_use;
float Roll_use;
float Pitch;
float Roll;

float Kp_Pitch = -0.05;
float Ki_Pitch = 0;
float Kd_Pitch = -0.1;
float Kp_Roll = 0.05;
float Ki_Roll = 0;
float Kd_Roll = 0.1;
int pid_flag = 0;
float Pitch_goal;
double theatx, theaty;  //可控旋转角
double theatx_last, theaty_last;
double theatx_use, theaty_use;
float Pitch_Integral;
float PidOutPut_Pitch;
float PidOutPut_Roll;
float h_out[6] = {80, 80, 80, 80, 80, 80};
float l_out[6] = {80, 80, 80, 80, 80, 80};
float h_test = 80;

//机器人上平面六点向量
double T03_RF[3][1] = {(5.875), (12.0), 0};
double T03_RM[3][1] = {(9), 0, 0};
double T03_RB[3][1] = {(5.875), -(12.0), 0};
double T03_LF[3][1] = {-(5.875), (12.0), 0};
double T03_LM[3][1] = {-(9), 0, 0};
double T03_LB[3][1] = {-(5.875), -(12.0), 0};

//机器人足端向量
double T06_RF[3];
double T06_RM[3];
double T06_RB[3];
double T06_LF[3];
double T06_LM[3];
double T06_LB[3];

//机器人足端向量初始值（静立）(实际值）
double T06_init_RF[3];
double T06_init_RM[3];
double T06_init_RB[3];
double T06_init_LF[3];
double T06_init_LM[3];
double T06_init_LB[3];

//机器人足端向量初始值（静立）(理想值）
double T06_init_RF_ideal[3];
double T06_init_RM_ideal[3];
double T06_init_RB_ideal[3];
double T06_init_LF_ideal[3];
double T06_init_LM_ideal[3];
double T06_init_LB_ideal[3];

//机器人机械参数
double d_RF[4] = {13.361, 4.3, 7.6, 13.5};
double d_RM[4] = {9, 4.3, 7.6, 13.5};
double d_RB[4] = {13.361, 4.3, 7.6, 13.5};
double d_LF[4] = {13.361, 4.3, 7.6, 13.5};
double d_LM[4] = {9, 4.3, 7.6, 13.5};
double d_LB[4] = {13.361, 4.3, 7.6, 13.5};

//旋转后的上平面六点向量
double T03_RFx[3][1], T03_RMx[3][1], T03_RBx[3][1], T03_LFx[3][1],
    T03_LMx[3][1], T03_LBx[3][1];
double T03_RFxy[3][1], T03_RMxy[3][1], T03_RBxy[3][1], T03_LFxy[3][1],
    T03_LMxy[3][1], T03_LBxy[3][1];

//逆运动学运算结果
double tt_RF[6];
double tt_RM[6];
double tt_RB[6];
double tt_LF[6];
double tt_LM[6];
double tt_LB[6];

double tt_RF_last[6];
double tt_RM_last[6];
double tt_RB_last[6];
double tt_LF_last[6];
double tt_LM_last[6];
double tt_LB_last[6];

///////////////////////////////////六足机器人逆运动学运算器//////////////////////////////////////
void xyz_to_theat(double T03[3][1], double T06[3], double d[4], double tt[6]) {
    //                                     D-H参数表
    //                       theta    d           a        alpha     offset
    //              ML1=Link([0       0           0           0          0])
    //              ML2=Link([0       0           0 pi/2      0          0])
    //              ML3=Link([0       0           d3 -pi/2    0          0])
    //              ML4=Link([0       0           d4 pi/2     0          0])
    //              ML5=Link([0       0           d5          0          0])
    //              ML6=Link([0       0           d6          0          0])

    double d3, d4, d5, d6;
    double tt1, tt2, tt3, tt4, tt5;
    double x26, y26, z26;
    double g;
    double cc4, ss4;
    double x03, y03, z03;
    double x06, y06, z06;

    d3 = d[0];
    d4 = d[1];
    d5 = d[2];
    d6 = d[3];

    x03 = T03[0][0];
    y03 = T03[1][0];
    z03 = T03[2][0];

    x06 = T06[0];
    y06 = T06[1];
    z06 = T06[2];

    //计算 T1 T2
    tt1 = atan2(y03, x03);
    tt2 = asin(z03 / d3);
    //计算 x26 y26 z26
    x26 = (z06 * sin(tt2) * cos(tt1) * cos(tt1) + x06 * cos(tt2) * cos(tt1) +
           z06 * sin(tt2) * sin(tt1) * sin(tt1) + y06 * cos(tt2) * sin(tt1)) /
          ((cos(tt1) * cos(tt1) + sin(tt1) * sin(tt1)) *
           (cos(tt2) * cos(tt2) + sin(tt2) * sin(tt2)));

    y26 = -(x06 * cos(tt1) * sin(tt2) + y06 * sin(tt1) * sin(tt2) -
            z06 * cos(tt1) * cos(tt1) * cos(tt2) -
            z06 * cos(tt2) * sin(tt1) * sin(tt1)) /
          ((cos(tt1) * cos(tt1) + sin(tt1) * sin(tt1)) *
           (cos(tt2) * cos(tt2) + sin(tt2) * sin(tt2)));

    z26 = -(y06 * cos(tt1) - x06 * sin(tt1)) /
          (cos(tt1) * cos(tt1) + sin(tt1) * sin(tt1));
    //计算 T3
    tt3 = atan2(-z26, x26 - d6);
    //计算T5
    g = (x26 - d6) / cos(tt3) - d4;
    tt5 = -acos((g * g + y26 * y26 - d5 * d5 - d6 * d6) / (d5 * d6 * 2.0f));
    //计算 T4
    cc4 = (d5 * g + d6 * g * cos(tt5) + d6 * y26 * sin(tt5)) /
          (d6 * d6 * cos(tt5) * cos(tt5) + d6 * d6 * sin(tt5) * sin(tt5) +
           d5 * d5 + 2.0f * d5 * d6 * cos(tt5));
    ss4 = (d5 * y26 - d6 * g * sin(tt5) + d6 * y26 * cos(tt5)) /
          (d6 * d6 * cos(tt5) * cos(tt5) + d6 * d6 * sin(tt5) * sin(tt5) +
           d5 * d5 + 2.0f * d5 * d6 * cos(tt5));
    tt4 = atan2(ss4, cc4);

    tt[1] = tt1;
    tt[2] = tt2;
    tt[3] = tt3;
    tt[4] = tt4;
    tt[5] = tt5;
}
///////////////////////////////////PID控制器////////////////////////////////////
float balance_Pitch(float Angle, float Gyro) {
    float Bias;
    float balance;
    Bias = Angle - (Pitch_goal);  //===求出平衡的角度中值 和机械相关
    Pitch_Integral += Bias;
    balance =
        Kp_Pitch * Bias + Gyro * Kd_Pitch +
        Pitch_Integral *
            Ki_Pitch;  //===计算平衡控制的电机PWM  PD控制   kp是P系数 kd是D系数
    return balance;
}
float Roll_goal;
float Roll_Integral;
float balance_Roll(float Angle, float Gyro) {
    float Bias;
    float balance;
    Bias = Angle - (Roll_goal);  //===求出平衡的角度中值 和机械相关
    Roll_Integral += Bias;
    balance =
        Kp_Roll * Bias + Gyro * Kd_Roll +
        Roll_Integral *
            Ki_Roll;  //===计算平衡控制的电机PWM  PD控制   kp是P系数 kd是D系数
    return balance;
}

//向量沿X轴旋转（旋转矩阵法）
void rotate_x(double matrix2[3][1], double matrix[3][1], double theta) {
    double matrix1[3][3];
    int i, j, k;

    matrix1[0][0] = 1;
    matrix1[0][1] = 0;
    matrix1[0][2] = 0;
    matrix1[1][0] = 0;
    matrix1[1][1] = cos(theta);
    matrix1[1][2] = -sin(theta);
    matrix1[2][0] = 0;
    matrix1[2][1] = sin(theta);
    matrix1[2][2] = cos(theta);

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 1; j++) {
            matrix[i][j] = 0;
        }
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 1; j++) {
            for (k = 0; k < 3; k++) {
                matrix[i][j] = matrix[i][j] + matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

//向量沿Y轴旋转（旋转矩阵法）
void rotate_y(double matrix2[3][1], double matrix[3][1], double theta) {
    double matrix1[3][3];
    int i, j, k;

    matrix1[0][0] = cos(theta);
    matrix1[0][1] = 0;
    matrix1[0][2] = sin(theta);
    matrix1[1][0] = 0;
    matrix1[1][1] = 1;
    matrix1[1][2] = 0;
    matrix1[2][0] = -sin(theta);
    matrix1[2][1] = 0;
    matrix1[2][2] = cos(theta);

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 1; j++) {
            matrix[i][j] = 0;
        }
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 1; j++) {
            for (k = 0; k < 3; k++) {
                matrix[i][j] = matrix[i][j] + matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

//进行旋转运算
void rotate(void) {
    rotate_x(T03_RF, T03_RFx, theatx_use);
    rotate_y(T03_RFx, T03_RFxy, theaty_use);
    rotate_x(T03_RM, T03_RMx, theatx_use);
    rotate_y(T03_RMx, T03_RMxy, theaty_use);
    rotate_x(T03_RB, T03_RBx, theatx_use);
    rotate_y(T03_RBx, T03_RBxy, theaty_use);
    rotate_x(T03_LF, T03_LFx, theatx_use);
    rotate_y(T03_LFx, T03_LFxy, theaty_use);
    rotate_x(T03_LM, T03_LMx, theatx_use);
    rotate_y(T03_LMx, T03_LMxy, theaty_use);
    rotate_x(T03_LB, T03_LBx, theatx_use);
    rotate_y(T03_LBx, T03_LBxy, theaty_use);
}
//进行逆运动学运算
void xyz_to_theat_math(void) {
    xyz_to_theat(T03_RFxy, T06_RF, d_RF, tt_RF);
    xyz_to_theat(T03_RMxy, T06_RM, d_RM, tt_RM);
    xyz_to_theat(T03_RBxy, T06_RB, d_RB, tt_RB);
    xyz_to_theat(T03_LFxy, T06_LF, d_LF, tt_LF);
    xyz_to_theat(T03_LMxy, T06_LM, d_LM, tt_LM);
    xyz_to_theat(T03_LBxy, T06_LB, d_LB, tt_LB);
}

//执行逆运动学运算结果
void xyz_to_theat_action(int time) {
    /*检测是否超出活动空间*/
    if (isnan(tt_RF[5])) {
        tt_RF[3] = tt_RF_last[3];
        tt_RF[4] = tt_RF_last[4];
        tt_RF[5] = tt_RF_last[5];
    }
    if (isnan(tt_RM[5])) {
        tt_RM[3] = tt_RM_last[3];
        tt_RM[4] = tt_RM_last[4];
        tt_RM[5] = tt_RM_last[5];
    }
    if (isnan(tt_RB[5])) {
        tt_RB[3] = tt_RB_last[3];
        tt_RB[4] = tt_RB_last[4];
        tt_RB[5] = tt_RB_last[5];
    }
    if (isnan(tt_LF[5])) {
        tt_LF[3] = tt_LF_last[3];
        tt_LF[4] = tt_LF_last[4];
        tt_LF[5] = tt_LF_last[5];
    }
    if (isnan(tt_LM[5])) {
        tt_LM[3] = tt_LM_last[3];
        tt_LM[4] = tt_LM_last[4];
        tt_LM[5] = tt_LM_last[5];
    }
    if (isnan(tt_LB[5])) {
        tt_LB[3] = tt_LB_last[3];
        tt_LB[4] = tt_LB_last[4];
        tt_LB[5] = tt_LB_last[5];
    }

    /*执行动作*/
    action_angle(RF1, tt_RF[3] / 3.1416 * 180, time);
    action_angle(RF2, tt_RF[4] / 3.1416 * 180, time);
    action_angle(RF3, tt_RF[5] / 3.1416 * 180, time);

    action_angle(RM1, tt_RM[3] / 3.1416 * 180, time);
    action_angle(RM2, tt_RM[4] / 3.1416 * 180, time);
    action_angle(RM3, tt_RM[5] / 3.1416 * 180, time);

    action_angle(RB1, tt_RB[3] / 3.1416 * 180, time);
    action_angle(RB2, tt_RB[4] / 3.1416 * 180, time);
    action_angle(RB3, tt_RB[5] / 3.1416 * 180, time);

    action_angle(LF1, tt_LF[3] / 3.1416 * 180, time);
    action_angle(LF2, tt_LF[4] / 3.1416 * 180, time);
    action_angle(LF3, tt_LF[5] / 3.1416 * 180, time);

    action_angle(LM1, tt_LM[3] / 3.1416 * 180, time);
    action_angle(LM2, tt_LM[4] / 3.1416 * 180, time);
    action_angle(LM3, tt_LM[5] / 3.1416 * 180, time);

    action_angle(LB1, tt_LB[3] / 3.1416 * 180, time);
    action_angle(LB2, tt_LB[4] / 3.1416 * 180, time);
    action_angle(LB3, tt_LB[5] / 3.1416 * 180, time);

    moveServosByArray(servos_array, 18 /*Num*/, time);

    /*记录上一次动作*/
    tt_RF_last[3] = tt_RF[3];
    tt_RF_last[4] = tt_RF[4];
    tt_RF_last[5] = tt_RF[5];

    tt_RM_last[3] = tt_RM[3];
    tt_RM_last[4] = tt_RM[4];
    tt_RM_last[5] = tt_RM[5];

    tt_RB_last[3] = tt_RB[3];
    tt_RB_last[4] = tt_RB[4];
    tt_RB_last[5] = tt_RB[5];

    tt_LF_last[3] = tt_LF[3];
    tt_LF_last[4] = tt_LF[4];
    tt_LF_last[5] = tt_LF[5];

    tt_LM_last[3] = tt_LM[3];
    tt_LM_last[4] = tt_LM[4];
    tt_LM_last[5] = tt_LM[5];

    tt_LB_last[3] = tt_LB[3];
    tt_LB_last[4] = tt_LB[4];
    tt_LB_last[5] = tt_LB[5];
}

int angle_t;
int angle_t0;
float angle_T = 5000;
float angle_phase;

float L_init_BuChang = 3.5f;
struct INIT_BuChang init_BuChang = {0, 0, 0, 0, 0, 0.5};

void T06_init(void) {
    //直立末端位置生成器
    LH_to_xyz_init(T03_RF, d_RF, L_init, H_init, T06_init_RF);
    LH_to_xyz_init(T03_RM, d_RM, L_init + L_init_BuChang, H_init, T06_init_RM);
    LH_to_xyz_init(T03_RB, d_RB, L_init, H_init, T06_init_RB);
    LH_to_xyz_init(T03_LF, d_LF, L_init, H_init, T06_init_LF);
    LH_to_xyz_init(T03_LM, d_LM, L_init + L_init_BuChang, H_init, T06_init_LM);
    LH_to_xyz_init(T03_LB, d_LB, L_init, H_init, T06_init_LB);

    //直立末端位置补偿器
    T06_init_RF[2] += init_BuChang.rf;
    T06_init_RM[2] += init_BuChang.rm;
    T06_init_RB[2] += init_BuChang.rb;
    T06_init_LF[2] += init_BuChang.lf;
    T06_init_LM[2] += init_BuChang.lm;
    T06_init_LB[2] += init_BuChang.lb;
}

void T06_init_ideal(void) {
    int i;
    for (i = 0; i < 3; i++) {
        T06_init_RF_ideal[i] = T06_init_RF[i];
        T06_init_RM_ideal[i] = T06_init_RM[i];
        T06_init_RB_ideal[i] = T06_init_RB[i];
        T06_init_LF_ideal[i] = T06_init_LF[i];
        T06_init_LM_ideal[i] = T06_init_LM[i];
        T06_init_LB_ideal[i] = T06_init_LB[i];
    }
}

void gain_Angle_And_Gyro(void) {
    Roll_use = stcAngle.Angle[0] / 32768.0 * 180;
    Pitch_use = stcAngle.Angle[1] / 32768.0 * 180;
    Gyro_Roll = stcGyro.w[0] / 32768.0 * 2000;
    Gyro_Pitch = stcGyro.w[1] / 32768.0 * 2000;
}

void Balance_Pid_Contral(void) {
    PidOutPut_Pitch = balance_Pitch(Pitch_use, Gyro_Pitch);  //俯仰角控制
    PidOutPut_Roll = balance_Roll(Roll_use, Gyro_Roll);      //横滚角控制
    //设定旋转角度
    theaty -= PidOutPut_Roll;
    theatx -= PidOutPut_Pitch;
    //限制角度范围

    if (theatx > 20)
        theatx = 20;
    if (theatx < -20)
        theatx = -20;
    if (theaty > 20)
        theaty = 20;
    if (theaty < -20)
        theaty = -20;
}
void directContralForBalance(void) {
    //获取当前时间
    angle_t = systime - angle_t0;

    //获取当前相位
    angle_phase = angle_t / angle_T;

    //进入下一周期
    if (angle_phase >= 1) {
        angle_phase = 0, angle_t0 = systime;
        Roll_use = stcAngle.Angle[1] / 32768.0 * 180;
        Pitch_use = stcAngle.Angle[0] / 32768.0 * 180;
        Gyro_Roll = stcGyro.w[1] / 32768.0 * 2000;
        Gyro_Pitch = stcGyro.w[0] / 32768.0 * 2000;
        theatx_last = theatx;
        theaty_last = theaty;
    }
    if (angle_phase <= 1 && (Roll_use > 2 || Roll_use < -2)) {
        theatx = theatx_last + angle_phase * Roll_use;
    }
    if (angle_phase <= 1 && (Pitch_use > 2 || Pitch_use < -2)) {
        theaty = theaty_last - angle_phase * Pitch_use;
    }
}
///////////////////////////////////////angle任务函数////////////////////////////////
void angle_task(void* pvParameters) {
    while (1) {
        gain_Angle_And_Gyro();

        if (pid_flag == 1) {
            Balance_Pid_Contral();
        }

        //  directContralForBalance();
        //              printf("%d",systime);
        vTaskDelay(110);
    }
}
u8 flag_action_run_single = 0, flag_action_run_continiue = 0;
///////////////////////////////////////banlance任务函数////////////////////////////////
void banlance_task(void* pvParameters) {
    while (1) {
        if (flag_action_run_single == 2) {
            //角度制转弧度制
            theatx_use = theatx / 180.0 * 3.1415926;
            theaty_use = theaty / 180.0 * 3.1415926;

            flag_action_run_single = 0;

            rotate();  //对机器人上平面六点向量进行旋转变换

            xyz_to_theat_math();  //进行逆运动学运算

            xyz_to_theat_action(250);  //执行运算结果
        }
        vTaskDelay(1);
    }
}
