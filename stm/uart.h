#ifndef _UART_H
#define _UART_H

void uart_init(void);

void uart_write(char ch);

char uart_read(void);

#endif