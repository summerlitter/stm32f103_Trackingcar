#include "stm32f10x.h"
#include "bluetooth.h"

static RingBuffer rx_buffer = {{0}, 0, 0};

void Buffer_Init(void) {
    rx_buffer.head = 0;
    rx_buffer.tail = 0;
}

void uart2_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // TX (PA2) 配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // RX (PA3) 配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);
    Buffer_Init();
}

void uart2_write(char ch)
{
    USART_SendData(USART2, (uint8_t)ch);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

void uart2_wstring(const char* p)
{
    while(*p != '\0') {
        uart2_write(*p++);
    }
}

void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
    {
        char ch = USART_ReceiveData(USART2);
        uint16_t next = (rx_buffer.head + 1) % RX_BUFFER_SIZE;
        
        if(next != rx_buffer.tail) {
            rx_buffer.buffer[rx_buffer.head] = ch;
            rx_buffer.head = next;
        }
        
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

int uart2_readline(char *buf, int max_len)
{
    int len = 0;
    uint8_t line_end = 0;
    
    while(rx_buffer.tail != rx_buffer.head && len < max_len-1)
    {
        char ch = rx_buffer.buffer[rx_buffer.tail];
        rx_buffer.tail = (rx_buffer.tail + 1) % RX_BUFFER_SIZE;
        
        if(ch == '\r' || ch == '\n') {
            line_end = 1;
            break;
        } else {
            buf[len++] = ch;
        }
    }
    
    if(line_end) {
        buf[len] = '\0';
        while(rx_buffer.tail != rx_buffer.head && 
             (rx_buffer.buffer[rx_buffer.tail] == '\r' || 
              rx_buffer.buffer[rx_buffer.tail] == '\n')) 
        {
            rx_buffer.tail = (rx_buffer.tail + 1) % RX_BUFFER_SIZE;
        }
        return len;
    }
    
    return 0;
}