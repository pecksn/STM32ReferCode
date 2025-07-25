/**
 **********************************************************************************************************
 * 实验简介
 * 实验名称：ATK-MW8266D模块TCP透传实验
 * 实验平台：正点原子 MiniSTM32 V4开发板
 * 实验目的：学习ATK-MW8266D模块TCP透传的使用方法

 **********************************************************************************************************
 * 硬件资源及引脚分配
 * 1 LED灯
     DS0（RED）     : LED0 - PA8
 * 2 独立按键
     KEY0    - PC5
     KEY1    - PA15
 * 3 串口1 (PA9/PA10连接在板载USB转串口芯片CH340上面)
 * 4 ALIENTEK 2.8/3.5/4.3/7/10寸TFTLCD模块(仅限MCU屏，16位8080并口驱动)
 * 5 ATK-MW8266D模块

 **********************************************************************************************************
 * 实验现象
 * 1 程序运行后，自动配置ATK-MW8266D模块连接WIFI，在连接成功后，通过串口和LCD显示IP地址，并连接TCP服务器
 * 2 通过按键1，可以切换透传状态
 * 3 默认状态下不开启透传，此时按下按键0，可以进行AT指令的测试，测试结果通过串口打印
 * 4 开启透传后，按下按键0，可发送数据至TCP服务器，同时会通过串口实时打印接收自TCP服务器的数据

 **********************************************************************************************************
 * 注意事项
 * 1 程序默认设置串口1的波特率为115200bps,电脑端串口调试助手波特率必须设置为115200bsp与之匹配才能收发正常的数据
 * 2 请使用XCOM/SSCOM串口调试助手,其他串口助手可能控制DTR/RTS导致MCU复位/程序不运行
 * 3 串口输入字符串以回车换行结束
 * 4 请用USB线连接在USB_UART,找到USB转串口后测试本例程
 * 5 本例程仅支持MCU屏，不支持RGB屏

 **********************************************************************************************************
 * 公司名称：广州市星翼电子科技有限公司（正点原子）
 * 电话号码：020-38271790
 * 传真号码：020-36773971
 * 公司网址：www.alientek.com
 * 购买地址：zhengdianyuanzi.tmall.com
 * 技术论坛：http://www.openedv.com/forum.php
 * 最新资料：www.openedv.com/docs/index.html
 *
 * 在线视频：www.yuanzige.com
 * B 站视频：space.bilibili.com/394620890
 * 公 众 号：mp.weixin.qq.com/s/y--mG3qQT8gop0VRuER9bw
 * 抖    音：douyin.com/user/MS4wLjABAAAAi5E95JUBpqsW5kgMEaagtIITIl15hAJvMO8vQMV1tT6PEsw-V5HbkNLlLMkFf1Bd
 **********************************************************************************************************
 */