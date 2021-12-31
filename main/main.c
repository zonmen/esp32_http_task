#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "http.c"
#include "wifi.c"

void app_main(void){
	wifi_start();
	https_post_request("http://httpbin.org/post", "4");
}
