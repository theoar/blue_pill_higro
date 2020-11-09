#ifndef RCC_DEFS_H_
#define RCC_DEFS_H_


namespace rcc
{

enum class BusTimerAPB
{
	APB1,
	APB2,
};

enum class PeripheralClock
{
	GPIOA_PER,
	GPIOB_PER,
	GPIOC_PER,

	TIM1_PER,
	TIM2_PER,
	TIM3_PER,
	TIM4_PER,

	ADC1_PER,
	ADC2_PER,

	RTC_PER,

	USART1_PER,
	USART2_PER,
	USART3_PER,

	I2C1_PER,
	I2C2_PER,

	SPI1_PER,
	SPI2_PER
};


enum class PeripheralReset
{
	GPIOA_PER,
	GPIOB_PER,
	GPIOC_PER,

	TIM1_PER,
	TIM2_PER,
	TIM3_PER,
	TIM4_PER,

	ADC1_PER,
	ADC2_PER,

	RTC_PER,

	USART1_PER,
	USART2_PER,
	USART3_PER,

	I2C1_PER,
	I2C2_PER,

	SPI1_PER,
	SPI2_PER
};

enum class BusType
{
	APB1,
	APB2,
	AHB,
	BDCR
};


}

#endif

