#include "stm32f3xx_hal.h"

void change_led();
void loop();
void swap(int *arrayIndex, int *arrayIndex2);

int counter = 0;
int leds[4]={8,10,12,14};


void EXTI0_IRQHandler() {
	// Check if interrupt flag is active - EXTI0
	if ((EXTI->PR & (1UL << 0)) == (1UL << 0)) {
		//GPIOE->ODR ^= 1UL << leds[counter]*1; // toggle LED
		swap(&leds[0],&leds[3]);
		swap(&leds[1],&leds[2]);
		GPIOE->ODR &= ~(0xFF <<8*1);

		counter = 0;

		EXTI->PR |= 1UL << 0; // Clear flag manually
	}
}

void EXTI3_IRQHandler(){
	if((EXTI->PR & (1UL << 3)) == (1UL << 3)){
			if(counter == 4){
				counter = 0;
			}
			GPIOE->ODR ^= 1UL << leds[counter]*1;
			counter++;
		EXTI->PR |= 1UL << 3; // Clear flag manually
	}
}

int main() {
	// Enable clock for system configuration controller
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	// Select trigger source. EXTI0 - PA0 (push button)
	// See page 249 of datasheet
	SYSCFG->EXTICR[0] &= ~(15UL << 0);
	// Set up EXTI0. See page 297 in datasheet
	EXTI->RTSR |= 1UL << 0; // Monitor rising edge
	//EXTI->FTSR |= 1UL << 0; // Monitor falling edge
	EXTI->IMR |= 1UL << 0; // Enable EXTI line 0
	// Priority 1 - lower value has higher priority
	// Negative priorities are reserved for System Errors
	NVIC_SetPriority(EXTI0_IRQn, 0);
	// Enable EXTI 0 interrupt
	NVIC_EnableIRQ(EXTI0_IRQn);


	// Enable clock for GPIOA and GPIOE
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
	// Set pin 0 to input - PA0 push button
	GPIOA->MODER &= ~(3UL << 0);
	GPIOA->PUPDR &= ~(3UL << 0);
	GPIOA->PUPDR |= 2UL << 0; // pull down
	// Set pins 8-15 to output - PE8-15 LEDs
	GPIOE->MODER &= ~(0xFFFF << 16);
	GPIOE->MODER |= 0x5555 << 16;
	GPIOE->OTYPER &= ~(0xFF << 8);
	GPIOE->OSPEEDR &= ~(0xFFFF << 16);
	GPIOE->PUPDR &= ~(0xFFFF << 16);
	// Initialize LEDs to 0
	GPIOE->ODR &= ~(0xFF << 8);


	// Goal: Activate TIM2_CH3 located in PA2
	// Details: see page 22 in manual
	// Step 1: Enable clock for GPIOA
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
	// Step 2: Configure PA2
	int pin_number = 2;
	// MODER: Set PA0 as alternate function
	// Details: see page 237 in datasheet
	GPIOA->MODER &= ~(3UL << pin_number*2);
	GPIOA->MODER |= 2UL << pin_number*2;
	// OTYPER: Set PA0 type as push-pull
	// Details: see page 237 in datasheet
	GPIOA->OTYPER &= ~(1UL << pin_number*1);
	// OSPEEDR: Set speed of PA0 to be low
	// Details: see page 238 in datasheet
	GPIOA->OSPEEDR &= ~(3UL << pin_number*2);
	// PUPDR: Set PA0 as no pull up/down

	// Details: see page 238 in datasheet
	GPIOA->PUPDR &= ~(3UL << pin_number*2);
	// AFR register is divided into 2 32-bit registers - AFRL, AFRH
	// AFRL - AFR[0] holds pin numbers 0-7, and
	 // AFRH - AFR[1] holds pin numbers 8-15
	// AFR[x] requires 4 bits per pin
	// Details: see pages 241-242 in datasheet
	// Note that for AFR[1] you will need to use ...(pin_number - 8)*4
	GPIOA->AFR[0] &= ~(15UL << pin_number*4);
	GPIOA->AFR[0] |= 1UL << pin_number*4; // Select AF1
	// Step 3: Enable clock for TIM2
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	// Step 4: Configure TIM2 - PA2 AF1
	// Goal: Design pulse with 1s period. Assume System Clock is 8MHz
	// CR1: Set up-counting (DIR) - bit 4. Disable TIM2 for setup (CEN)
	// Details: see page 647 in datasheet
	TIM2->CR1 &= ~(1UL << 4) & ~(1UL << 0);
	// CCMR1: Clear OC1M (bit 4-6 and 16). Set OC1M to toggle mode - 0b0011
	// Details: see page 656 in datasheet
	TIM2->CCMR2 &= ~(7UL << 4) & ~(1UL << 16);
	TIM2->CCMR2 |= 3UL << 4; // OC1M = 0b0011
	// CCER: Set polarity bit (CC1P) to be active high. Enable CH1 (CC1E)
	// Details: see page 562 in datasheet
	TIM2->CCER &= ~(1UL << 9);
	TIM2->CCER |= 1UL << 8;
	// PSC: Set prescaler as 1999. f_ck_cnt = 8MHz/2000 = 4KHz
	// Details: see page 664 in datasheet
	TIM2->PSC = 1999;
	// ARR: Set auto-reload value to 1999. ARR = f_ck_cnt/2 - 1 = 1999
	// Details: see page 664 in datasheet
	TIM2->ARR = 400;
	// CCR1: Set capture/compare value of CH1 to be CCR = (1+ARR)/2
	// Details: see page 665 in datasheet
	TIM2->CCR1 = 200;
	// Step 5: Enable TIM2 to activate configuration
	TIM2->CR1 |= 1UL << 0;



	//select trigger source EXTI3-PA2
	SYSCFG->EXTICR[0] &= ~(15UL << 12);
	//EXTI->RTSR |= 1UL << 3; // Monitor rising edge
	EXTI->FTSR |= 1UL << 3; // Monitor falling edge
	EXTI->IMR |= 1UL << 3; // Enable EXTI line 0
	//set priority
	NVIC_SetPriority(EXTI3_IRQn,1);
	NVIC_EnableIRQ(EXTI3_IRQn);



	// Dead loop
	while (1) {
	}
}
void change_led(){
	int leds[4] = {8,12,10,14};//1 = 9, 5 = 13,3 = 11, 7 = 15
	for(int i = 0;i<=4;i++){
		GPIOE->ODR |= 1UL << leds[i]*1;
		loop();
		GPIOE->ODR &= ~(1UL << leds[i]*1);
	}
}
void loop(){
	for(int i = 0;i<250000;i++);
}
void swap( int *arrayIndex, int *arrayIndex2){
	int temp = *arrayIndex;
	*arrayIndex = *arrayIndex2;
	*arrayIndex2 = temp;
}



