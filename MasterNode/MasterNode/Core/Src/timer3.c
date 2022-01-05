/*
 * timer3.c
 *
 *  Created on: Jun 4, 2021
 *      Author: anilp
 */
#include "timer3.h"

void timer3_init(void)
{
	//Enable TIM2 clock
	__HAL_RCC_TIM3_CLK_ENABLE();

	//Timer clock = 64MHz / 64000= 1000Hz (1ms period)
	TIM3->PSC = 63999; // 16 bit PSC = 65335

	// Reload in every 11sec
	TIM3->ARR = 10999;

	// Enable Timer Update Interrupt
	TIM3->DIER |= TIM_DIER_UIE;

	NVIC_EnableIRQ(TIM3_IRQn);
	NVIC_SetPriority(TIM3_IRQn,0);


}

uint16_t timer3_setPeriod(uint16_t period)
{
	uint16_t cnt = TIM3->CNT;

	return cnt;
}

void timer3_enable(void)
{
	TIM3->CR1 |= TIM_CR1_CEN;
}

void timer3_disable(void)
{
	TIM3->CR1 &= ~(TIM_CR1_CEN);
}


