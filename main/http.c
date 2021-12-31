#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"


esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            //ESP_LOGI("http", "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
          ESP_LOGI("http", "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI("http", "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
           ESP_LOGI("http", "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            if (!esp_http_client_is_chunked_response(evt->client)) {
                printf("%.*s", evt->data_len, (char*)evt->data);
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI("http", "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI("http", "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}


void https_post_request(char* site, char* data)
{
    char local_response_buffer[1000] = {0};
	esp_http_client_config_t config = {
        .event_handler = _http_event_handle,
        .user_data = local_response_buffer,
		.url = site,
    };

	esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, data, strlen(data));
    esp_http_client_perform(client);
    esp_http_client_get_status_code(client);
    esp_http_client_get_content_length(client);
}


void https_get_request(char* site)
{
	 char local_response_buffer[1000] = {0};

	    esp_http_client_config_t config = {
	        .event_handler = _http_event_handle,
	        .user_data = local_response_buffer,
			.url = site,
	    };
	    esp_http_client_handle_t client = esp_http_client_init(&config);
	    esp_http_client_get_status_code(client);
	    esp_http_client_get_content_length(client);

	    ESP_LOG_BUFFER_HEX("SERG", local_response_buffer, strlen(local_response_buffer));
}
