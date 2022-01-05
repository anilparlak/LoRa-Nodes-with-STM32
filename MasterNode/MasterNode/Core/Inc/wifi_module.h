/*
 * wifi_module.h
 *
 *  Created on: Nov 14, 2020
 *      Author: anilp
 */


#ifndef INC_WIFI_MODULE_H_
#define INC_WIFI_MODULE_H_

#include "UartRingbuffer_multi.h"
#include "stdio.h"
#include "string.h"
//#include "delay.h"
//#include "led.h"

//#define wifi_uart &huart1
//#define pc_uart &huart2

void wifi_Init (char *SSID, char *PASSWD);
void Get_Node1(void);
void Get_Node2(void);
void button_node1(void);
void button_node2(void);

#endif /* INC_WIFI_MODULE_H_ */
