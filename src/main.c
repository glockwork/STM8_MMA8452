#include <iostm8s003f3.h>
#include <stdio.h>
#include "MMA8452.h"
void configUART();
void config();
void configEXTI();
void configI2C(int khz,int longerNs);
extern char BUF[3];
//=========================================
// 功能：配置定时器1,只有HSI作为时钟源才有效
// 输入：unsigned int ms -> 定时的毫秒数
//      unsigned char interruptState -> 是否中断
// 输出：无
// 返回：无*/
//=========================================
void configTIM1(unsigned int ms,unsigned char interruptState){
	/* 查询单片机的时钟源是否是内部HSI */
	if(CLK_CMSR == 0XE1){
		/* 查询HSI的分频系数 */
		unsigned char temp = CLK_CKDIVR&0X18;
		temp >>=3 ;

		/* 设置定时器1的1KH的工作频率的预分频系数 */ 
		char t = 1;
		for(;temp>0;temp--,t*=2); 
		unsigned int time = 16000/t;
		TIM1_PSCRH = time/256;
		TIM1_PSCRL = time%256;


		/* 设置单片机中断频率 */
		TIM1_ARRH  = ms/256;
		TIM1_ARRL = ms%256;

		
		/* 开启定时器1 */
		TIM1_CR1_CEN = 1;
		
		/* 如果需要设置中断 */
		if(interruptState){
			/* 使能定时器1的计算溢出中断位 */
			TIM1_IER_UIE = 1;
            TIM1_SR1_UIF = 1;
			/* 开启总中断源 */
    			asm("rim");
		}	
	}
}
void LEDInit(){
    PC_DDR_DDR4 = 1;
    PC_CR1_C14 = 1;
    PC_CR2_C24 = 1;
    PC_ODR_ODR4 = 1;
}
void LEDCtrl(unsigned char temp){
    static unsigned char i = 1;
    if(temp<3)
        PC_DDR_DDR4 = temp;
    else{
        PC_ODR_ODR4 = ~PC_ODR_ODR4;
    }
}
void main(){
    CLK_CKDIVR = 0X00;
    LEDInit();
    configEXTI();
    configUART();
    printf("***************************start************************************\n");
    configI2C(100,1000);
    Init_MMA8452();
    configTIM1(1000,1);
    while(1){
        void dome();
        dome();
        //printf("%d \n\r\n",*(&i1[2]-1));
      
      
        //void test();
        //test();
    }    
}
void configEXTI(){
    PD_DDR_DDR3 = 0;
    PD_CR1_C13 = 1;
    PD_ODR_ODR3 = 0;
    
    PD_CR2_C23 = 1;
    EXTI_CR1=0x80;
    
    asm("rim");
}
void configUART(){
    UART1_CR1 = 0;
    UART1_CR3 = 0;
    UART1_BRR1 = 0X11;
    UART1_BRR2 = 0X06;
    UART1_CR2_TEN = 1;
    UART1_CR2_REN = 1;
}
int putchar(int c)  
{  
  while (!UART1_SR_TC); 
  UART1_DR = (unsigned char )c;
  return (c);  
}
void configI2C(int khz,int longerNs){
	/* 配置硬件I2C的外设时钟频率 */
	I2C_FREQR &= ~0x3F;
	I2C_FREQR |= 0x0a;
	
	/* 配置时钟控制寄存器 */
	int temp = 10000/khz;//单位100ns
			
	temp /= 2;
	
	//temp /= 1;

	I2C_CCRH = temp/256;
	I2C_CCRL = temp%256; 
    
	/* 配置最大上升时间 */
	I2C_TRISER = longerNs/100+1;

	/* 应答使能 */
	I2C_CR2_ACK = 1;

	/* 启用I2C */
	I2C_CR1_PE = 1;
}
