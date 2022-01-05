/*
 * timer2.h
 *
 *  Created on: Jun 4, 2021
 *      Author: anilp
 */

#ifndef INC_TIMER2_H_
#define INC_TIMER2_H_

#include "main.h"

void timer2_init(void);

uint16_t timer2_setPeriod(uint16_t period);

void timer2_enable(void);

void timer2_disable(void);

#endif /* INC_TIMER2_H_ */
