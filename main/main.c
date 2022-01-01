#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include <string.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp32/rom/uart.h"

#include "http.c"
#include "wifi.c"

#define BUF_SIZE (1024)

//buffer to store line from console
uint8_t buff[256];
//flags to send get/post request
int  send_get_request = 0, send_post_request = 0;
//Handle console input
static void IRAM_ATTR uart_intr_handle(void *arg);
//change baud_rate to 9600
void uart_config();
//Get url from console line for get method
void get_request_url(char *url);
//Get url from console line for post method
void post_request_url_body(char *url, char *body);




void app_main(void){
	char url[100], body[100];

	wifi_start();

	uart_config();
	while(1){
		if(send_get_request == 1){
			get_request_url(url);
			get(url);
			send_post_request = send_get_request = 0;
		}else if(send_post_request == 1){
			post_request_url_body(url, body);
			post(url, body);
			send_post_request = send_get_request = 0;
		}
		vTaskDelay( 100 / portTICK_PERIOD_MS);
	}
}

//////////////////////////////////////////////////
void post_request_url_body(char *url, char *body){
	int i, j;
	for(i = 10, j = 0; buff[i] != '\0' && buff[i] != ' '; i++, j++){
		url[j] = buff[i];
	}
	url[j] = '\0';
	i++;
	for(j = 0; buff[i] != '\0' && buff[i] != '\n'; i++, j++){
		body[j] = buff[i];
	}
	body[j] = '\0';
}

void get_request_url(char *url){
	int i, j;
	for(i=9,j=0; buff[i] != '\0' && buff[i] != '\n'; i++, j++ ){
		url[j] = buff[i];
	}
	url[j] = '\0';
}

void uart_config(){
	uart_config_t uart_config_0 = {
		        .baud_rate = 9600, //115200,
		        .data_bits = UART_DATA_8_BITS,
		        .parity    = UART_PARITY_DISABLE,
		        .stop_bits = UART_STOP_BITS_1,
		        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		        .source_clk = UART_SCLK_APB,
		    };
	uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0);
	uart_param_config(UART_NUM_0, &uart_config_0);

	ESP_ERROR_CHECK(uart_isr_free(UART_NUM_0));
	ESP_ERROR_CHECK(uart_isr_register(UART_NUM_0,uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, NULL));
	ESP_ERROR_CHECK(uart_enable_rx_intr(UART_NUM_0));

}


static void IRAM_ATTR uart_intr_handle(void *arg)
{
	int rx_fifo_len, status, i = 0;

	status = UART0.int_st.val;
	rx_fifo_len = UART0.status.rxfifo_cnt;

	while(rx_fifo_len){
	 buff[i++] = UART0.fifo.rw_byte;
	 rx_fifo_len--;
	}
	uart_clear_intr_status(UART_NUM_0, UART_RXFIFO_FULL_INT_CLR|UART_RXFIFO_TOUT_INT_CLR);

	if(buff[5] == (int)'G' && buff[6] == (int)'E' && buff[7] == (int)'T'){
		send_get_request = 1;
	}else
		if(buff[5] == (int)'P' && buff[6] == (int)'O' && buff[7] == (int)'S' && buff[8] == (int)'T'){
			send_post_request = 1;
		}

}

