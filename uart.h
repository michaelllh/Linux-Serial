#ifndef _UART_H_ 
#define _UART_H_ 

int fd;
int open_serial(int uart_port);
void intUart(int fd);
void read_buff();
void send();

#endif