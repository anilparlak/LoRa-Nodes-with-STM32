/*
 * timer4.c
 *
 *  Created on: 4 Jun 2021
 *      Author: anilp
 */


#include "timer4.h"

void timer4_init(void)
{
	//Enable TIM4 clock
	__HAL_RCC_TIM4_CLK_ENABLE();

	//Timer clock = 64MHz / 64000= 1000Hz (1ms period)
	TIM4->PSC = 63999; // 16 bit PSC = 65335

	// Reload in every 13sec
	TIM4->ARR = 12999;

	// Enable Timer Update Interrupt
	TIM4->DIER |= TIM_DIER_UIE;

	NVIC_EnableIRQ(TIM4_IRQn);
	NVIC_SetPriority(TIM4_IRQn,1);


}

uint16_t timer4_setPeriod(uint16_t period)
{
	uint16_t cnt = TIM4->CNT;

	return cnt;
}

void timer4_enable(void)
{
	TIM4->CR1 |= TIM_CR1_CEN;
}

void timer4_disable(void)
{
	TIM4->CR1 &= ~(TIM_CR1_CEN);
}
