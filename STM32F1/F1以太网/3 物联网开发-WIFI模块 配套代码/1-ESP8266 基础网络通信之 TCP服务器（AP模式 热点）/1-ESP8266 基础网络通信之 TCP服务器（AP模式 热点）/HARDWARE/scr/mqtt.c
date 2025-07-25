/*-------------------------------------------------*/
/*            ModuleStar STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*            实现MQTT协议功能的源文件             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"    //包含需要的头文件
#include "mqtt.h"         //包含需要的头文件
#include "string.h"       //包含需要的头文件
#include "stdio.h"        //包含需要的头文件
#include "usart1.h"       //包含需要的头文件
#include "wifi.h"         //包含需要的头文件

unsigned char  MQTT_RxDataBuf[R_NUM][BUFF_UNIT];            //数据的接收缓冲区,所有服务器发来的数据，存放在该缓冲区,缓冲区第一个字节存放数据长度
unsigned char *MQTT_RxDataInPtr;                            //指向接收缓冲区存放数据的位置
unsigned char *MQTT_RxDataOutPtr;                           //指向接收缓冲区读取数据的位置
unsigned char *MQTT_RxDataEndPtr;                           //指向接收缓冲区结束的位置

unsigned char  MQTT_TxDataBuf[T_NUM][BUFF_UNIT];            //数据的发送缓冲区,所有发往服务器的数据，存放在该缓冲区,缓冲区第一个字节存放数据长度
unsigned char *MQTT_TxDataInPtr;                            //指向发送缓冲区存放数据的位置
unsigned char *MQTT_TxDataOutPtr;                           //指向发送缓冲区读取数据的位置
unsigned char *MQTT_TxDataEndPtr;                           //指向发送缓冲区结束的位置

unsigned char  MQTT_CMDBuf[C_NUM][BUFF_UNIT];               //命令数据的接收缓冲区
unsigned char *MQTT_CMDInPtr;                               //指向命令缓冲区存放数据的位置
unsigned char *MQTT_CMDOutPtr;                              //指向命令缓冲区读取数据的位置
unsigned char *MQTT_CMDEndPtr;                              //指向命令缓冲区结束的位置

char ClientID[128];                                          //存放客户端ID的缓冲区
int  ClientID_len;                                           //存放客户端ID的长度

char Username[128];                                          //存放用户名的缓冲区
int  Username_len;											 //存放用户名的长度

char Passward[128];                                          //存放密码的缓冲区
int  Passward_len;											 //存放密码的长度

char ServerIP[128];                                          //存放服务器IP或是域名
int  ServerPort;                                             //存放服务器的端口号

int   Fixed_len;                       					     //固定报头长度
int   Variable_len;                     					 //可变报头长度
int   Payload_len;                       					 //有效负荷长度
unsigned char  temp_buff[BUFF_UNIT];						 //临时缓冲区，构建报文用

char Ping_flag;           //ping报文状态      0：正常状态，等待计时时间到，发送Ping报文
                          //ping报文状态      1：Ping报文已发送，当收到 服务器回复报文的后 将1置为0
char Connect_flag;        //同服务器连接状态  0：还没有连接服务器  1：连接上服务器了
char ConnectPack_flag;    //CONNECT报文状态   1：CONNECT报文成功
char SubcribePack_flag;   //订阅报文状态      1：订阅报文成功

/*----------------------------------------------------------*/
/*函数名：初始化接收,发送,命令数据的 缓冲区 以及各状态参数  */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_Buff_Init(void)
{	
	MQTT_RxDataInPtr=MQTT_RxDataBuf[0];               //指向发送缓冲区存放数据的指针归位
	MQTT_RxDataOutPtr=MQTT_RxDataInPtr;               //指向发送缓冲区读取数据的指针归位
    MQTT_RxDataEndPtr=MQTT_RxDataBuf[R_NUM-1];        //指向发送缓冲区结束的指针归位
	
	MQTT_TxDataInPtr=MQTT_TxDataBuf[0];               //指向发送缓冲区存放数据的指针归位
	MQTT_TxDataOutPtr=MQTT_TxDataInPtr;               //指向发送缓冲区读取数据的指针归位
	MQTT_TxDataEndPtr=MQTT_TxDataBuf[T_NUM-1];        //指向发送缓冲区结束的指针归位
	
	MQTT_CMDInPtr=MQTT_CMDBuf[0];                     //指向命令缓冲区存放数据的指针归位
	MQTT_CMDOutPtr=MQTT_CMDInPtr;                     //指向命令缓冲区读取数据的指针归位
	MQTT_CMDEndPtr=MQTT_CMDBuf[C_NUM-1];              //指向命令缓冲区结束的指针归位
	
	Connect_flag = 0;                                      //各个参数清零
	Ping_flag = ConnectPack_flag = SubcribePack_flag = 0;  //各个参数清零
}
/*----------------------------------------------------------*/
/*函数名：重新初始化接收,发送,命令缓冲区 以及各状态参数     */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_Buff_ReInit(void)
{			
	unsigned char *MQTT_TxDatatempPtr;                 //指向发送缓冲区位置的临时指针
	
	if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr){         //if成立的话，说明发送缓冲区有数据了
		MQTT_TxDataOutPtr = MQTT_TxDataInPtr;          //OUT指针指向IN指针
		if(MQTT_TxDataOutPtr==MQTT_TxDataBuf[0]){      //如果，现在OUT指针在缓冲区顶部，进入if
			MQTT_TxDataOutPtr =MQTT_TxDataBuf[T_NUM-4];//重定位OUT指针
		}else if(MQTT_TxDataOutPtr==MQTT_TxDataBuf[1]){//如果，现在OUT指针在缓冲区顶部下一个单元，进入if
		    MQTT_TxDataOutPtr =MQTT_TxDataBuf[T_NUM-3];//重定位OUT指针
		}else if(MQTT_TxDataOutPtr==MQTT_TxDataBuf[2]){//如果，现在OUT指针在缓冲区顶部下两个单元，进入if
		    MQTT_TxDataOutPtr =MQTT_TxDataBuf[T_NUM-2];//重定位OUT指针
		}else{
			MQTT_TxDataOutPtr -= BUFF_UNIT;            //OUT指针上移一个单元
			MQTT_TxDataOutPtr -= BUFF_UNIT;            //OUT指针上移一个单元
			MQTT_TxDataOutPtr -= BUFF_UNIT;            //OUT指针上移一个单元
		}			
		MQTT_TxDatatempPtr = MQTT_TxDataInPtr;         //将当前IN指针的位置暂存在temp指针中
		MQTT_TxDataInPtr = MQTT_TxDataOutPtr;          //IN指针指向当前OUT指针
		MQTT_ConectPack();                             //发送缓冲区添加连接报文
		MQTT_Subscribe(S_TOPIC_NAME,0);	               //发送缓冲区添加订阅topic，等级0									
		MQTT_TxDataInPtr = MQTT_TxDatatempPtr;         //IN指针通过temp指针，返回原来的位置		
	}else{                                             //反之，说明发送缓冲区没有数据
		MQTT_ConectPack();                             //发送缓冲区添加连接报文
		MQTT_Subscribe(S_TOPIC_NAME,0);	               //发送缓冲区添加订阅topic，等级0		                                
	}
	Ping_flag = ConnectPack_flag = SubcribePack_flag = 0;  //各个参数清零
}
/*----------------------------------------------------------*/
/*函数名：OneNet云初始化参数，得到客户端ID，用户名和密码    */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void OneNetIoT_Parameter_Init(void)
{	
	memset(ClientID,0,128);                              //客户端ID的缓冲区全部清零
	sprintf(ClientID,"%s",DEVICEID);                     //构建客户端ID，并存入缓冲区
	ClientID_len = strlen(ClientID);                     //计算客户端ID的长度
	
	memset(Username,0,128);                              //用户名的缓冲区全部清零
	sprintf(Username,"%s",PRODUCTID);                    //构建用户名，并存入缓冲区
	Username_len = strlen(Username);                     //计算用户名的长度
	
	memset(Passward,0,128);                              //用户名的缓冲区全部清零
	sprintf(Passward,"%s",AUTHENTICATION);               //构建密码，并存入缓冲区
	Passward_len = strlen(Passward);                     //计算密码的长度
	
	memset(ServerIP,0,128);  
	sprintf(ServerIP,"%s","183.230.40.39");              //构建服务器域名
	ServerPort = 6002;                                   //服务器端口号6002
	
	u1_printf("服 务 器：%s:%d\r\n",ServerIP,ServerPort); //串口输出调试信息
	u1_printf("客户端ID：%s\r\n",ClientID);               //串口输出调试信息
	u1_printf("用 户 名：%s\r\n",Username);               //串口输出调试信息
	u1_printf("密    码：%s\r\n",Passward);               //串口输出调试信息
}

/*----------------------------------------------------------*/
/*函数名：连接服务器报文                                    */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_ConectPack(void)
{	
	Fixed_len = 2;                                                        //连接报文中，固定报头长度=2
	Variable_len = 10;                                                    //连接报文中，可变报头长度=10
	Payload_len = 2 + ClientID_len + 2 + Username_len + 2 + Passward_len; //总报文长度       
	
	temp_buff[0]=0x10;                              //第1个字节 ：固定0x01	
	temp_buff[1]=Variable_len + Payload_len;        //第2个字节 ：可变报头+有效负荷的长度
	temp_buff[2]=0x00;          				    //第3个字节 ：固定0x00	            
	temp_buff[3]=0x04;                              //第4个字节 ：固定0x04
	temp_buff[4]=0x4D;								//第5个字节 ：固定0x4D
	temp_buff[5]=0x51;								//第6个字节 ：固定0x51
	temp_buff[6]=0x54;								//第7个字节 ：固定0x54
	temp_buff[7]=0x54;								//第8个字节 ：固定0x54
	temp_buff[8]=0x04;								//第9个字节 ：固定0x04
	temp_buff[9]=0xC2;								//第10个字节：使能用户名和密码校验，不使用遗嘱，不保留会话
	temp_buff[10]=0x00; 						    //第11个字节：保活时间高字节 0x00
	temp_buff[11]=0x64;								//第12个字节：保活时间高字节 0x64   100s
	
	/*     CLIENT_ID      */
	temp_buff[12] = ClientID_len/256;                			  			//客户端ID长度高字节
	temp_buff[13] = ClientID_len%256;               			  			//客户端ID长度低字节
	memcpy(&temp_buff[14],ClientID,ClientID_len);                 			//复制过来客户端ID字串	
	/*     用户名        */
	temp_buff[14+ClientID_len] = Username_len/256; 				  		    //用户名长度高字节
	temp_buff[15+ClientID_len] = Username_len%256; 				 		    //用户名长度低字节
	memcpy(&temp_buff[16+ClientID_len],Username,Username_len);              //复制过来用户名字串	
	/*      密码        */
	temp_buff[16+ClientID_len+Username_len] = Passward_len/256;			    //密码长度高字节
	temp_buff[17+ClientID_len+Username_len] = Passward_len%256;			    //密码长度低字节
	memcpy(&temp_buff[18+ClientID_len+Username_len],Passward,Passward_len); //复制过来密码字串

	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);      //加入发送数据缓冲区
}
/*----------------------------------------------------------*/
/*函数名：SUBSCRIBE订阅topic报文                            */
/*参  数：QoS：订阅等级                                     */
/*参  数：topic_name：订阅topic报文名称                     */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_Subscribe(char *topic_name, int QoS)
{	
	Fixed_len = 2;                              //SUBSCRIBE报文中，固定报头长度=2
	Variable_len = 2;                           //SUBSCRIBE报文中，可变报头长度=2	
	Payload_len = 2 + strlen(topic_name) + 1;   //计算有效负荷长度 = 2字节(topic_name长度)+ topic_name字符串的长度 + 1字节服务等级
	
	temp_buff[0]=0x82;                                    //第1个字节 ：固定0x82                      
	temp_buff[1]=Variable_len + Payload_len;              //第2个字节 ：可变报头+有效负荷的长度	
	temp_buff[2]=0x00;                                    //第3个字节 ：报文标识符高字节，固定使用0x00
	temp_buff[3]=0x01;		                              //第4个字节 ：报文标识符低字节，固定使用0x01
	temp_buff[4]=strlen(topic_name)/256;                  //第5个字节 ：topic_name长度高字节
	temp_buff[5]=strlen(topic_name)%256;		          //第6个字节 ：topic_name长度低字节
	memcpy(&temp_buff[6],topic_name,strlen(topic_name));  //第7个字节开始 ：复制过来topic_name字串		
	temp_buff[6+strlen(topic_name)]=QoS;                  //最后1个字节：订阅等级
	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //加入发送数据缓冲区
}
/*----------------------------------------------------------*/
/*函数名：PING报文，心跳包                                  */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_PingREQ(void)
{
	temp_buff[0]=0xC0;              //第1个字节 ：固定0xC0                      
	temp_buff[1]=0x00;              //第2个字节 ：固定0x00 

	TxDataBuf_Deal(temp_buff, 2);   //加入数据到缓冲区
}
/*----------------------------------------------------------*/
/*函数名：等级0 发布消息报文                                */
/*参  数：topic_name：topic名称                             */
/*参  数：data：数据                                        */
/*参  数：data_len：数据长度                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_PublishQs0(char *topic, char *data, int data_len)
{	
	Fixed_len = 2;                             //固定报头长度：2字节
	Variable_len = 2 + strlen(topic);          //可变报头长度：2字节(topic长度)+ topic字符串的长度
	Payload_len = data_len;                    //有效负荷长度：就是data_len
	
	temp_buff[0]=0x30;                         //第1个字节 ：固定0xC0                
	temp_buff[1]=Variable_len + Payload_len;   //第2个字节 ：可变报头+有效负荷的长度	
	temp_buff[2]=strlen(topic)/256;            //第3个字节 ：topic长度高字节
	temp_buff[3]=strlen(topic)%256;		       //第4个字节 ：topic长度低字节
	memcpy(&temp_buff[4],topic,strlen(topic)); //第5个字节开始 ：拷贝topic字符串	
	memcpy(&temp_buff[4+strlen(topic)],data,data_len);   //拷贝data数据
	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //加入发送数据缓冲区
}
/*----------------------------------------------------------*/
/*函数名：处理服务器发来的等级0的推送                       */
/*参  数：redata：接收的数据                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_DealPushdata_Qs0(unsigned char *redata)
{
	int  re_len;               	           //定义一个变量，存放接收的数据总长度
	int  pack_num;                         //定义一个变量，当多个推送一起过来时，保存推送的个数
    int  temp_len;                         //定义一个变量，暂存数据
    int  totle_len;                        //定义一个变量，存放已经统计的推送的总数据量
	int  topic_len;              	       //定义一个变量，存放推送中主题的长度
	int  cmd_len;                          //定义一个变量，存放推送中包含的命令数据的长度
	int  cmd_loca;                         //定义一个变量，存放推送中包含的命令的起始位置
	int  i;                                //定义一个变量，用于for循环
	unsigned char temp[BUFF_UNIT];	       //临时缓冲区
	unsigned char *data;                   //redata过来的时候，第一个字节是数据总量，data用于指向redata的第2个字节，真正的数据开始的地方
		
	re_len = redata[0];                                   			//获取接收的数据总长度
	data = &redata[1];                                              //data指向redata的第2个字节，真正的数据开始的 
	pack_num = temp_len = totle_len = 0;                			//各个变量清零
	do{
		pack_num++;                                     			//开始循环统计推送的个数，每次循环推送的个数+1
		temp_len = data[1+totle_len]+2;                 			//计算本次统计的推送的数据长度
		totle_len += temp_len;                          			//累计统计的总的推送的数据长度
		re_len -= temp_len ;                            			//接收的数据总长度 减去 本次统计的推送的总长度      
	}while(re_len!=0);                                  			//如果接收的数据总长度等于0了，说明统计完毕了
	if(pack_num!=1)u1_printf("本次接收了%d个推送数据\r\n",pack_num);//串口输出信息
	temp_len = totle_len = 0;                		            	//各个变量清零
	for(i=0;i<pack_num;i++){                                        //已经统计到了接收的推送个数，开始for循环，取出每个推送的数据 
		topic_len = data[2+totle_len]*256+data[3+totle_len] + 2;    //计算本次推送数据中主题占用的数据量
		cmd_len = data[1+totle_len]-topic_len;                      //计算本次推送数据中命令数据占用的数据量
		cmd_loca = totle_len + 2 +  topic_len;                      //计算本次推送数据中命令数据开始的位置
		memcpy(temp,&data[cmd_loca],cmd_len);                       //命令数据拷贝出来		                 
		CMDBuf_Deal(temp, cmd_len);                                 //加入命令到缓冲区
		temp_len = data[1+totle_len]+2;                             //记录本次推送的长度
		totle_len += temp_len;                                      //累计已经统计的推送的数据长度
	}	
}
/*----------------------------------------------------------*/
/*函数名：处理发送缓冲区                                    */
/*参  数：data：数据                                        */
/*参  数：size：数据长度                                    */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void TxDataBuf_Deal(unsigned char *data, int size)
{
	memcpy(&MQTT_TxDataInPtr[1],data,size);      //拷贝数据到发送缓冲区
	MQTT_TxDataInPtr[0] = size;                  //记录数据长度
	MQTT_TxDataInPtr+=BUFF_UNIT;                 //指针下移
	if(MQTT_TxDataInPtr==MQTT_TxDataEndPtr)      //如果指针到缓冲区尾部了
		MQTT_TxDataInPtr = MQTT_TxDataBuf[0];    //指针归位到缓冲区开头
}
/*----------------------------------------------------------*/
/*函数名：处理命令缓冲区                                    */
/*参  数：data：数据                                        */
/*参  数：size：数据长度                                    */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void CMDBuf_Deal(unsigned char *data, int size)
{
	memcpy(&MQTT_CMDInPtr[1],data,size);      //拷贝数据到命令缓冲区
	MQTT_CMDInPtr[0] = size;                  //记录数据长度
	MQTT_CMDInPtr[size+1] = '\0';             //加入字符串结束符
	MQTT_CMDInPtr+=BUFF_UNIT;                 //指针下移
	if(MQTT_CMDInPtr==MQTT_CMDEndPtr)         //如果指针到缓冲区尾部了
		MQTT_CMDInPtr = MQTT_CMDBuf[0];       //指针归位到缓冲区开头
}
