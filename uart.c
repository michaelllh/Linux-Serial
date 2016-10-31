
/*******************************************************
*ilename：uart.c
* Description：Receive and send data
* Date：
*******************************************************/

#include "uart.h" 
#include <sys/types.h>   
#include <sys/stat.h>  
#include <fcntl.h>  
#include <termios.h>  
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <unistd.h>  

#define max_buffer_size   100   /*定义缓冲区最大宽度*/ 

char key_val[2] = {0};				/*用于保存按键触发键值*/
char  hd[max_buffer_size],*rbuf; 	/*定义接收缓冲区*/

/*************结构体用于与键值对应****************/
typedef struct
{
  char url[10];
  char key;
}Pair;

Pair KEY[] ={
	{"HOME\n", 0x30},
	{"END\n", 0X31},
	{"F1\n", 0X32},
	{"F2\n", 0x33},
	{"F3\n", 0X34},
	{"F4\n", 0X35},
	{"F5\n", 0x36},
	{"UP\n", 0X37},
	{"DOWN\n", 0X38},
	{"LEFT\n", 0X39},
	{"RIGHT\n", 0X3a}
};
/*******************************************************/


/** 
*@brief  打开串口
*@param  uart_port     类型 int  要打开的串口 
*@return  Int 
*/  


int open_serial(int uart_port)
{
	if(uart_port == 0)       /*串口选择*/
	{
	 fd = open("/dev/ttyS0",O_RDWR|O_NOCTTY);  /*读写方式打开串口*/
	 perror("open /dev/ttyS0");
	}
	else
	{
	 fd = open("/dev/ttyS1",O_RDWR|O_NOCTTY);
	 perror("open /dev/ttyS1");
	}
	if(fd == -1)  /*打开失败*/
	return -1;
	else
	return 0;
}


////////////////////////////////////////////////////////////////////////////////  
/** 
*@brief  初始化串口 
*@param  fd     打开文件返回
*@return  void 
*/  
 
void intUart(int fd){ 

	struct termios opt;
	
	tcgetattr(fd,&opt);
	cfmakeraw(&opt);
	cfsetispeed(&opt,B115200); /*波特率设置为115200bps*/
	cfsetospeed(&opt,B115200);
	
	opt.c_cflag |= CS8;//设置数据位8
	opt.c_cflag &= ~PARENB;   /* Clear parity enable */  //校验
    opt.c_iflag &= ~INPCK;     /* Enable parity checking */ 
	opt.c_cflag &= ~CSTOPB;//停止位为1
	
	tcsetattr(fd,TCSANOW,&opt);
	
}  


/** 
*@brief  打开串口
*@param  data     类型 char  	解析的数据包
*@param  size     类型 int  	数据包有效位数长度
*@return  Int 					数据包有效 1   数据包无效-1
*/ 

int dataAna(char data[],int size)
{	
	int i = 0;
	char sum = 0;
	if(size < 4)return -1;//接收到空包或者无用包
	if(data[1] == 0x33&&(data[0]&0x000000ff) == 0xff)
	{	
		
		if(size == data[2] + 2)
		{	
			for(i = 2; i < size - 1;i++)
			{
				sum += data[i];
			}
			if((data[size-1]&0x000000ff) == 0x100 -sum)//数据包正常
			{			
				return 1;
			}else
			{
				
				printf("%x:check eorr\n",0x100 -sum);
				return -1;
			}
		}else
		{
			printf("size is : %d\n",size);
			printf("size eorr\n");
			return -1;
		}
			
	}else
	{
		//printf("0:%x,1:%x",data[0],data[1]);
		printf("head eorr\n");
		return -1;
	}
}

/** 
*@brief 读取数据
*@param  null
*@return  void 
*/ 
void read_buff()
{	
    int ncount = 0,write_re,i = 0;
	rbuf=hd; 			/*数据保存*/
	printf("ready for receiving data...\n");/*用户界面提示*/
	
	while (1) //循环读取数据
	{   
		while((ncount = read(fd, hd, 99))>0)
		{ 
			
			/*************显示解析完数据包代码******************/
			if( dataAna(hd,ncount) == 1)//如果数据包正常
			{
				for(i = 3;i < ncount - 1;i++)
				{
					if((hd[i] -0x30) < 11)
					{
						/*********选择回显数据部分************/
						//write_re = write(fd,KEY[hd[i] -0x30].url,strlen(KEY[hd[i] -0x30].url)); //发送数据 
						//if(write_re == -1)
						//{
						// perror("write");
						//}
						/*************************************/
						
						printf("%s",KEY[hd[i] -0x30].url); //打印数据
					}
				}
			}else
			{
				
				printf("receive a eorr package !! \n");
			}
			/*******************************************************/
			
			/*********直接显示接收到数据包，不做解析部分代码*********/
			/*
			hd[ncount] = '\n';  
			hd[ncount + 1] = '\0'; 
			printf( "%s", hd); 
			write_re = write(fd,hd,ncount + 1); 
			if(write_re == -1)
			{
			 perror("write");
			}
			*/
			/*******************************************************/
			
			memset(hd,0,sizeof(hd));//清空数组
		
		}
	}
}

/** 
*@brief 功能函数
*@param  NULL
*@return  int
*/ 

int kbhit(void)  
{  
	struct termios oldt, newt;  
	int ch;  
	int oldf;  
	tcgetattr(STDIN_FILENO, &oldt);  
	newt = oldt;  
	newt.c_lflag &= ~(ICANON | ECHO);  
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);  
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);  
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);  
	ch = getchar();  
	
	key_val[0] = ch;
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  
	fcntl(STDIN_FILENO, F_SETFL, oldf);  
	if(ch != EOF)  
	{  
		ungetc(ch, stdin);  
		return 1;  
	}  
	return 0;  
	} 

	
/** 
*@brief 发送数据
*@param  NULL
*@return  VOID
*/ 
	
void send()
{
	int quit_flag = 1,write_re = 0;
	char get_char = 0,key_num = 0;
	while(quit_flag)
	{
		while(!kbhit()) 
		{
			//puts("Press a key!"); 
		} 
		getchar();//做一次读取处理，但不获取数据
		key_val[1] = '\n';
		
		//进行键值对应
		switch(key_val[0])
		{
			case 0x31:
				key_num = 1;
				break;
			case 0x34:
				key_num = 2;
				break;
			case 0x50:
				key_num = 3;
				break;
			case 0x51:
				key_num = 4;
				break;
			case 0x52:
				key_num = 5;
				break;
			case 0x53:
				key_num = 6;
				break;
			case 0x35:
				key_num = 7;
				break;					
			case 0x41:
				key_num = 8;
				break;
			case 0x42:
				key_num = 9;
				break;
			case 0x43:
				key_num = 11;
				break;
			case 0x44:
				key_num = 10;
				break;
			default:
				key_num = 0;
				break;
		}
		//接收到数据是否为有效KEY
		if(key_num > 0)
		{
			write_re = write(fd,KEY[key_num -1].url,strlen(KEY[key_num - 1].url));  
			printf("%s",KEY[key_num -1].url); 
			if(write_re == -1)
			{
				perror("write");
			}
		}
	}	
}

