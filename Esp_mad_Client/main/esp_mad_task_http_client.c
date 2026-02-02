/**
 * @file    esp_mad_task_http_client.c
 * @author  Alain Désandré - alain.desandre@wanadoo.fr
 * @date    November 18th 2018
 * @brief   This file include all the code of the esp_mad client.
 *
 * @details The task initialize Wifi in STA mode and connect to ESP_MAD AP station.
 *          
 */

/*-----------------------------------------
 *-            INCLUDES        
 *-----------------------------------------*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include <esp_http_client.h>
#include <Esp_mad.h>
#include <Esp_mad_Globals_Variables.h>

#define ESP_MAD_SSID_MAXIMUM_RETRY   10  // Nombre maximum de retry de connexion.

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static int s_retry_num = 0;

static const char *TAG="Esp_Client->";

/**
 *	@fn 	    esp_err_t event_handler(void *ctx, system_event_t *event).
 *	@brief 		Wifi Event handler.
 *	@param[in]	*ctx : httpd_handler_t pointer.
 *	@param[in]	*event : system_event_t event pointer.
 *	@return		ESP_OK
 */
static void event_handler(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < ESP_MAD_SSID_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }

 } /* end event_handler */


/**
 *	@fn 	    static void initialise_wifi(void *arg);
 *	@brief 		wifi initialisation.
 *	@param[in]	void*
 *	@return		void.
 */
static void initialise_wifi(void *arg)
{

    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

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

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s",
                 AP_WIFI_SSID);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s",
                 AP_WIFI_SSID);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

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

        case HTTP_EVENT_REDIRECT:

            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        
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
    char post_data[30];

    static const char tag[] = "http_client->";

    float voltage2;

    EventBits_t uxBits;

    esp_http_client_config_t config = {

        .url = "http://192.168.1.1/sensor2",

        .event_handler = _http_event_handler,

    };

    /*--- Initialize nvs partition ---*/
    esp_err_t ret = nvs_flash_init();
    
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {

      ESP_ERROR_CHECK(nvs_flash_erase());

      ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    initialise_wifi(NULL);

	while(1)
	{

    uxBits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);    

    voltage2 = g_voltage/1000.0;

    ESP_LOGI(tag, "voltage2 %f - voltage %d\n", voltage2, g_voltage);

    /*--- if station connected and MPU calibration done ---*/
    if(((uxBits & WIFI_CONNECTED_BIT) != 0) && BInit)
        {

        esp_http_client_handle_t client = esp_http_client_init(&config);

        sprintf(post_data,"{\"angle\":%0.1f,\"voltage\":%0.2f}", g_angle, voltage2);

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