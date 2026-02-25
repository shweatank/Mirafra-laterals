#define USART1_SR  (*(int*)0x40013800)
#define USART1_DR  (*(int*)0x40013804)
#define USART1_CR1 (*(int*)0x4001380C)

void uart_write(char c) {
    while(!(USART1_SR &0x80)); 
    USART1_DR=(c& 0xFF); 
    while(!(USART1_SR& 0x40));
}