/*
 * wifi_module.c
 *
 *  Created on: Nov 14, 2020
 *      Author: anilp
 */

#include <wifi_module.h>


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

char buffer[20];


void wifi_Init (char *SSID, char *PASSWD)
{
	char data[80];

	Ringbuf_init(); // initialization Uarts
	Uart_sendstring("AT&F\r", &huart1); //restore factory default settings
	while (!(Wait_for("\r\nOK\r\n", &huart1))); // wait for response
	Uart_sendstring("\n restore factory default settings\n", pc_uart); //print to pc uart
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	Uart_sendstring("AT+CFUN=1\r", wifi_uart); // resetting the wifi module
	Uart_sendstring("\nRESETTING", pc_uart);
	for (int i=0; i<5; i++)
	{
		Uart_sendstring(".", pc_uart);
		HAL_Delay(1000);
	}
	Uart_sendstring("AT\r", wifi_uart);// did the reset complete ?
	while (!(Wait_for("\r\nOK\r\n", wifi_uart))); // wait for response
	Uart_sendstring("\nOK\n", pc_uart);
	Uart_flush(wifi_uart);
	Uart_sendstring("connecting... to the provided AP\n", pc_uart);
	sprintf (data, "AT+S.SSIDTXT=%s\r",SSID);//enter ssid
	Uart_sendstring(data, wifi_uart);//sending to wifi module the ssid
	while (!(Wait_for("\r\nOK\r\n", wifi_uart))); // sending is done
	//Uart_sendstring(data, pc_uart);
	data[80] = 0;
	HAL_Delay(500);
	sprintf (data, "AT+S.SCFG=wifi_wpa_psk_text,%s\r",PASSWD);
	Uart_sendstring(data, wifi_uart);
	while (!(Wait_for("\r\nOK\r\n", wifi_uart))); // sending is done
	sprintf (data, "\nConnected to,\"%s\"\n", SSID);
	Uart_sendstring(data,pc_uart);
	/* ssid and password are complete*/

	Uart_flush(wifi_uart);
	Uart_sendstring("AT+S.SCFG=wifi_priv_mode,2\r", wifi_uart);
	while (!(Wait_for("\r\nOK\r\n", wifi_uart)));
	Uart_sendstring("AT+S.SCFG=wifi_mode,1\r", wifi_uart);
	while (!(Wait_for("\r\nOK\r\n", wifi_uart)));
	Uart_sendstring("AT+S.SCFG=ip_use_dhcp,1\r", wifi_uart);
	while (!(Wait_for("\r\nOK\r\n", wifi_uart)));

	//http init*/
/*
	Uart_sendstring("AT+S.SCFG=ip_use_httpd,1\r", wifi_uart);
	while (!(Wait_for("\r\nOK\r\n", wifi_uart)));
	Uart_sendstring("AT+S.HTTPD=1\r", wifi_uart);
	while (!(Wait_for("\r\nOK\r\n", wifi_uart)));
	Uart_sendstring("HTTP is done\n",pc_uart);
*/


	Uart_sendstring("AT&W\r", wifi_uart);//save current settings
	while (!(Wait_for("\r\nOK\r", wifi_uart)));
	Uart_sendstring("AT+CFUN=1\r", wifi_uart);
	Uart_sendstring("\n*OK", pc_uart);
	for (unsigned int i=0; i<13; i++)
		{
			Uart_sendstring(".", pc_uart);
			HAL_Delay(1000);
			//Delay_ms(1000);
		}
	// wifi is connected
	//user_led_off();

}



/* For Test WiFi module and NodeJS server */
/*
void button_node1(void)
{
	char buffertosave[20];
	int durum;
	Uart_sendstring("AT+S.HTTPGET=192.168.1.30,/button/get,3000\r",wifi_uart);
	durum = Get_after("HTTP/1.1",20, &buffertosave, wifi_uart);
	//Uart_sendstring("\n durum alindi",pc_uart);
	if(durum == 1)
	{
		//Uart_sendstring(buffertosave,pc_uart);
		if(Look_for("201",buffertosave) == 1)
		{
			//user_led_on();
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
			Uart_sendstring("\nLed 1 On",pc_uart);

		}
		else if(Look_for("202",buffertosave) == 1)
		{
			//user_led_off();
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
			Uart_sendstring("\nLed 1 Off",pc_uart);
		}
	}

}
void button_node2(void)
{
	char buffertosave[20];
	int durum;
	Uart_sendstring("AT+S.HTTPGET=192.168.1.30,/button/get2,3000\r",wifi_uart);
	durum = Get_after("HTTP/1.1",20, &buffertosave, wifi_uart);
	//Uart_sendstring("\n durum alindi",pc_uart);
	if(durum == 1)
	{
		//Uart_sendstring(buffertosave,pc_uart);
		if(Look_for("201",buffertosave) == 1)
		{
			Uart_sendstring("\nLed 2 On",pc_uart);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
			//user_led_on();

		}
		else if(Look_for("202",buffertosave) == 1)
		{
			//user_led_off();
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
			Uart_sendstring("\nLed 2 Off",pc_uart);
		}
	}

}

void Get_Node1(void)

{
	char buffer[250];
	int temp = (int)rand() % 100;
	int press = (int)rand() % 100;
	int hum = (int)rand() % 100;
	sprintf(buffer,"AT+S.HTTPGET=192.168.1.30,/node1/save?temperature=%d&pressure=%d&humidity=%d,3000\r",temp,press,hum);
	Uart_sendstring(buffer,wifi_uart); // uart_write(buffer,wifi_uart) ????


	//Uart_sendstring("AT+S.HTTPGET=192.168.43.23,/node1/save?temperature=28&pressure=02&humidity=1994,3000\r",wifi_uart);
	//HAL_Delay(2000);
	Uart_sendstring("\nGET1--->OK*",pc_uart);
}
void Get_Node2(void)

{
	char buffer[250];
	int temp = (int)rand() % 100;
	int press = (int)rand() % 100;
	int hum = (int)rand() % 100;
	sprintf(buffer,"AT+S.HTTPGET=192.168.1.30,/node2/save?temperature=%d&pressure=%d&humidity=%d,3000\r",temp,press,hum);
	Uart_sendstring(buffer,wifi_uart);


	//Uart_sendstring("AT+S.HTTPGET=192.168.43.23,/node1/save?temperature=28&pressure=02&humidity=1994,3000\r",wifi_uart);
	//HAL_Delay(2000);
	Uart_sendstring("\nGET2--->OK**",pc_uart);
}
*/
/* -------------------------- Test End -------------------------------------------*/

