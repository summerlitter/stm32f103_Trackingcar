#ifndef _BLUETOOTH_H
#define _BLUETOOTH_H

#include <stdint.h>

#define RX_BUFFER_SIZE 256
#define MAX_CMD_LEN 128

typedef struct {
    char buffer[RX_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
} RingBuffer;

void uart2_init(void);
void uart2_write(char ch);
void uart2_wstring(const char* p);
int  uart2_readline(char *buf, int max_len);
void Buffer_Init(void);

#endif