/*
 * timer.3.c
 *
 *  Created on: Jun 4, 2021
 *      Author: anilp
 */
#include "timer3.h"

void timer3_init(void)
{
	//Enable TIM2 clock
	__HAL_RCC_TIM3_CLK_ENABLE();

	//Timer clock = 36MHz / 36000= (1ms period)
	TIM3->PSC = 35999;

	// Reload in every 30sec
	TIM3->ARR = 29999;

	// Enable Timer Update Interrupt
	TIM3->DIER |= TIM_DIER_UIE;

	NVIC_EnableIRQ(TIM3_IRQn);
	NVIC_SetPriority(TIM3_IRQn,0);// 2.öncelikli

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
