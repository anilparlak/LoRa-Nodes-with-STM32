/*
 * timer4.h
 *
 *  Created on: 19 Tem 2021
 *      Author: anilp
 */

#ifndef INC_TIMER4_H_
#define INC_TIMER4_H_
#include "main.h"

void timer4_init(void);

uint16_t timer4_setPeriod(uint16_t period);

void timer4_enable(void);

void timer4_disable(void);

#endif /* INC_TIMER4_H_ */
