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

为了建立起与电调的通信，我们需要使用到STM32的CAN通信功能，同时为了使编程更加高效，减少花在底层配置上的时间，这里使用了CUBAEMX+HAL库编程。

整体分为CUBEMX配置、初始化、报文收发处理三部分。

# 一、CUBEMX配置代码底层

## 1.1 基础配置

包含时钟源、系统配置、运行主频以及生成选项，这一部分配置与其他项目是相同的所以不上图了。

## 1.2 使能CAN并更改IO口

**分别**在CAN1和CAN2配置窗口勾选Activated开启CAN通信功能

由于A板上使用的CANIO口并不是CUBEMX默认IO，所以需要根据A型开发板原理图进行修改。修改后的IO如下：

![image-20220914124249633](https://gitee.com/zhang-how/image_bed/raw/master/blog/image-20220914124249633.png)

此处注意如果使用的是C型开发板需要根据C型开发板的原理图修改到对应的IO口，否则将导致C型开发板无法与电机进行通信。C型开发板修改后的IO对应如下图：

![image-20230105181417194](https://gitee.com/zhang-how/image_bed/raw/master/blog/image-20230105181417194.png)

## 1.3 CAN通信参数设置

在这里主要配置CAN通信的波特率。将里面的选项按下图步骤依次修改，先修改 Time Quanta（tq），再修改 Prescaler （分频系数）。不论是A型开发板或是C型开发板的运行频率都是168MHz，所以他们的波特率设置是相同的。

可以看到最终结果波特率一栏为1Mbit/s，即CAN总线支持的最高通讯波特率。

![image-20220914130434475](https://gitee.com/zhang-how/image_bed/raw/master/blog_image/a/aa/image-20220914130434475.png)

## 1.4 中断设置

分别为CAN1、CAN2接口开启数据发送中断和数据接收中断，以便在中断函数中处理收发的数据。CAN1、CAN2设置如下：

![image-20220914132200498](https://gitee.com/zhang-how/image_bed/raw/master/blog/image-20220914132200498.png)

![image-20220925170846941](https://gitee.com/zhang-how/image_bed/raw/master/blog/image-20220925170846941.png)

至此，在CUBEMX内对于STM32的双路CAN配置完成，在配置完成CAN通信之后若有需要开启FreeRTOS的需求则再在FreeRTOS配置中开启并为电机控制创建任务函数即可。最后点击Generate生成项目文件。

# 二、CAN通信初始化

经过CUBEMX的生成之后，可以得到一份完整的底层代码。进入Keil之后编译验证通过之后就可以进入下一步，CAN通信过滤器初始化。

由于项目中使用了双路CAN通信，所以需要对CAN通信过滤器进行配置。首先需要使能CAN过滤器，设置过滤器的工作模式位标识符屏蔽位模式且位宽为32位，并设置过滤器标识符寄存器和屏蔽位寄存器均为0。双路CAN模式下可选过滤器地址为0~27，本项目中将CAN1过滤器组起始设为0，CAN2过滤器组起始设为14。即CAN1使用地址为0~13的过滤器组，CAN2使用地址为14~27的过滤器组。

最后调用HAL_CAN_Start函数使能CAN控制器和HAL_CAN_ActivateNotification使能CAN中断即可。初始化函数如下：

```c
void can_filter_init(void)
{
	CAN_FilterTypeDef can_filter_st;	                //定义过滤器结构体
    can_filter_st.FilterActivation = ENABLE;			//ENABLE使能过滤器
    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;	//设置过滤器模式--标识符屏蔽位模式
    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;	//过滤器的位宽 32 位
    can_filter_st.FilterIdHigh = 0x0000;				//ID高位
    can_filter_st.FilterIdLow = 0x0000;					//ID低位
    can_filter_st.FilterMaskIdHigh = 0x0000;			//过滤器掩码高位
    can_filter_st.FilterMaskIdLow = 0x0000;				//过滤器掩码低位
    
    can_filter_st.FilterBank = 0;						//过滤器组-双CAN可指定0~27
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;	//与过滤器组管理的 FIFO
    HAL_CAN_ConfigFilter(&hcan1, &can_filter_st);		//HAL库配置过滤器函数
    HAL_CAN_Start(&hcan1);								//使能CAN1控制器
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);//使能CAN的各种中断

    can_filter_st.SlaveStartFilterBank = 14;   //双CAN模式下规定CAN的主从模式的过滤器分配，从过滤器为14
    can_filter_st.FilterBank = 14;						//过滤器组-双CAN可指定0~27
    HAL_CAN_ConfigFilter(&hcan2, &can_filter_st);		//HAL库配置过滤器函数
    HAL_CAN_Start(&hcan2);								//使能CAN2控制器
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);//使能CAN的各种中断
}
```

经过CAN过滤器初始化之后，STM32已经可以跟其他CAN设备进行通信了。接下来只需要根据上一篇文章的内容正确地处理CAN收发数据即可使用STM32控制其他CAN设备。

# 三、CAN通信报文解码收发

在收发数据处理之前，首先建立一个结构体用于存放电机的数据，包括前文所说的速度、角度、转矩电流、转子温度等数据，结构体定义如下：

```c
Motor_measure_t Motor_measure[14];
```

```c

typedef struct{
	int16_t	 	speed;
  int16_t  	torque_current;
	int16_t   Output;
  uint8_t  	temp;
	int16_t 	angle;				//abs angle range:[0,8191]
	int16_t 	last_angle;	        //abs angle range:[0,8191]
	int32_t		round_cnt;
	int32_t		total_angle;
}Motor_measure_t;
```

由于CAN初始化时已经开启了CAN接收中断，CAN接口接收到数据之后会调起中断回调函数HAL_CAN_RxFifo0MsgPendingCallback，我们只需要在这个函数中将接收到的数据进行处理并区分不同ID和对应CAN接口，然后调用Motor_measure_fun函数进行数据解析即可得到对应电机的反馈数据了。数据解析方法参考[RM电机CAN通信协议](https://blog.csdn.net/weixin_54448108/article/details/125881138)进行，上述两个函数源码如下：

```c
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef RX_Header;	//数据帧帧头
	uint8_t RX_BUFFER[8];	//接收存放数据帧数据的数组
	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&RX_Header,RX_BUFFER);	
	
	static uint8_t i = 0;
	if(hcan==&hcan1){		
		i = RX_Header.StdId - Chassis_3508A;		
		Motor_measure_fun(&Motor_measure[i], RX_BUFFER);	
	}
	else if(hcan==&hcan2){    
		i = RX_Header.StdId - CAN2_3508_ID1 + 7;	
		Motor_measure_fun(&Motor_measure[i], RX_BUFFER);	
	}
}

void Motor_measure_fun(Motor_measure_t *ptr,uint8_t* RX_buffer)
{
	ptr->last_angle = ptr->angle;		
	ptr->angle = (uint16_t)((RX_buffer)[0] << 8 | (RX_buffer)[1]); 	//转子机械角度
	ptr->speed = (uint16_t)((RX_buffer)[2] << 8 | (RX_buffer)[3]);	//转子转速(rpm)
	ptr->torque_current = (uint16_t)((RX_buffer)[4] << 8 | (RX_buffer)[5]);	//转矩电流
	ptr->temp = (RX_buffer)[6];         
	if(ptr->angle - ptr->last_angle > 4096)				
		ptr->round_cnt --;								
	else if(ptr->angle - ptr->last_angle < -4096)		
		ptr->round_cnt ++;
	ptr->total_angle = ptr->round_cnt * 8192 + ptr->angle;	
}
```

以上为CAN通信数据接收的数据解析部分，完成以上部分可以接收到电机的反馈数据。下一步为电机控制的数据发送部分，由电机的通信协议可知发送控制电流值时为每路CAN的低四位电机、高四位电机为一组进行控制，所以发送函数内将四个电机的控制电流一起发送到CAN总线上。该函数仅将数据进行处理之后调用HAL_CAN_AddTxMessage将数据传入发送邮箱并激活发送请求，函数源码如下：

```c
void Set_motor_cmd(CAN_HandleTypeDef *hcan,uint32_t STDID,int16_t motor1,int16_t motor2,int16_t motor3,int16_t motor4)
{
	uint32_t send_mail_box;	
	CANx_tx_message.StdId = STDID;			
	CANx_tx_message.IDE = CAN_ID_STD;		
    CANx_tx_message.RTR = CAN_RTR_DATA;	
    CANx_tx_message.DLC = 0x08;			
	
	CANx_send_data[0] = motor1 >> 8;		
	CANx_send_data[1] = motor1;
	CANx_send_data[2] = motor2 >> 8;
	CANx_send_data[3] = motor2;
	CANx_send_data[4] = motor3 >> 8;
	CANx_send_data[5] = motor3;
	CANx_send_data[6] = motor4 >> 8;
	CANx_send_data[7] = motor4;

	HAL_CAN_AddTxMessage(hcan,&CANx_tx_message, CANx_send_data,&send_mail_box);	
}
```

# 四、总结

至此，加入了接收解析函数和发送函数的代码即可接收电机的反馈数据并且可以向电机发送控制电流值。但是此时我们的控制电流值并没有与反馈数据相关联，即没有构成闭环系统，仅仅是一个开环控制系统，这并不能对电机的速度进行精准的控制，所以下一篇中将介绍使用PID控制电机的速度或者位置的方法。