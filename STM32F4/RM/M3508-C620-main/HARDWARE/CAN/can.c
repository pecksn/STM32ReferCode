#include "can.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "Motor_Control.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//CAN驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/7
//版本：V1.1 
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved	
//********************************************************************************
//V1.1修改说明 20150528
//修正了CAN初始化函数的相关注释，更正了波特率计算公式
////////////////////////////////////////////////////////////////////////////////// 	 
 
//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024;  tq=(brp)*tpclk1
//波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
//则波特率为:36M/((8+9+1)*4)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败; 
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE 
	NVIC_InitTypeDef  		NVIC_InitStructure;
#endif

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PORTA时钟	                   											 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//初始化IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//初始化IO

	//CAN单元设置
	CAN_InitStructure.CAN_TTCM=DISABLE;			//非时间触发通信模式  
	CAN_InitStructure.CAN_ABOM=DISABLE;			//软件自动离线管理	 
	CAN_InitStructure.CAN_AWUM=DISABLE;			//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN_InitStructure.CAN_NART=ENABLE;			//禁止报文自动传送 
	CAN_InitStructure.CAN_RFLM=DISABLE;		 	//报文不锁定,新的覆盖旧的  
	CAN_InitStructure.CAN_TXFP=DISABLE;			//优先级由报文标识符决定 
	CAN_InitStructure.CAN_Mode= mode;	        //模式设置： mode:0,普通模式;1,回环模式; 
	//设置波特率
	CAN_InitStructure.CAN_SJW=tsjw;				//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; 			//Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;				//Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;        //分频系数(Fdiv)为brp+1	
	CAN_Init(CAN1, &CAN_InitStructure);        	//初始化CAN1 

	CAN_FilterInitStructure.CAN_FilterNumber=0;	//过滤器0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	//屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	//32位宽 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;	//32位ID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//激活过滤器0

	CAN_FilterInit(&CAN_FilterInitStructure);			//滤波器初始化
	
#if CAN_RX0_INT_ENABLE 
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);				//FIFO0消息挂号中断允许.		    

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // 主优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   
 
#if CAN_RX0_INT_ENABLE	//使能RX0中断
//中断服务函数			    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
 CanRxMsg RxMessage;
	
	static int32_t  RM3508R_ori = 0;
	static int32_t	RM3508Rtmp;
	static int32_t	RM3508R_base	= 0;
	static int32_t	RM3508Rtmp_pre	= 0;

	static float Goal_Speed = 0;
  u8 i;
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	
	switch(RxMessage.StdId)
	{
		case 0x0201 : {
      for(i=0; i < 8; ++i){
        //储存原始信息
        M3508[0].Original_Data[i] = RxMessage.Data[i];
      }
      //进行消息的转换
      Data_Convert(0);
			RM3508Rtmp = M3508[0].Angle;
			if ( RM3508Rtmp - RM3508Rtmp_pre > 4095 ){
				RM3508R_base -= 8191;
			}
			else if ( RM3508Rtmp - RM3508Rtmp_pre < -4095 ){
				RM3508R_base += 8191;
			}
      RM3508Rtmp_pre		= RM3508Rtmp;
			RM3508R_ori		    = RM3508R_base + RM3508Rtmp;
			M3508[0].CNT_ABS	= RM3508R_ori;
      break;
    }
    case 0x0202 : {
      for(i=0; i < 8; ++i){
        //储存原始信息
        M3508[1].Original_Data[i] = RxMessage.Data[i];
      }
      //进行消息的转换
      Data_Convert(1);
			
			//printf("0x202");
			break;
    }
		case 0x0203 : {
      for(i=0; i < 8; ++i){
        //储存原始信息
        M3508[2].Original_Data[i] = RxMessage.Data[i];
      }
      //进行消息的转换
      Data_Convert(2);
			//printf("%d\r\n",M3508[2].Speed);
			//printf("0x202");
			break;
    }
		case 0x0204 : {
      for(i=0; i < 8; ++i){
        //储存原始信息
        M3508[3].Original_Data[i] = RxMessage.Data[i];
      }
      //进行消息的转换
      Data_Convert(3);
			
			//printf("0x202");
			break;
    }
		case 0x0205 : {
      for(i=0; i < 8; ++i){
        //储存原始信息
        M3508[4].Original_Data[i] = RxMessage.Data[i];
      }
      //进行消息的转换
      Data_Convert(4);
			
			//printf("0x202");
			break;
    }
		case 0x0206 : {
      for(i=0; i < 8; ++i){
        //储存原始信息
        M3508[5].Original_Data[i] = RxMessage.Data[i];
      }
      //进行消息的转换
      Data_Convert(5);
			
			//printf("0x202");
			break;
    }
    
//    // 接受主控消息
//    case 0x01e0 >> 5 : {
//      Goal_Speed = *((float*)(&RxMessage.Data[0])); //主1
//      Goal_Speed *= 1000;
//			//printf("ID1:%d\r\n",(int32_t)Goal_Speed);
//		  M3508[0].PID.Goal_Speed =(int32_t) Goal_Speed;
//			//printf("Goal_Speed: %d\r\n",M3508[0].PID.Goal_Speed);
//			//printf("0x01e0\r\n");
//			
//			Goal_Speed=*((float*)(&RxMessage.Data[4]));//主2
//			Goal_Speed*=1000;
//			M3508[1].PID.Goal_Speed=(int32_t)Goal_Speed;
//			break;
//    }
//    case 0x02e0 >> 5 : {
//      Goal_Speed = *((float*)(&RxMessage.Data[0])); //
//      Goal_Speed *= 1000;
//			//printf("ID3:%d\r\n",(int32_t)Goal_Speed);
//		  M3508[2].PID.Goal_Speed =(int32_t) Goal_Speed;

//      Goal_Speed = *((float*)(&RxMessage.Data[4])); //
//      Goal_Speed *= 1000;
//			//printf("ID4:%d\r\n",(int32_t)Goal_Speed);
//		  M3508[3].PID.Goal_Speed =(int32_t) Goal_Speed;
//			break;
//    }
//    case 0x03e0 >> 5 : {
//      Goal_Speed = *((float*)(&RxMessage.Data[0])); //
//      Goal_Speed *= 1000;
//			//printf("ID5:%d\r\n",(int32_t)Goal_Speed);
//		  M3508[4].PID.Goal_Speed =(int32_t) Goal_Speed;

//      Goal_Speed = *((float*)(&RxMessage.Data[4])); //
//      Goal_Speed *= 1000;
//			//printf("ID6:%d\r\n",(int32_t)Goal_Speed);
//		  M3508[5].PID.Goal_Speed =(int32_t) Goal_Speed;
//			break;
//    }
    
  }
	return;
	
}
	
	
#endif

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 CAN1_SendMsg(u8* msg,u8 len,u32 ID)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=ID;			// 标准标识符 
	TxMessage.ExtId=ID;			// 设置扩展标示符 
	TxMessage.IDE=CAN_Id_Standard; 	// 标准帧
	TxMessage.RTR=CAN_RTR_Data;		// 数据帧
	TxMessage.DLC=len;				// 要发送的数据长度
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];			          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	if(i>=0XFFF)return 1;
	return 0;	 
}
//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//		 其他,接收的数据长度;
u8 Can_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//没有接收到数据,直接退出 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
    for(i=0;i<8;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}














