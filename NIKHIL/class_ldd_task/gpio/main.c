#define GPIOA_BASE    0x40020000
#define GPIOA_BASE    0x40020000
#define GPIOA_MODER   (*(int *)(GPIOA_BASE + 0x00)) 
#define GPIOA_ODR     (*(int *)(GPIOA_BASE + 0x14)) 



void delay(int count) {
    for(int i=count;i>=0;i--);
}


int main(void) {
    GPIOA_MODER|=(1<<(5*2));
    while(1){
        GPIOA_ODR^=(1<<5);
        delay(1000000);
    }
}