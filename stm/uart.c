#include "stm32f10x.h"                  // Device header
#include "uart.h"
void uart_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure; 

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 9600;  
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1; 
    USART_InitStructure.USART_Parity = USART_Parity_No;   
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);
   
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);                
    NVIC_InitStructure.NVIC_IRQChannel =  USART1_IRQn;             
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;      
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;             
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    USART_Cmd(USART1, ENABLE);
}

void uart_write(char ch)
{
    USART_SendData(USART1, ch);
    while (RESET == USART_GetFlagStatus(USART1, USART_FLAG_TXE));
}

void uart_wstring(char* p)
{
    //while(*p)   
    while(*p != '\0')     
    {
        uart_write(*p);
        p++;
    }
}

char uart_read(void)
{

    char ch; 
    while(RESET == USART_GetFlagStatus(USART1, USART_FLAG_RXNE));  
    ch = USART_ReceiveData(USART1);
    return ch; 
}

void uart_rstring(char *p)
{
    char ch;
    while('\n' != (ch = uart_read()))
    {
        *p++ = ch;
    }
    *p = '\n';  
}

void USART1_IRQHandler(void)
{
    char buf[100];  
    if(USART_GetITStatus(USART1, USART_IT_RXNE))
    {
        uart_rstring(buf);
        uart_wstring(buf);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}