//
// Created by 86139 on 2022/11/16.
//

#ifndef TINKER_MOTOR_DEBUG_PID_H
#define TINKER_MOTOR_DEBUG_PID_H

#define LIMIT_MIN_MAX(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

typedef struct
{
    float kp;
    float ki;
    float kd;
    float i_max;
    float out_max;
    float ref;
    float fdb;
    float err[2];
    float p_out;
    float i_out;
    float d_out;
    float output;
}pid_struct_t;

typedef struct
{
    pid_struct_t outer;
    pid_struct_t inner;
    float output;
}pid_Cascade_t;

void pid_init(pid_struct_t *pid,float kp,float ki,float kd,float i_max,float out_max);
float pid_calc(pid_struct_t *pid, float ref, float fdb);
float pid_CascadeCalc(pid_Cascade_t *pid,float angleRef,float angleFdb,float speedFdb);

#endif //TINKER_MOTOR_DEBUG_PID_H
