#ifndef UART1_H_
#define UART1_H_
#include <stdio.h>

void uart0Init(void);
int uart0Transmit(char data, FILE *stream);
int uart0Receive(FILE *stream);
void uart0PrintString(char *str);
void uart0Print1ByteNumber(unsigned char n);

#endif // UART1_h_