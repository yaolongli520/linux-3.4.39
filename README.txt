编译进去 build 目录进行 make 

要使用 3.5以上版本的cmake 所以要 16.04的ubuntu


source insight 工程只包含 src 和 include 的文件

github 中不包含 source insight 的目录

计划:	
分离硬件控制把开灯关灯单独出来一个进程!!!!
好处是不论其他任务如温湿度的异常导致应用崩溃,
仍然保留了核心的开关灯程序！！！


3.MFC控制功能
4.LCD需实现可以关闭背光的效果 闪烁的蓝色灯要关闭