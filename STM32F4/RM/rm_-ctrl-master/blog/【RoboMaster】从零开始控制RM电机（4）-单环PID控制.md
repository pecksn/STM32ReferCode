> 硬件以及软件环境：
>
> STM32Cube_FW_F4_V1.26.2
>
> MDK-ARM 5.29.0.0
>
> 大疆RoboMaster开发板A型开发板（STM32F427IIHx）/C型开发板（STM32F407IGTx）
>
> 源码：
>
> [RM_ctrl](https://gitee.com/zhang-how/rm_-ctrl.git)
>
> 本系列文章目录：
> [【RoboMaster】从零开始控制RM电机（2）-CAN通信原理及电调通信协议](https://blog.csdn.net/weixin_54448108/article/details/125881138?spm=1001.2014.3001.5501)
>
> [【RoboMaster】从零开始控制RM电机（3）- 建立与电调的通信](https://blog.csdn.net/weixin_54448108/article/details/128570593?spm=1001.2014.3001.5501)
>
> [【RoboMaster】从零开始控制RM电机（4）-单环PID控制](https://blog.csdn.net/weixin_54448108/article/details/128596671?spm=1001.2014.3001.5501)
>
> 注：本系列文章同步更新RoboMaster电控学习的相关知识、分享开源代码以及对代码的部分解释到这一个专栏中。有需要的朋友可以点一下专栏或者作者的关注，方便之后查找。

@[TOC]

上一篇文章中，最终可以使用CAN通信与RM系列点击进行通信，并且进行开环控制。本文将通过PID控制器实现电机的闭环控制方法。

# 一、PID及其离散化表示

## 1.1 模拟PID控制器

通过自动控制原理课程的学习，可以了解到在自动调节的发展历程中，PID 的创立是非常重要的一环。PID即对输入偏差进行比例积分微分运算，最后使用运算的叠加结果去控制执行机构。

![image-20230107214930188](https://gitee.com/zhang-how/image_bed/raw/master/blog_image/a/aa/image-20230107214930188.png)

常规的模拟 PID 控制系统原理框图如上图所示。该系统由模拟PID控制器和被控对象组成。图中，r(t) 是给定值，y(t) 是系统的实际输出值，给定值与实际输出值构成控制偏差e(t)，且e(t)=r(t) -y(t) ，e(t)作为 PID 控制器的输入，u(t) 作为 PID 控制器的输出和被控对象的输入。所以可以得到模拟 PID 控制器的控制规律为

​                                                        $\mathrm{u}(t)=K p\left[e(t)+\frac{1}{T i} \int_0^t e(t) d t+T d \frac{d e(t)}{d t}\right]$

其中： *Kp* 为控制器的比例系数， Ti为控制器的积分时间，也称积分系数，Td为控制器的微分时间，也称微分系数

## 1.2 数字PID控制器

由于该控制系统最终将用于STM32单片机上运行，所以需要将连续的模拟系统转换为离散的数字系统。数字式 PID 控制器可以分为位置式 PID 和增量式 PID 控制算法。位置式PID控制器每次输出均与过去状态有关，计算时要对e(k)  进行累加，控制系统计算工作量大，相比之下增量式PID的计算工作量较小。

### 1.2.1 位置式PID控制

在模拟PID的模型基础上，以T作为采样周期，k作为采样序号，则离散采样时间kT对应着连续时间t，用矩形法数值积分近似代替积分，用一阶后向差分近似代替微分，最终得到数字位置式PID控制系统如下：

​                                                         $u_k=K p\left[e_k+\frac{T}{T i} \sum_{j=0}^k e_j+T d \frac{e_k-e_{k-1}}{T}\right]$

或

​                                                     $u_k=K p * e_k+K i \sum_{j=0}^k e_j+K d\left(e_k-e_{k-1}\right)$

上式即为位置式PID的实际控制模型，接下来的PID控制器代码也将依据该模型构建。

### 1.2.2 增量式PID控制

增量式PID控制器即控制器输出为控制两种增量$\Delta u_k$。具体推导过程如下式：

$\Delta u_k=u_k-u_{k-1}=K p\left(e_k-e_{k-1}+\frac{T}{T i} e_k+T d \frac{e_k-2 e_{k-1}+e_{k-2}}{T}\right)$
$\left.=K p\left(1+\frac{T}{T i}+\frac{T d}{T}\right) e_k-K p\left(1+\frac{2 T d}{T}\right) e_{k-1}+K p \frac{T d}{T} e_{k-2}\right)$
$=A e_k+B e_{k-1}+C e_{k-2}$

其中，$\mathrm{~A}=K p\left(1+\frac{T}{T i}+\frac{T d}{T}\right) ;$
            $\mathrm{B}=K p\left(1+\frac{2 T d}{T}\right) ;$
            $\mathrm{C}=K p \frac{T d}{T}$。

上式即为增量式PID的实际控制模型，接下来的PID控制器代码也将依据该模型构建。

# 二、PID在电机控制中的应用

接下来将基于数字PID控制器的模型编写基于STM32的PID控制器代码。代码将同时包含增量式和位置式PID控制器的控制方式且由PID初始化确定控制器类型作为区分。

## 2.1 PID控制器代码

首先声明一个结构体用于存放PID控制器的数据：

```C
PID_typedef Motor_pid[14];
```

```c
typedef struct
{
	PID_mode mode;		
	float Kp;
	float Ki;
	float Kd;
	float Max_iout;	
	float Max_out;	
	float measure;
	float target;
	float DeadBand;	
	float Pout;
	float Iout;
	float Dout;
	float D_item;
	float error[3];
	float OUT;
}PID_typedef;
```

其中，PID_mode为一个枚举型变量类型，包含PID_POSITION_SPEED、PID_POSITION_ANGLE、PID_DELTA_SPEED，分别为位置式速度控制、位置式角度控制以及增量式速度控制三种模式。Max_iout、Max_out为控制器输出限幅，kp、ki、kd分别为PID控制器的比例系数、积分系数、微分系数。

PID控制器运算函数源码如下:

```C
float PID_calc(PID_typedef *PID, float measure, float target)
{
	if(PID == NULL)
		return 0;
	PID->error[2] = PID->error[1];
	PID->error[1] = PID->error[0];
	PID->measure = measure;
	PID->target = target;
	PID->error[0] =target - measure;
		if(PID->mode == PID_POSITION_SPEED || PID->mode == PID_POSITION_ANGLE){	
			if(PID->mode == PID_POSITION_ANGLE){
				if(PID->error[0]>4096)	PID->error[0]=PID->error[0]-8191;
				else if(PID->error[0]<-4096)	PID->error[0]=PID->error[0]+8191;
			}
			PID->Pout = PID->Kp * PID->error[0];
			PID->Iout += PID->Ki * PID->error[0];
			PID->D_item = (PID->error[0] - PID->error[1]);
			PID->Dout = PID->Kd * PID->D_item;
			LimitMax(PID->Iout,PID->Max_iout);
			PID->OUT = PID->Pout + PID->Iout + PID->Dout;
			LimitMax(PID->OUT,PID->Max_out);
		}
		else if(PID->mode == PID_DELTA_SPEED){
			PID->Pout = PID->Kp * (PID->error[0] - PID->error[1]);
			PID->Iout = PID->Ki * PID->error[0];
			PID->D_item = (PID->error[0] - 2.0f*PID->error[1] + PID->error[2]);
			PID->Dout = PID->Kd * PID->D_item;
			PID->OUT += PID->Pout + PID->Iout + PID->Dout;
			LimitMax(PID->OUT, PID->Max_out);
		}
	else{
        PID->OUT=0;
	}
	return PID->OUT;
}
```

其中，针对增量式和位置式做出判断，分别执行不同的流程进行运算；且位置式PID中针对位置式角度控制做出输入量的处理，原因为位置式控制时电机转子角度为0~8191，所以将输入量限制为-4096~+4096之间，避免输入量过大导致的控制系统输出过大最终电机失控。

## 2.2 实际应用

实际应用中只需要在程序初始化时调用PID控制器初始化函数PID_Init对每个控制对象进行初始化赋值即可。随后在固定的控制周期内循环调用PID控制器运算函数PID_calc得出控制量输出即可。下面给出M2006电机控制示例：

```C
void Motor_cmd_task(void const * argument)
{
	can_filter_init();
	PID_Total_Init();
	for(;;){
		spd = remote_control.ch1*5000/660;
		Motor_measure[0].Output = PID_calc(&Motor_pid[0],Motor_measure[0].speed,spd);
		Set_motor_cmd(&hcan1,First_STDID,Motor_measure[0].Output,0,0,0);
		osDelay(10);
	}
}
```

该任务为FreeRTOS的一个子任务，通过osDelay函数控制函数运行停止周期为10ms，即控制频率为100Hz。任务中CAN通信相关函数的解释已经在上一篇文章中给出，本文在其基础上加入PID控制器相关的函数。

# 三、总结

至此，RM电机控制系列文章基本结束，但是对于RM电机的控制系统设计而言并没有结束。电机速度、位置控制仅仅只是入门，云台控制所需的性能要求远不止如此，接下来会根据进度陆续进行串级PID控制、前馈控制等控制系统的验证，并验证PID控制系统参数整定的方式。