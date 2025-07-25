
/* 头文件 */
#include "demo.h"
#include "key.h"
#include "rs485.h"
#include "usart.h"



/****************************************************************************************************
 * 函数名称： void TestOpr(void)
 * 入口参数：无
 * 返回  值：无
 * 功能说明：用按键模拟实现 和从机通读测试，从机可用PC机的串口调试助手来接收，应答等操作
 *           READ按键按下后，向从机(PC模拟)发送读命令  从机(PC端模拟应答)后，按键再按方有效
 *			 WRITE按键按下后，向从机(PC模拟)发送写命令 从机(PC端模拟应答)后，按键再按方有效
 ***************************************************************************************************/
void TestOpr(void)
{
	u8 ucKeyVal = 0;
	
	if (!RS485Busy)								//判断485总线状态 0空闲 1忙
	{
		ucKeyVal = KeyScan(0);					//获取按键值
		if (ucKeyVal == PRES_READ)				//按下读按键
		{
			RS485_RW_Opr(0x01,READ,0X01,0X01);	//向地址为01的从机发送读命令，读取从机寄存器01保存的数据 长度为1个字节
		}
		else if (ucKeyVal == PRES_WRITE)		//按下写按键
		{
			RS485_RW_Opr(0x01,WRITE,0X01,0x55);	//向地址为01的从机发送写命令，向从机寄存器01的地方写入0x55
		}
	}
}


/* END */
