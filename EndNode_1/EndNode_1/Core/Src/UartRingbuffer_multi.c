/*
 *
 *
 *  Created on: Nov 14, 2020
 *      Author: anilp
 */

#include "UartRingbuffer_multi.h"
#include <string.h>


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;



ring_buffer rx_buffer1 = { { 0 }, 0, 0};
ring_buffer tx_buffer1 = { { 0 }, 0, 0};
ring_buffer rx_buffer2 = { { 0 }, 0, 0};
ring_buffer tx_buffer2 = { { 0 }, 0, 0};
ring_buffer rx_buffer3 = { { 0 }, 0, 0};
ring_buffer tx_buffer3 = { { 0 }, 0, 0};

ring_buffer *_rx_buffer1;
ring_buffer *_tx_buffer1;
ring_buffer *_rx_buffer2;
ring_buffer *_tx_buffer2;
ring_buffer *_rx_buffer3;
ring_buffer *_tx_buffer3;

void store_char (unsigned char c, ring_buffer *buffer);


void Ringbuf_init(void)
{
  _rx_buffer1 = &rx_buffer1;
  _tx_buffer1 = &tx_buffer1;
  _rx_buffer2 = &rx_buffer2;
  _tx_buffer2 = &tx_buffer2;
  _rx_buffer3 = &rx_buffer3;
  _tx_buffer3 = &tx_buffer3;

  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
 // __HAL_UART_ENABLE_IT(device_uart, UART_IT_ERR);
  __HAL_UART_ENABLE_IT(pc_uart, UART_IT_ERR);
  __HAL_UART_ENABLE_IT(lora_uart, UART_IT_ERR);

  /* Enable the UART Data Register not empty Interrupt */
  //__HAL_UART_ENABLE_IT(device_uart, UART_IT_RXNE);
  __HAL_UART_ENABLE_IT(pc_uart, UART_IT_RXNE);
  __HAL_UART_ENABLE_IT(lora_uart, UART_IT_RXNE);
}

void store_char(unsigned char c, ring_buffer *buffer)
{
  int i = (unsigned int)(buffer->head + 1) % UART_BUFFER_SIZE;

  // if we should be storing the received character into the location
  // just before the tail (meaning that the head would advance to the
  // current location of the tail), we're about to overflow the buffer
  // and so we don't write the character or advance the head.
  if(i != buffer->tail) {
    buffer->buffer[buffer->head] = c;
    buffer->head = i;
  }
}


int Look_for (char *str, char *buffertolookinto)
{
	int stringlength = strlen (str);
	int bufferlength = strlen (buffertolookinto);
	int so_far = 0;
	int indx = 0;
repeat:
	while (str[so_far] != buffertolookinto[indx]) indx++;
	if (str[so_far] == buffertolookinto[indx]){
	while (str[so_far] == buffertolookinto[indx])
	{
		so_far++;
		indx++;
	}
	}

	else
		{
			so_far =0;
			if (indx >= bufferlength) return -1;
			goto repeat;
		}

	if (so_far == stringlength) return 1;
	else return -1;
}


void Uart_flush (UART_HandleTypeDef *uart)
{

	if (uart == pc_uart)
	{
		memset(_rx_buffer2->buffer,'\0', UART_BUFFER_SIZE);
		_rx_buffer2->head = 0;
	}
	if (uart == lora_uart)
	{
		memset(_rx_buffer3->buffer,'\0', UART_BUFFER_SIZE);
		_rx_buffer3->head = 0;
	}

}


int Uart_peek(UART_HandleTypeDef *uart)
{

	if (uart == pc_uart)
	{
		  if(_rx_buffer2->head == _rx_buffer2->tail)
		  {
		    return -1;
		  }
		  else
		  {
		    return _rx_buffer2->buffer[_rx_buffer2->tail];
		  }
	}

	else if (uart == lora_uart)
	{
		  if(_rx_buffer3->head == _rx_buffer3->tail)
		  {
		    return -1;
		  }
		  else
		  {
		    return _rx_buffer3->buffer[_rx_buffer3->tail];
		  }
	}

	return -1;
}

int Uart_read(UART_HandleTypeDef *uart)
{
	if (uart == pc_uart)
	{
		  // if the head isn't ahead of the tail, we don't have any characters
		  if(_rx_buffer2->head == _rx_buffer2->tail)
		  {
		    return -1;
		  }
		  else
		  {
		    unsigned char c = _rx_buffer2->buffer[_rx_buffer2->tail];
		    _rx_buffer2->tail = (unsigned int)(_rx_buffer2->tail + 1) % UART_BUFFER_SIZE;
		    return c;
		  }
	}

	else if (uart == lora_uart)
	{
		  // if the head isn't ahead of the tail, we don't have any characters
		  if(_rx_buffer3->head == _rx_buffer3->tail)
		  {
		    return -1;
		  }
		  else
		  {
		    unsigned char c = _rx_buffer3->buffer[_rx_buffer3->tail];
		    _rx_buffer3->tail = (unsigned int)(_rx_buffer3->tail + 1) % UART_BUFFER_SIZE;
		    return c;
		  }
	}


	else return -1;
}

void Uart_write(int c, UART_HandleTypeDef *uart)//parameter errror
{
	if (c>=0)
	{
		if (uart == pc_uart){
			int i = (_tx_buffer2->head + 1) % UART_BUFFER_SIZE;

			// If the output buffer is full, there's nothing for it other than to
			// wait for the interrupt handler to empty it a bit
			// ???: return 0 here instead?
			while (i == _tx_buffer2->tail);

			_tx_buffer2->buffer[_tx_buffer2->head] = (uint8_t)c;
			_tx_buffer2->head = i;

			__HAL_UART_ENABLE_IT(pc_uart, UART_IT_TXE); // Enable UART transmission interrupt
			}

		else if (uart == lora_uart){
			int i = (_tx_buffer3->head + 1) % UART_BUFFER_SIZE;


			while (i == _tx_buffer3->tail);

			_tx_buffer3->buffer[_tx_buffer3->head] = (uint8_t)c;
			_tx_buffer3->head = i;

			__HAL_UART_ENABLE_IT(lora_uart, UART_IT_TXE); // Enable UART transmission interrupt
			}

	}
}

int IsDataAvailable(UART_HandleTypeDef *uart)
{

	if (uart == pc_uart) return (uint16_t)(UART_BUFFER_SIZE + _rx_buffer2->head - _rx_buffer2->tail) % UART_BUFFER_SIZE;
	else if (uart == lora_uart) return (uint16_t)(UART_BUFFER_SIZE + _rx_buffer3->head - _rx_buffer3->tail) % UART_BUFFER_SIZE;
	return -1;
}



int Get_after (char *string, uint8_t numberofchars, char *buffertosave, UART_HandleTypeDef *uart)
{

	while (Wait_for(string, uart) != 1);
	for (int indx=0; indx<numberofchars; indx++)
	{
		while (!(IsDataAvailable(uart)));
		buffertosave[indx] = Uart_read(uart);
	}
	return 1;
}

void Uart_sendstring (const char *s, UART_HandleTypeDef *uart)
{
	while(*s!='\0') Uart_write(*s++, uart);
}


int Wait_for (char *string,UART_HandleTypeDef *uart) // burayı kontrol et
{
	int so_far =0;
	int len = strlen (string);

again_device:
	while (!IsDataAvailable(uart));
	if (Uart_peek(uart) != string[so_far])
	{
		 _rx_buffer3->tail = (unsigned int)(_rx_buffer3->tail + 1) % UART_BUFFER_SIZE ;
		goto again_device;

	}
	while (Uart_peek(uart) == string [so_far])
	{
		so_far++;
		Uart_read(uart);
		if (so_far == len) return 1;
		while (!IsDataAvailable(uart));
	}

	if (so_far != len)
	{
		so_far = 0;
		goto again_device;
	}

	if (so_far == len) return 1;
	else return -1;
}

void Uart_isr (UART_HandleTypeDef *huart)
{
	  uint32_t isrflags   = READ_REG(huart->Instance->SR);
	  uint32_t cr1its     = READ_REG(huart->Instance->CR1);

    /* if DR is not empty and the Rx Int is enabled */
    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {

		huart->Instance->SR;                       /* Read status register */
        unsigned char c = huart->Instance->DR;     /* Read data register */

        if (huart == pc_uart)
        {
           	store_char (c, _rx_buffer2);  // store data in buffer
        }

        else if (huart == lora_uart)
        {
           	store_char (c, _rx_buffer3);  // store data in buffer
        }



        return;
    }

    /*If interrupt is caused due to Transmit Data Register Empty */
    if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
    {

    	if (huart == pc_uart){
        	if(tx_buffer2.head == tx_buffer2.tail)
        	    {
        	      // Buffer empty, so disable interrupts
        	      __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);

        	    }

        	 else
        	    {
        	      // There is more data in the output buffer. Send the next byte
        	      unsigned char c = tx_buffer2.buffer[tx_buffer2.tail];
        	      tx_buffer2.tail = (tx_buffer2.tail + 1) % UART_BUFFER_SIZE;


        	      huart->Instance->SR;
        	      huart->Instance->DR = c;

        	    }
        	}

    	else if (huart == lora_uart){
        	if(tx_buffer3.head == tx_buffer3.tail)
        	    {
        	      // Buffer empty, so disable interrupts
        	      __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);

        	    }

        	 else
        	    {
        	      // There is more data in the output buffer. Send the next byte
        	      unsigned char c = tx_buffer3.buffer[tx_buffer3.tail];
        	      tx_buffer3.tail = (tx_buffer3.tail + 1) % UART_BUFFER_SIZE;


        	      huart->Instance->SR;
        	      huart->Instance->DR = c;

        	    }
        	}

    	return;
    }
}


