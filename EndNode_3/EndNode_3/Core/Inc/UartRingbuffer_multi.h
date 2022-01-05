/*
 *
 *
 *  Created on: Nov 14, 2020
 *      Author: anilp
 */

#ifndef UARTRINGBUFFER_H_
#define UARTRINGBUFFER_H_


#include "stm32f1xx_hal.h"


#define UART_BUFFER_SIZE 512
//#define device_uart &huart2
#define pc_uart &huart1
#define lora_uart &huart3

typedef struct
{
  unsigned char buffer[UART_BUFFER_SIZE];
  volatile unsigned int head;
  volatile unsigned int tail;
} ring_buffer;


/* reads the data in the rx_buffer and increment the tail count in rx_buffer of the given UART */
int Uart_read(UART_HandleTypeDef *uart);

/* writes the data to the tx_buffer and increment the head count in tx_buffer */
void Uart_write(int c, UART_HandleTypeDef *uart);

/* function to send the string to the uart */
void Uart_sendstring(const char *s, UART_HandleTypeDef *uart);

/* Initialize the ring buffer */
void Ringbuf_init(void);

/* Resets the entire ring buffer, the new data will start from position 0 */
void Uart_flush (UART_HandleTypeDef *uart);

/* checks if the data is available to read in the rx_buffer of the uart */
int IsDataAvailable(UART_HandleTypeDef *uart);

/* Look for a particular string in the given buffer
 * @return 1, if the string is found and -1 if not found
 * @USAGE:: if (Look_for ("some string", buffer)) do something
 */
int Look_for (char *str, char *buffertolookinto);


/* Peek for the data in the Rx Bffer without incrementing the tail count
* Returns the character
* USAGE: if (Uart_peek () == 'M') do something
*/
int Uart_peek(UART_HandleTypeDef *uart);

/* Copies the entered number of characters (blocking mode) from the Rx buffer into the buffer, after some particular string is detected
* Returns 1 on success and -1 otherwise
* USAGE: while (!(Get_after ("some string", 6, buffer, uart)));
*/
int Get_after (char *string, uint8_t numberofchars, char *buffertosave, UART_HandleTypeDef *uart);

/* Wait until a paricular string is detected in the Rx Buffer
* Return 1 on success and -1 otherwise
* USAGE: while (!(Wait_for("some string", uart)));
*/
int Wait_for (char *string, UART_HandleTypeDef *uart);

/* the ISR for the uart. put it in the IRQ handler */
void Uart_isr (UART_HandleTypeDef *huart);


#endif /* UARTRINGBUFFER_H_ */
