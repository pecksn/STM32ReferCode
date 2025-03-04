@[TOC]

#  1、串口初始化配置

​		记录一下STM32使用CubeMX配置串口DMA通信的流程和模块协议解析代码，避免重复学习和造轮子，提高效率。

该代码和配置基于RoboMaster--A板，其他芯片和开发板也适用，改配置即可。

## 1.1CubeMX配置

​		由于是使用类型的笔记，所以不涉及DMA原理之类的内容。

### 1.1.1 项目总体配置

​		首先是配置RCC 、SYS、时钟频率、文件名、存储位置之类的基础配置。个人习惯打开一份新工程时首先一套下来把这些全配置好，防止后面有遗漏。这部分配置都是很基本的操作，配置完成的截图如下。

- **RCC时钟源**设置为外部高速时钟：

![image-20220807161826526](https://s2.loli.net/2022/08/07/PzbcKpjynHqt5Bl.png)

- **时钟频率**设为板子对应的频率（A板--168MHZ）：

![image-20220807161935190](https://s2.loli.net/2022/08/07/vHDL82tmhrwzTfk.png)

- **项目文件设置**（project manager）：

![image-20220807185225129](https://s2.loli.net/2022/08/07/4TVULh13SazlwYJ.png)

![image-20220807184249814](https://s2.loli.net/2022/08/07/hjyL2Wzw5tB9ilq.png)

### 1.1.2串口参数配置

##### **开启串口**

- 点击USART6

- 设置为异步通信

- 点击Parameter Setting

- 设置为115200波特率，数据长度8位，无校验，停止位1，接收发送模式

  （这里的配置是一般情况下的串口配置，**实际使用根据模块的通信协议设置**）

![image-20220807234621104](https://s2.loli.net/2022/08/07/UMrT7LajNJE8uGX.png)

##### DMA&NVIC

- 依次点击1->2->3添加DMA接收通道

![image-20220807235636878](https://s2.loli.net/2022/08/07/eHAF8xhu6Ip2LRV.png)

- 重复一次添加DMA发送通道（结果如下图1）
- 2处设置为循环模式
- 3处注意一下，一般情况下默认即可

![image-20220808000137242](https://s2.loli.net/2022/08/08/FlvfBU3ojYMJspZ.png)

- 开启中断，箭头指向的那个串口中断要勾选

  ![image-20220808000340197](https://s2.loli.net/2022/08/08/ieKHoOWRA5q8CzT.png)

**配置完成后直接点\< ==GENERATE CODE== >生成。**

## 1.2 代码

### 1.2.1 串口初始化

- main.c

  添加用户串口初始化函数,参数为模块对应串口的句柄

  ![image-20220809181214322](https://s2.loli.net/2022/08/09/mTFvGQAj5othRiD.png)

- bsp.c

  用户串口初始化函数

  ```c
  void GY33_init(UART_HandleTypeDef *huart)
  {
  	__HAL_UART_ENABLE_IT(huart,UART_IT_IDLE);
  	HAL_UART_Receive_DMA(huart,GY33_RBUF,DATA_LENS);
  }
  ```


### 1.2.2 串口中断数据处理

​		完成串口初始化后，当串口数据传输结束进入空闲状态时就会调用空闲中断函数进行数据处理，用户只要在串口中断加入自己的数据处理函数即可。

- stm32f4xx_it.c（简称it.c）

  在 it.c 内可以找到串口对应的中断处理函数，在里面调用用户的数据处理函数即可。

  如下图中`Color = GY33_decode_to_RGB(&huart6);`

![image-20220809182250904](https://s2.loli.net/2022/08/09/Rbifvu9zpcM7ZLB.png)

- bsp.c

  这里面是用户串口中断数据处理调用的函数的主体。

  在这里只列出数据接收部分函数，只是把串口数据接收到接收数组中，并没有进行后续的数据校验，数据处理等流程。例如运行图中这个函数后，只是在GY33_RBUF中存入了串口接收到的数据，并没有得到RGB值等数据。

  ```c
  RGB GY33_decode_to_RGB(UART_HandleTypeDef *huart)
  {
  	RGB_t color_;
  	uint8_t Last_lens,check;
  	if(__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE)==SET){
  		__HAL_UART_CLEAR_IDLEFLAG(huart);  //清除标志位，准备下次接收数据
  		HAL_UART_DMAStop(huart);										//停止dma传输
      Last_lens=DATA_LENS-hdma_usart6_rx.Instance->NDTR;//读取ndtr寄存器中dma已发送数据字节数，然后算出距离9字节还需要多少字节
      HAL_UART_Transmit_DMA(huart,GY33_RBUF,Last_lens); //将剩余字节发送到接收数组中
      //在此插入数据处理函数
  	}
  	HAL_UART_Receive_DMA(huart,GY33_RBUF,DATA_LENS);		//重启DMA
  	return color_;
  }
  ```
  
  

# 2.模块协议解析

## GY33颜色传感器-串口模式

#### 通信协议

首先看GY33的串口通信协议。

一个数据帧内可以包含8-13字节的数据，每个数据帧组成如下表所示（以获取MCU处理后的RGB值为例）

| 帧头 | 帧头 |     帧数据类型      | 数据长度 | 数据  | 校验和 |
| :--: | :--: | :-----------------: | :------: | :---: | :----: |
| 0x5A | 0x5A | 0x45（处理后RGB值） |   0x03   | RGB值 | ...... |

帧数据类型字节含义如下：

|   0x15    |         0x25         |       **0x45**       |   0x55   |
| :-------: | :------------------: | :------------------: | :------: |
| 原始 RGBC | 亮度、色温、简单颜色 | **MCU 处理后的 RGB** | IIC 地址 |

主要关注的就是MCU处理后RGB值，所以其他的不做过多介绍，只了解这一类型的数据帧。数据字节长度为3字节，字节内依次为RGB三原色的值，范围是0~255。

其他的通信指令可以通过查阅手册获取。

#### 数据处理

**GY33.c**

数据处理函数如下，首先判断数据帧前四帧的固定数据是否正确，正确的话就把数据存入RGB数组，然后判断校验和，校验和正确就调用颜色判断函数进行判断。

```c
if(GY33_RBUF[0]==0x5A&&GY33_RBUF[1]==0x5A&&GY33_RBUF[2]==0x45&&GY33_RBUF[3]==0x03){
			check=0XFC;							//前四个字节校验和
			for(uint8_t i=0;i<3;i++){
				RGB[i]=GY33_RBUF[i+4];			//数据存入RGB数组
				check+=GY33_RBUF[i+4];			//校验和计算
			}
			if(check==GY33_RBUF[7])
				color_ = GY33_RGB_to_Color(RGB);//校验和正确，调用颜色判断
			else 
				color_ = None;					//否则颜色返回None
		}
```

**GY33.c**

​		由于是在为一个比赛做准备，要求中只需要判定RGB三种颜色，所以没有判断出其他颜色，需要其他颜色的思路是通过标准色表得到一个颜色的数值区间在与RGB三值作比较得到结果。

​		颜色判断函数，若RGB均小于50为黑，均大于250为白，否则取RGB值最大的一项返回判定颜色值。

```c
RGB_t GY33_RGB_to_Color(uint8_t *buf)
{
	RGB_t col;
	if(buf[0]<50&&buf[1]<50&&buf[2]<50)
		col = Black;
	else if(buf[0]>250&&buf[1]>250&&buf[2]>250)
		col = White;
	else{
		if(buf[0]>buf[1]&&buf[0]>buf[2])
			col = Red;
		else if(buf[1]>buf[0]&&buf[1]>buf[2])
			col = Green;
		else if(buf[2]>buf[0]&&buf[2]>buf[1])
			col = Blue;
	}
	return col;
}
```

**GY33.h**

1. 定义数据长度
2. 通过枚举定义RGB数据类型

```c
#define DATA_LENS 8

typedef enum{
	Red =0,
	Green,
	Blue,
	White,
	Black,
	None
}RGB_t;
```



