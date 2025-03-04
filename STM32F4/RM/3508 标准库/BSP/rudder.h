#ifndef __RUDDER_H
#define __RUDDER_H


#include "Driver.h"

#define LF_F_MidAngle     2713
#define RF_F_MidAngle     2021
#define LB_F_MidAngle     6836
#define RB_F_MidAngle     5470

#define CHAS_CHASE_K    2.2f
#define FB_DIR_speed_Limit    2   //ǰ�������  15
#define Report_speed_Limit    16//FB_DIR_speed_Limit*1.5
#define SPIN_DIR_Angle        22//FB_DIR_speed_Limit*1.4   //��ת�������  15

#define   SPIN_SPEED   580
#define   SPIN_SPEED_50W  580//-3300
#define   SPIN_SPEED_60W  620//-3450w
#define   SPIN_SPEED_80W  850//-3600
#define   SPIN_SPEED_100W  980//-4000
#define   SPIN_SPEED_120W  1060//-4500
#define   SPIN_SPEED_SUPER 1150//-5000

#define   Move_SPIN_SPEED   320
#define   Move_SPIN_SPEED_50W  320//-3300
#define   Move_SPIN_SPEED_60W  340//-3450w
#define   Move_SPIN_SPEED_80W  360//-3600
#define   Move_SPIN_SPEED_100W  380//-4000
#define   Move_SPIN_SPEED_120W  400//-4500
#define   Move_SPIN_SPEED_SUPER 440//-5000


//#define LF_LR_MidAngle     LF_F_MidAngle + 2048
//#define RF_LR_MidAngle     LF_F_MidAngle - 2048
//#define LB_LR_MidAngle     LF_F_MidAngle - 2048
//#define RB_LR_MidAngle     LF_F_MidAngle + 2048

//#define LF_Z_MidAngle      LF_F_MidAngle + 1024
//#define RF_Z_MidAngle      LF_F_MidAngle - 1024
//#define LB_Z_MidAngle      LF_F_MidAngle - 1024
//#define RB_Z_MidAngle      LF_F_MidAngle + 1024


/* ���ֵ�������±� */
typedef enum 
{
  RUD_LF = 0,
  RUD_RF = 1,
  RUD_LB = 2,
  RUD_RB = 3,
  RUD_MOTOR_CNT = 4,
}RUD_Motor_cnt_t;

typedef enum
{
  LEFT = -1,
  RIGH = 1,
}Z_LR_dir_t;

/*���ʸ������Ϣ*/
typedef struct
{
  float F_dir;//ʼ��Ϊͷָ����
  float XY_dir;//XYƽ���ǰ������
  float Z_dir;//Z��ת����
  float XY_Z_Angle;//XYƽ��ǰ������Zƽ����ת��֮��ļн�
  float prev_Z_angle;//����xyz�ޱ��ǰ�����һ�����
  float XYZ_Fusion_Speed;
  Z_LR_dir_t Z_LR;
  float Dir;//ʸ���Ƿ���
  bool  IF_SPIN_Dir;//�ж����һ�α���Ƿ�Ϊ����ת��
}Rudder_Move_t;

/*4������Ϣ�ṹ��*/
typedef struct
{
  Motor_Info_t motor_info;
  Motor_Data_t motor_data;
  PID_Info_t PID;

  //�ǶȻ�������
  extKalman_t KF_Angle[PID_TYPE_CNT];
  
  float MECH_Mid_Angle;//����Ļ�е��ֵ����ʼ��¼��
  float SPIN_Mid_Angle;//������ת��
  
  Rudder_Move_t RUD_Move;
  
  
}Rudder_Axis_Info_t;//����������ƽṹ��

typedef struct
{
  float X_speed;
  float Y_speed;
  float Z_speed;
  Z_LR_dir_t Z_LR;
  float XY_Fusion_speed;
  float XYZ_Fusion_speed;
}Rudder_Vector_Move_t;


/*����Ϣ�ṹ��*/
typedef struct
{
  System_Ctrl_Mode_t ctrl_mode;
  System_Pid_Mode_t pid_mode;
  System_Action_t action_mode;
  PID_type_t PID_type; 
  
  Rudder_Axis_Info_t Rudder_Axis[RUD_MOTOR_CNT];//�ĸ�����
  Defense_Mode_t Defense_Mode;
  Rudder_Vector_Move_t Vector;//�����ٶ�ʸ��
  
}Rudder_Info_t;




extern Rudder_Info_t Rudder;
void RUDDER_Ctrl(void);
void RUD_KEY_Ctrl(void);
void RUD_RC_Ctrl(void);
void RUD_Reset(void);
void RUD_GET_Info(void);
void RUD_SetPID(void);
void RUD_PID_Switch(PID_Info_t *str );
void RUD_Output(void);
void RUD_SetEmpty(PID_Info_t *str);
void RUD_Stop(void);
void RUD_FirstAngle(Rudder_Axis_Info_t *str);
void Axis_Move_Angle_Init(void);//ǰ���Ƕ�ͬ��
void Axis_SPIN_Angle_Init(void);//��ת��ͬ��
float Get_RC_dir_Angle_XY(void);
float Report_Fusion_Speed(void);
void RUD_Get_F_Angle_GYRO(void);
void RUD_Get_F_Angle_MECH(void);
void RUD_Get_Z_Angle_GYRO(void);
void RUD_Get_Z_Angle_MECH(void);
void RUD_Angle_Fusion_XYZ(void);
bool Report_IF_RUNNING(void);
void Judge_IF_RUNNING( float speed);
void Judge_Z_LR_Dir(Rudder_Axis_Info_t *str , float CH0);
bool Judge_IF_SPIN_Dir(void);
float Report_Fusion_Speed_LF(void);
float Report_Fusion_Speed_RF(void);
float Report_Fusion_Speed_LB(void);
float Report_Fusion_Speed_RB(void);

float Movedir_Z_speed_Proc(float CH0);

void RUD_SPIN(void);
float SPIN_Speed(void);
float Move_SPIN_Speed(void);

#endif 



