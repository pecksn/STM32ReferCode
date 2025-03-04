# RM_Ctrl

### 介绍
这是一个基于RM主控板的RM相关外设底层控制代码的仓库。
建立这个仓库并且重写底层的目的一是为了自己更好的理解底层配置，二来也是为了帮助后来的学弟学妹可以更快的入门RM相关的控制，可以将重心放在上层逻辑的控制上。   

相关专栏链接：[ RoboMaster电控_optimization-z的博客-CSDN博客](https://blog.csdn.net/weixin_54448108/category_11929090.html)

#### 文件架构

blog--博客文件（暂未完善）

RM_motor_test--RM电机控制底层，包含CAN收发处理和PID计算

RM_remote_control--RM遥控数据接收底层

RM_Robot_on_A--RM机器人控制底层（A板，带rtos）

RM_Robot_on_A_no_os--RM机器人控制底层（A板，不带rtos）

RM_Robot_on_C--RM机器人控制底层（C板，带rtos）



