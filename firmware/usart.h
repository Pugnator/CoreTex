#pragma once
extern char usart1_fifo[8];
extern char usart1_fifo_len;
void usart1_init(int baud);
void usart2_init(int baud);
void uart1_putc(int c);
void uart2_putc(int c);
int uart1_getc(void);
int uart2_getc(void);
void uprint_str( char *text);
