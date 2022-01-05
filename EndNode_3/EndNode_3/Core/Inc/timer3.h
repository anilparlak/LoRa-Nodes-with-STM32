/*
 * timer3.h
 *
 *  Created on: Jun 4, 2021
 *      Author: anilp
 */

#ifndef INC_TIMER3_H_
#define INC_TIMER3_H_
#include "main.h"

void timer3_init(void);

uint16_t timer3_setPeriod(uint16_t period);

void timer3_enable(void);

void timer3_disable(void);

#endif /* INC_TIMER3_H_ */
