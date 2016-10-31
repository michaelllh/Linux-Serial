
/*******************************************************
*ilename：test.c
* Description：Receive and send data
* 设置串口为115200 8 N 1 
* 默认使用ttsy1
*******************************************************/

/*********************头文件定义***********************/
#include <stdio.h> 
#include <pthread.h>
#include "uart.h"
#include <stdlib.h>  
/*******************************************************/



int main()
{
	int flag_close = 0;
	int ret_thread = 0;
	pthread_t id;		/*线程id*/
	open_serial(1);    	/*打开串口1*/
	intUart(fd);		/*初始化串口比特率为115200 8 N 1；*/
	
	ret_thread=pthread_create(&id,NULL,(void *) read_buff,NULL);/*创建一个线程，实现数据接收*/
	if(ret_thread!=0)/*创建线程出错报告*/
	{
		printf ("Create pthread error!n");
		exit (1);
	}
	send();				/*发送数据*/
	printf("\n");
	flag_close =close(fd);
	if(flag_close == -1)   /*判断是否成功关闭文件*/
	printf("Close the Device failur！\n");
	return 0;
}