/*
 * timer.c
 *
 *  Created on: Jun 4, 2021
 *      Author: anilp
 */

#include "timer.h"

void timer2_init(void)
{

	//Enable TIM2 clock
	__HAL_RCC_TIM2_CLK_ENABLE();

	//Timer clock = 36MHz / 36000= 1000Hz (1ms period)
	TIM2->PSC = 35999;

	// Reload in every 500ms
	TIM2->ARR = 499;

	// Enable Timer Update Interrupt
	TIM2->DIER |= TIM_DIER_UIE;

	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_SetPriority(TIM2_IRQn,3);


}

uint16_t timer2_setPeriod(uint16_t period)
{
	uint16_t cnt = TIM2->CNT;

	return cnt;
}

void timer2_enable(void)
{
	TIM2->CR1 |= TIM_CR1_CEN;
}

void timer2_disable(void)
{
	TIM2->CR1 &= ~(TIM_CR1_CEN);
}


