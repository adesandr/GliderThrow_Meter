/**
 * @file    esp_mad_task_http_client.c
 * @author  Alain Désandré
 * @version 1.0.3
 * @date    November 18th 2018
 * @brief   This file include all the code to deal with a simple http server.
 *
 * @details HTTP server deal with the various http request from the client.
 *			At the fist reception of an http get from one client, the server
 *			respond with the esp_mad HTML page (see esp_html.h).
 *          
 */

/*-----------------------------------------
 *-            INCLUDES        
 *-----------------------------------------*/
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <freertos/event_groups.h>
#include <esp_http_client.h>
#include <string.h>
#include <stdlib.h>
#include <Esp_mad.h>
#include <Esp_mad_Globals_Variables.h>

/* FreeRTOS event group to signal when we are connected & ready to make a request */

static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,

   but we only care about one event - are we connected

   to the AP with an IP? */

const int CONNECTED_BIT = BIT0;

static const char *TAG="Esp_Client";

/**
 *	@fn 	    esp_err_t event_handler(void *ctx, system_event_t *event).
 *	@brief 		Wifi Event handler.
 *	@param[in]	*ctx : httpd_handler_t pointer.
 *	@param[in]	*event : system_event_t event pointer.
 *	@return		ESP_OK
 */
static esp_err_t event_handler(void *ctx, system_event_t *event)
{

    switch(event->event_id) 
	{

    	case SYSTEM_EVENT_STA_START:

        	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");

        	ESP_ERROR_CHECK(esp_wifi_connect());

        	break;

    	case SYSTEM_EVENT_STA_GOT_IP:
        
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

        	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");

        	ESP_LOGI(TAG, "Got IP: '%s'",

            ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

        	break;

    	case SYSTEM_EVENT_STA_DISCONNECTED:

        	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
            
            /* This is a workaround as ESP32 WiFi libs don't currently

               auto-reassociate. */

        	ESP_ERROR_CHECK(esp_wifi_connect());

            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);

    	    break;

	    default:

    	    break;

    } /* edn switch */	

    return ESP_OK;

} /* end event_handler */


/**
 *	@fn 	    static void initialise_wifi(void *arg);
 *	@brief 		wifi initialisation.
 *	@param[in]	void*
 *	@return		void.
 */
static void initialise_wifi(void *arg)
{

    tcpip_adapter_init();

    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t wifi_config = {

        .sta = {

            .ssid = AP_WIFI_SSID,

        },

    };

    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

    ESP_ERROR_CHECK(esp_wifi_start());

} /* end initialise wifi */

/**
 *	@fn 	    esp_err_t _http_event_handler(esp_http_client_event_t *evt)
 *	@brief 		catch the http event
 *	@param[in]	esp_http_client_event_t *evt
 *	@return		void.
 */
esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{

    switch(evt->event_id) {

        case HTTP_EVENT_ERROR:

            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");

            break;

        case HTTP_EVENT_ON_CONNECTED:

            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");

            break;

        case HTTP_EVENT_HEADER_SENT:

            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");

            break;

        case HTTP_EVENT_ON_HEADER:


            break;

        case HTTP_EVENT_ON_DATA:

            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);

            if (!esp_http_client_is_chunked_response(evt->client)) {

                // Write out data

                // printf("%.*s", evt->data_len, (char*)evt->data);

            }

            break;

        case HTTP_EVENT_ON_FINISH:

            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");

            break;

        case HTTP_EVENT_DISCONNECTED:

            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");

            break;

    }

    return ESP_OK;

} /* end _http_event_handler() */

/**
 *	@fn 	    task_http_client
 *	@brief 		Main task for the http client.
 *	@param[in]	void*
 *	@return		void.
 */
void task_http_client(void *ignore)

{
    char post_data[20];

    EventBits_t uxBits;

    esp_http_client_config_t config = {

        .url = "http://192.168.1.1/sensor2",

        .event_handler = _http_event_handler,

    };

    esp_err_t ret = nvs_flash_init();
    
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {

      ESP_ERROR_CHECK(nvs_flash_erase());

      ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    initialise_wifi(NULL);

	while(1)
	{

    uxBits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);    

    /*--- if station connected and MPU calibration done ---*/
    if(((uxBits & CONNECTED_BIT) != 0) && BInit)
        {

        esp_http_client_handle_t client = esp_http_client_init(&config);

        sprintf(post_data,"%.1f", angle);

        esp_http_client_set_url(client, "http://192.168.1.1/sensor2");

        esp_http_client_set_method(client, HTTP_METHOD_POST);

        esp_http_client_set_post_field(client, post_data, strlen(post_data));

        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK) {

            ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",

                esp_http_client_get_status_code(client),

                esp_http_client_get_content_length(client));

            } else {

            ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
            }

        esp_http_client_cleanup(client);


        } /* end if uxBits*/  

    vTaskDelay(900/portTICK_PERIOD_MS);
	
    } /* end while() */

    vTaskDelete(NULL);

} /* end task_http_client() */