/*************************************************************************************
文件名称：RS485.c
版    本：V1.0
日    期：2020-5-11
编    著：Eric Xie
说    明：串口相关设置
修改日志：

modbus协义基本格式说明：
	1.主机对从机写数据操作，从机接收到报文后对报文进行解析，然后执行相应的处理，同时需要向主机应答
	  为什么要应答？因为主机需要确认从机是否收到报文，然后可向从机发送其它的数据，执行其它的操作，
	  有些操作是基于上一次操作而产生的，所以需要应答，以保证系统的健壮性和稳定性。当然也可不理会
	  
	例：主机向从机发送一条报文格式如下(16进制)
		01		  06	 00 01	   00 17	 98 04
	  从机地址	功能号	数据地址	数据	CRC校验码	
	解：主机向地址为01的从机 寄存器地址为0001的地方 写入数据0017
	    从机可把这条报文原样返回作为应答，主机收到后表示写数据成功，可执行下一条操作
	
	2.主机对从机进行读数据操作，从机接收到报文后进行解析，然后根据报文的内容，把需要返回给主机的数
	  据返回给主机，返回的报文即同等于应答。
	
	例：主机向从机发送一条报文格式如下(16进制)
		 01		  03	 00 01		   00 01		 d5 ca
	  从机地址	功能号	数据地址	读取数据个数	CRC校验码
	解：假设从机寄存器组0001的地方存放的数据为aa，那么返回给主机的数据为 01 03 02 00 aa 38 3b
		主机收到后对这条报文解析或把读到的数据保存在指定的变量中即可。
		
备注说明：以上是基本通讯格式，这些数据可通过其它方式逻辑实现更多的功能，具体请自行研究

**************************************************************************************/   
#include "rs485.h"	 
#include "delay.h"
#include "crc16.h"
#include "usart.h"

/* 串口中断接收到数据的长度变量 */
static u8 USART_RX_CNT = 0;
/* 串口中断接收到数据保存的缓冲区 */
static u8 USART2_RX_BUF[64];
/* 用于标识串口接收数据包是否完成标志 */
static u8 From_Flag = 0;
/* 通讯标志 主机发送数据后置1 接收到应答后清零 */
u8 RS485Busy = 0;
/* 接收缓存区 */
u8 RS485_RX_BUF[64];  	//接收缓冲,最大64个字节.
/* 用于保存读命令获得的数据 */
u16 ReadDateVal = 0;
/* Modbus寄存器组，地址为0x00~0x04 */
u8 regGroup[5];  

/***************************************************************************************************
 * 函数说明: 485串口初始化
 * 入口参数：u32 bound
 * 出口参数：
 * 函数功能：初始化串口2 用于485通讯 
 *			 波特率要和从机的波特率一致
 *			 具体设置可以rs485.h是查看设置 对应硬件
***************************************************************************************************/ 
void RS485_Init(u32 bound)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RS485_GPIO_RCC | RS485_MODE_RCC, ENABLE);	//打开串口GPIO时钟 RS485控制引脚GPIO时钟 
	RCC_APB1PeriphClockCmd(RS485_USART_RCC,ENABLE);				//使能对应使用串口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);			//复用功能时钟，如果串口引脚为复用功能即需要有这一条

	GPIO_InitStructure.GPIO_Pin   = RS485_MODE_Pin;				//RS485模式控制引脚
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 			//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//翻转速度50MHz
	GPIO_Init(RS485_MODE_PROT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = RS485_TX_Pin;				//TXD引脚
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;			//复用推挽
	GPIO_Init(RS485_PROT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = RS485_RX_Pin;				//RXD引脚
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING; 		//浮空输入
	GPIO_Init(RS485_PROT, &GPIO_InitStructure);  
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; 			//使能串口2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 	//先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 			//从优先级2级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 							//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
 
	USART_InitStructure.USART_BaudRate = bound;					//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据长度
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式

	USART_Init(RS485_USART_NUM, &USART_InitStructure); ; 		//初始化串口
	USART_ITConfig(RS485_USART_NUM, USART_IT_RXNE, ENABLE);		//开启中断
	USART_Cmd(RS485_USART_NUM, ENABLE);                    		//使能串口 
	RS485_TX_EN = 0;											//默认为接收模式 
}

/****************************************************************************************************
 * 函数名称： void Send_Data(u8 *buf,u8 len)
 * 入口参数：u8 *buf u8 len
 * 返回  值：无
 * 功能说明：串口发送数据
 * 			 buf:发送区首地址
 *			 len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
 ***************************************************************************************************/
void Send_Data(u8 *buf,u8 len)
{
	u8 t;
	RS485_TX_EN=1;			//设置为发送模式
	delay_ms(1);
	for(t=0;t<len;t++)		//循环发送数据
	{		   
		while(USART_GetFlagStatus(RS485_USART_NUM, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART2,buf[t]);
	}	 
	delay_ms(1);			//IO转换稍作延时
	USART_RX_CNT=0;	  
	RS485_TX_EN=0;			//设置为接收模式	
}

/****************************************************************************************************
 * 函数名称：u8 UartRead(u8 *buf, u8 len) 
 * 入口参数：u8 *buf u8 len
 * 返回  值：u8
 * 功能说明：计算接收的数据长度，并且将数据放到*buf数组中
 ***************************************************************************************************/             
u8 UartRead(u8 *buf, u8 len)  
{
	u8 i;
	if(len > USART_RX_CNT)  		//指定读取长度大于实际接收到的数据长度时
	{
		len = USART_RX_CNT; 		//读取长度设置为实际接收到的数据长度
	}
	
	for(i = 0;i < len;i ++)  		//拷贝接收到的数据到接收指针中
	{
		*buf = USART2_RX_BUF[i];  	//将数据复制到buf中
		buf  ++;
	}
	USART_RX_CNT=0;              	//接收计数器清零
	return len;                   	//返回实际读取长度
}
/****************************************************************************************************
 * 函数名称：void UartRxMonitor(u8 ms)
 * 入口参数：u8 ms
 * 返回  值：无
 * 功能说明：在定时器中调用，用于监控数据接收
 ***************************************************************************************************/   
void UartRxMonitor(u8 ms) 					
{
	static u8 USART_RX_BKP = 0;  			//定义USART2_RC_BKP暂时存储诗句长度与实际长度比较
	static u8 idletmr      = 0;        		//定义监控时间
	
	if(USART_RX_CNT > 0)					//接收计数器大于零时，监控总线空闲时间
	{
		if(USART_RX_BKP != USART_RX_CNT) 	//接收计数器改变，即刚接收到数据时，清零空闲计时
		{
			USART_RX_BKP = USART_RX_CNT;  	//赋值操作，将实际长度给USART2_RX_BKP
			idletmr      = 0;               //将监控时间清零
		}
		else                              	//接收计数器未改变，即总线空闲时，累计空闲时间
		{
											//如果在一帧数据完成之前有超过3.5个字节时间的停顿，接收设备将刷新当前的消息并假定下一个字节是一个新的数据帧的开始
			if(idletmr < 5)                 //空闲时间小于3ms时，持续累加
			{
				idletmr += ms;
				if(idletmr >= 5)            //空闲时间达到3ms时，即判定为1帧接收完毕
				{
					From_Flag = 1;			//设置命令到达标志，帧接收完毕标志
				}
			}
		}
	}
	else
	{
		USART_RX_BKP = 0;
	}
}

/****************************************************************************************************
 * 函数名称：void UartRxMonitor(u8 ms)
 * 入口参数：u8 ms
 * 返回  值：无
 * 功能说明：串口驱动函数，检测数据帧的接收，调度功能函数，需在主循环中调用
 *           这里是从机，需要对主机发来的报文进行对应的处理，并返回应答
 *			 从机不主动发送数据，一切操作都是为了响应主机的报文
 ***************************************************************************************************/   
void UartDriver(void)
{
	unsigned char i;
	unsigned char cnt;
	
	unsigned int crc;
	unsigned char crch,crcl;
	static unsigned char len;
	static unsigned char buf[64];
	if (From_Flag)            									//帧接收完成标志，即接收到一帧新数据
	{
		From_Flag = 0;           								//帧接收完成标志清零
		len       = UartRead(buf,sizeof(buf));   				//将接收到的命令读到缓冲区中
		crc       = GetCRC16(buf,len-2);       					//计算CRC校验值，除去CRC校验值
		crch=crc  >> 8;    										//crc高位
		crcl=crc  &  0xFF;										//crc低位
		if((buf[len-2] == crch) && (buf[len-1] == crcl))  		//判断CRC校验是否正确
		{
			/* 在点对多点的应用中，这里还需要加上一条，判断报文的地址是否和本机地址一致，否则不作处理 */
			/* 可设置不同从机为不同地址，用以区分报文是发给谁的，相同则处理，不同则不作处理 */
			switch (buf[1])  									//按功能码执行操作
			{
				case 0x03:     									//读数据
					if((buf[2] == 0x00) && (buf[3] <= 0x05))  	//寄存器地址支持0x0000~0x0005
					{
						if(buf[3] <= 0x04) 
						{
							i      = buf[3];					//提取寄存器地址
							cnt    = buf[5];  					//提取待读取的寄存器数量
							buf[2] = cnt*2;  					//读取数据的字节数，为寄存器*2，因modbus定义的寄存器为16位
							len    = 3;							//响应帧第4个字节开始为数据
							while(cnt --)
							{
								buf[len ++] = 0x00;				//寄存器高字节补0
								buf[len ++] = regGroup[i ++];	//低字节
							}
						}
						break;
					}
					else  										//寄存器地址不被支持时，返回错误码
					{   
						buf[1] = 0x83;  						//功能码最高位置1
						buf[2] = 0x02;  						//设置异常码为02-无效地址
						len    = 3;
						break;
					}
				case 0x06:           							//写入单个寄存器
					if((buf[2] == 0x00) && (buf[3] <= 0x05))   	//寄存器地址支持0x0000-0x0005
					{
						if(buf[3] <= 0x04)
						{
							i           = buf[3];				//提取寄存器地址
							regGroup[i] = buf[5];				//保存寄存器数据
						}
						len -= 2;                				//长度-2以重新计算CRC并返回原报文
						break;
					}
					else  
					{											//寄存器地址不被支持，返回错误码
						buf[1] = 0x86;           				//功能码最高位置1
						buf[2] = 0x02;          				//设置异常码为02-无效地址
						len    = 3;
						break;
					}
				default:   						 				//其他不支持的功能码
					buf[1] = 0x80;     							//功能码最高位置1
					buf[2] = 0x01;     							//设置异常码为01—无效功能
					len    = 3;
					break;
			}
			crc = GetCRC16(buf,len);           					//计算CRC校验值
			buf[len ++] = crc >> 8;           					//CRC高字节
			buf[len ++] = crc & 0xff;        					//CRC低字节
			Send_Data(buf,len); 			 					//发送响应帧
		}
		else	/* 如果校验值错误，执行错误处理函数 这里用串口1打印一条提示信息 也可自定义共它处理函数 */
			printf("接收错误\r\n");
	}
}
/****************************************************************************************************
 * 函数名称：void UartRxMonitor(u8 ms)
 * 入口参数：无
 * 返回  值：无
 * 功能说明：串口接收中断处理函数
 ***************************************************************************************************/   
void USART2_IRQHandler(void)
{
	unsigned char ucTemp;	    
 	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 	//接收到数据
	{	 
//		USART_ClearITPendingBit(USART2, uint16_t USART_IT)
		ucTemp =USART_ReceiveData(USART2); 					//读取接收到的数据
		if(USART_RX_CNT < 64)
		{
			USART2_RX_BUF[USART_RX_CNT] = ucTemp;			//记录接收到的值
			USART_RX_CNT ++;								//接收数据增加1 
		} 
	}  											 
} 



















