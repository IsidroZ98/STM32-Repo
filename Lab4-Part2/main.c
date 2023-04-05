#include "main.h"
#include "stm32f3xx_hal.h"


void GPIO_Pin_Init(int);
int main(){
    // PIN A4 init - DAC_OUT1
    int pin_number = 4;
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER &= ~(3UL << pin_number*2);
    GPIOA->MODER |= 3UL << pin_number*2; // Analog mode
    // DAC module init
    RCC->APB1ENR |= RCC_APB1ENR_DAC1EN;
    DAC->CR &= ~(1 << 0); // EN1 - disable channel 1
    DAC->CR |= 1 << 2; // TEN1 - enable trigger for channel 1
    DAC->CR |= 7 << 3; // TSEL1[2:0] - set software trigger (0b111)
    DAC->CR |= 1 << 0; // BOFF1 - buffer enabled
    DAC->CR |= 1 << 0; // EN1 - enable channel 1
    // Iterate over every possible value
    int output = 0;

    while (1)
        {
    	//up
        for(int i =0; i<=4095; i+=127){
		// 12-bit resolution
		DAC->DHR12R1 = output;
		DAC->SWTRIGR |= 1 << 0; // SWTRIG1 - activate DAC with trigger
		output = (output+127) & 0xFFF; // Make sure that output is 8 bits
        }
        loop();
        output = 4095;

        for(int i =0; i<=4095; i+=127){
        		// 12-bit resolution
        DAC->DHR12R1 = output;
        DAC->SWTRIGR |= 1 << 0; // SWTRIG1 - activate DAC with trigger
        output = (output-127) & 0xFFF; // Make sure that output is 8 bits
         }
        output = 0;
        loop();


        }
    }

void GPIO_Pin_Init(int pin){
    GPIOA->MODER &= ~(3UL << pin*2);
    GPIOA->MODER |= 1UL << pin*2;

    GPIOA->OTYPER &= ~(1UL << pin*1);

    GPIOA->OSPEEDR &= ~(3UL << pin*2);

    GPIOA->OSPEEDR |= 1UL << pin*2;

    GPIOA->PUPDR &= ~(3UL << pin*2);
}

void loop() {
    for(int i=0; i <430; i++){}
}