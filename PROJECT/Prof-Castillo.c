#include "stm32f3xx_hal.h"

void GPIO_USART_Init(GPIO_TypeDef*, int, int);
void USART_Init(USART_TypeDef*, GPIO_TypeDef*, int);
void LED_Init(GPIO_TypeDef*, int);
void Toggle_LED(GPIO_TypeDef*, int);

int main(void) {
	// Enable GPIO clock sources
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	RCC->AHBENR |= RCC_AHBENR_GPIODEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
	// Initialize GPIO pins for LED
	int led_usart3 = 15;
	int led_usart2 = 11;
	GPIO_TypeDef* GPIO_LED = GPIOE;
	LED_Init(GPIO_LED, led_usart3);
	LED_Init(GPIO_LED, led_usart2);
	// Initialize GPIO pins for USART
	GPIO_USART_Init(GPIOB, 10, 11); // PB10 - USART3_TX, PB11 - USART3_RX
	GPIO_USART_Init(GPIOD, 5, 6); // PD5 - USART2_TX, PD6 - USART2_RX
	// Enable USART clock sources
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	// Configure USART3 (location 18) and USART2 (location 16) to SYSCLK - 01
	RCC->CFGR3 &= ~(3UL << 18 | 3UL << 16);
	RCC->CFGR3 |= 1UL << 18 | 1UL << 16;
	// Initialize USART peripherals
	USART_Init(USART3, GPIO_LED, led_usart3);
	USART_Init(USART2, GPIO_LED, led_usart2);

	while(1) {

	}
}

void Toggle_LED(GPIO_TypeDef* GPIOx, int pin_number) {
	GPIOx->ODR ^= 1UL << pin_number;
}

void LED_Init(GPIO_TypeDef *GPIOx, int pin_number) {
	GPIOx->MODER &= ~(3UL << pin_number*2);
	GPIOx->MODER |= 1UL << pin_number*2; // Digital output
	GPIOx->OSPEEDR &= ~(3UL << pin_number*2); // Low speed
	GPIOx->PUPDR &= ~(3UL << pin_number*2); // No pull up/down
	GPIOx->OTYPER &= ~(1UL << pin_number); // Push-pull
}

void GPIO_USART_Init(GPIO_TypeDef *GPIOx, int tx_pin, int rx_pin) {
	GPIOx->MODER &= ~(3UL << tx_pin*2 | 3UL << rx_pin*2);
	GPIOx->MODER |= 2UL << tx_pin*2 | 2UL << rx_pin*2; // Alternate mode
	// Select appropriate Alternate Function
	int index = 0;
	if (tx_pin >= 8) {
		index = 1;
	}
	GPIOx->AFR[index] |= 3UL << (tx_pin % 8)*4;
	if (rx_pin < 8) {
		index = 0;
	}
	GPIOx->AFR[index] |= 3UL << (rx_pin % 8)*4;
	GPIOx->OSPEEDR |= 3UL << tx_pin*2 | 3UL << rx_pin*2; // High speed
	GPIOx->PUPDR &= ~(3UL << tx_pin*2 | 3UL << rx_pin*2);
	GPIOx->PUPDR |= 1UL << tx_pin*2 | 1UL << rx_pin*2; // Pull up configuration
	GPIOx->OTYPER &= ~(1UL << tx_pin | 1UL << rx_pin); // Push-pull configuration
}

void USART_Init(USART_TypeDef* USARTx, GPIO_TypeDef* status_port, int status_pin) {
	USARTx->CR1 &= ~(1UL << 0); // Disable USART
	USARTx->CR1 &= ~(1UL << 12 | 1UL << 28); // 1 Start + 8 Data + n Stop
	USARTx->CR1 &= ~(3UL << 12); // n = 1
	USARTx->CR1 &= ~(1UL << 9); // Even parity
	USARTx->CR1 |= 1UL << 15; // Oversampling by 8
	USARTx->BRR = 0x681; // Baud rate = 9600 when fck = 8MHz
	USARTx->CR1 |= 1UL << 2 | 1UL << 3; // Enable transmission + reception
	USARTx->CR1 |= 1UL << 0; // Enable USARTx peripheral
	while((USARTx->ISR & 1UL << 21) == 0); // Verify USARTx is ready to transmit
	while((USARTx->ISR & 1UL << 22) == 0); // Verify USARTx is ready to receive
	Toggle_LED(status_port, status_pin); // Use LED to show user that USARTx is initialized
}
