/**
 * @file    esp_mad_task_http_server.c
 * @author  Alain Désandré - alain.desandre@wanadoo.fr
 * @date    November 18th 2018
 * @brief   This file include all the code to deal with a simple http server.
 *
 * @details HTTP server deal with the various http request from the clients.
 *			At the fist reception of an http get from one client, the server
 *			respond with the requested uri (see WebsiteFiles/ for the various uri).
 *          esp_mad_task_http_server deals with :
 *              - nvs intialisation
 *              - dhcp server intialisation
 *              - wifi driver initialisation in soft AP mode
 *              - when AP station is started, the http server is launched and the uri handles are setup
 *          Uris are embedded in the .rodata DRAM segment (see CMakeList.txt)
 *          Main HTML page is WebsiteFiles/esp.html and used bootstrap framework and jquery.
 *          Ressources for bootstrap and jquery are minified version in WebsiteFiles/ 
 *          
 */

/*-----------------------------------------
 *-            INCLUDES        
 *-----------------------------------------*/
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_http_server.h>
#include <math.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <cJSON.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <stdlib.h>
#include <Esp_mad.h>
#include <Esp_mad_Globals_Variables.h>

/*-----------------------------------------
 *-            LOCALS VARIABLES        
 *-----------------------------------------*/
float travel2   = 0.0;
float angle2    = 0.0;

float maxiTravelSensor1 = 0.0;
float miniTravelSensor1 = 0.0;
float maxiTravelSensor2 = 0.0;
float miniTravelSensor2 = 0.0;

float voltage2 = 0.0;

extern const uint8_t esp_html_start[] asm("_binary_esp_html_start");
extern const uint8_t esp_html_end[]   asm("_binary_esp_html_end");

extern const uint8_t bootstrap_min_css_start[] asm("_binary_bootstrap_min_css_start");
extern const uint8_t bootstrap_min_css_end[]   asm("_binary_bootstrap_min_css_end");

extern const uint8_t bootstrap_min_js_start[] asm("_binary_bootstrap_min_js_start");
extern const uint8_t bootstrap_min_js_end[]   asm("_binary_bootstrap_min_js_end");

extern const uint8_t jquery_3_3_1_min_js_start[] asm("_binary_jquery_3_3_1_min_js_start");
extern const uint8_t jquery_3_3_1_min_js_end[]   asm("_binary_jquery_3_3_1_min_js_end");

static EventGroupHandle_t wifi_event_group;
const int CLIENT_CONNECTED_BIT = BIT0;
const int CLIENT_DISCONNECTED_BIT = BIT1;

static const char *TAG="Esp_Server->";

/**
 *	@fn 	    esp_err_t main_page_get_handler (httpd_req_t *req)
 *	@brief 		An HTTP GET handler for the main esp.html page.
 *	@param[in]	*req : un http_req_t pointer.
 *	@return		ESP_OK
 */
esp_err_t main_page_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Entering ----> main_page_get-handler()\n");

    httpd_resp_set_type(req,"text/html");

    httpd_resp_send(req, (const char *)esp_html_start, (esp_html_end-1) - esp_html_start);

    ESP_LOGI(TAG, "Exit    ----> main_page_get-handler()\n");
    
    return ESP_OK;
    
} /* end main_page_get_handler() */

httpd_uri_t main_page = {

    .uri       = "/",

    .method    = HTTP_GET,

    .handler   = main_page_get_handler,

    .user_ctx  = NULL

};

/**
 *	@fn 	    esp_err_t bootstrap_min_css_handler (httpd_req_t *req)
 *	@brief 		An HTTP GET handler for bootstrap.min.css uri.
 *	@param[in]	*req : un http_req_t pointer.
 *	@return		ESP_OK
 */
esp_err_t bootstrap_min_css_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Entering ----> bootstrap_min_css_handler()\n");

    httpd_resp_set_type(req,"text/css");

    httpd_resp_send(req, (const char *)bootstrap_min_css_start, (bootstrap_min_css_end-1) - bootstrap_min_css_start);

    ESP_LOGI(TAG, "Exit    ----> bootstrap_min_css_handler()\n");

    return ESP_OK;
    
} /* end bootstrap_min_css_handler() */

httpd_uri_t bootstrap_min_css_uri = {

    .uri       = "/bootstrap.min.css",

    .method    = HTTP_GET,

    .handler   = bootstrap_min_css_handler,

    .user_ctx  = NULL

};

/**
 *	@fn 	    esp_err_t bootstrap_min_js_handler (httpd_req_t *req)
 *	@brief 		An HTTP GET handler for bootstrap.min.js uri.
 *	@param[in]	*req : un http_req_t pointer.
 *	@return		ESP_OK.
 */
esp_err_t bootstrap_min_js_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Entering ----> Bootstrap_min_js Requested()\n");

    httpd_resp_set_type(req,"application/javascript");

    httpd_resp_send(req, (const char *)bootstrap_min_js_start, (bootstrap_min_js_end-1) - bootstrap_min_js_start);

    ESP_LOGI(TAG, "Exit    ----> Bootstrap_min_js Requested()\n");

    return ESP_OK;
    
} /* end bootstrap_min_js_handler() */

httpd_uri_t bootstrap_min_js_uri = {

    .uri       = "/bootstrap.min.js",

    .method    = HTTP_GET,

    .handler   = bootstrap_min_js_handler,

    .user_ctx  = NULL

};

/**
 *	@fn 	    esp_err_t jquery_3_3_1_min_js_handler (httpd_req_t *req)
 *	@brief 		An HTTP GET handler for jquery.3.3.1.min.js uri.
 *	@param[in]	*req : un http_req_t pointer.
 *	@return		ESP_OK.
 */
esp_err_t jquery_3_3_1_min_js_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Entering ----> jquery_3_3_1_min_js Requested()\n");

    httpd_resp_set_type(req,"application/javascript");

    httpd_resp_send(req, (const char *)jquery_3_3_1_min_js_start, (jquery_3_3_1_min_js_end-1) - jquery_3_3_1_min_js_start);

    ESP_LOGI(TAG, "Exit     ----> jquery_3_3_1_min_js Requested()\n");

    return ESP_OK;
    
} /* end jquery_3_3_1_min_js_handler() */

httpd_uri_t jquery_3_3_1_min_js_uri = {

    .uri       = "/jquery-3.3.1.min.js",

    .method    = HTTP_GET,

    .handler   = jquery_3_3_1_min_js_handler,

    .user_ctx  = NULL
};

/**
 *	@fn 	    esp_err_t sensors_get_handler (httpd_req_t *req)
 *	@brief 		An HTTP GET handler to serve, travel, angle and delta of both sensors
 *	@param[in]	*req : an http_req_t pointer.
 *	@return		ESP_OK
 */
esp_err_t sensors_get_handler(httpd_req_t *req)
{

    char*  buf;

    size_t buf_len;

    float DeltaTravel;
    float DeltaAngle;

    float voltage1 = 0.0;
    float voltage2 = 0.0;

    /* Get header value string length and allocate memory for length + 1,

     * extra byte for null termination */

    ESP_LOGI(TAG, "Entering ----> sensor_get_handler()\n");

    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;

    if (buf_len > 1) {

        buf = malloc(buf_len);

        /* Copy null terminated value string into buffer */

        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {

            ESP_LOGI(TAG, "Found header => Host: %s", buf);

        }

        free(buf);

    }

    buf = malloc(250);

    memset(buf,0,sizeof(buf)-1);

    /*--- Compute Min, Max and Deltas for both sensors ---*/
    DeltaTravel = travel - travel2;
    DeltaAngle = angle - angle2;

    if (travel > maxiTravelSensor1)
        maxiTravelSensor1 = travel;
    if (travel < miniTravelSensor1)
        miniTravelSensor1 = travel;

    if (travel2 > maxiTravelSensor2)
        maxiTravelSensor2 = travel2;
    if (travel2 < miniTravelSensor2)
        miniTravelSensor2 = travel2; 

    /*--- compute voltage in volt ---*/
    voltage1 = voltage/1000.0;

    ESP_LOGI(TAG, "voltage1 %f - voltage2 %f", voltage1, voltage2);

    /*--- Preparing the buffer request in json format ---*/
    sprintf(buf,"{\"travel1\":%0.1f,\"travel2\":%0.1f,\"DeltaTravel\":%0.1f,\"angle1\":%0.1f,\"angle2\":%0.1f,\"DeltaAngle\":%0.1f, \"maxiTravelSensor1\":%0.1f, \"miniTravelSensor1\":%0.1f,\"maxiTravelSensor2\":%0.1f,\"miniTravelSensor2\":%0.1f,\"voltage1\":%0.2f, \"voltage2\":%0.2f}",travel, travel2, DeltaTravel, angle, angle2, DeltaAngle, maxiTravelSensor1, miniTravelSensor1, maxiTravelSensor2, miniTravelSensor2, voltage1, voltage2);

 	ESP_LOGI(TAG, "[len = %d]  \n", strlen(buf));

    ESP_LOGI(TAG,"json = %s\n", buf);

    httpd_resp_set_type(req,"text/plain");

    /*--- Send the request ---*/
    httpd_resp_send(req, buf, strlen(buf));

    free(buf);

    ESP_LOGI(TAG, "Exit    ----> sensor_get_handler()\n");

    return ESP_OK;

} /* end sensors_get_handler() */

httpd_uri_t sensors = {

    .uri       = "/sensors",

    .method    = HTTP_GET,

    .handler   = sensors_get_handler,

    .user_ctx  = NULL

};

/**
 *	@fn 	    esp_err_t chord_post_handler (httpd_req_t *req)
 *	@brief 		An HTTP POST handler.
 *	@param[in]	*req : un http_req_t pointer.
 *	@return		
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t sensor2_post_handler(httpd_req_t *req)
{

    char buf[40];

    int ret, remaining = req->content_len;
    cJSON *sensor2_json = NULL;
    const cJSON *json_angle = NULL;
    const cJSON *json_voltage = NULL;

    ESP_LOGI(TAG, "Entering ----> sensor2_post_handler()\n");
    ESP_LOGI(TAG, "method: %d\n", req->method);
    ESP_LOGI(TAG, "uri: %s\n", req->uri);

    memset(buf,0,sizeof(buf)-1);

    while (remaining > 0) {

        /* Read the data for the request */

        if ((ret = httpd_req_recv(req, buf,

                        MIN(remaining, sizeof(buf)))) <= 0) {

            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {

                /* Retry receiving if timeout occurred */

                continue;

            }

            return ESP_FAIL;

        }

        /* Log data received */

        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");

        ESP_LOGI(TAG, "%.*s", ret, buf);

        ESP_LOGI(TAG, "====================================");

        /*--- Parse Json buffer receved form client ---*/
        sensor2_json = cJSON_Parse(buf);
        json_angle = cJSON_GetObjectItemCaseSensitive(sensor2_json, "angle");
        angle2 = json_angle->valuedouble;
        json_voltage = cJSON_GetObjectItemCaseSensitive(sensor2_json, "voltage");
        voltage2 = json_voltage->valuedouble;
        cJSON_Delete(sensor2_json);

        /*--- Compute travel2 ---*/
        travel2 = chordControlSurface * sin((angle2*(2.0*PI)/360.0)/2.0) * 2.0;

        ESP_LOGI(TAG,"angle2 : %.1f - travel2 : %.1f - voltage2 : %.2f\n",angle2, travel2, voltage2);

        /* Send response to the client, by default 200 OK status in the mime type */
        httpd_resp_send(req, NULL, 0);

        remaining -= ret;

    }

    ESP_LOGI(TAG, "Exit ----> sensor2_post_handler()\n");

    return ESP_OK;

} /* end sensor2_post_handler() */

httpd_uri_t sensor2 = {

    .uri       = "/sensor2",

    .method    = HTTP_POST,

    .handler   = sensor2_post_handler,

    .user_ctx  = NULL

};

/**
 *	@fn 	    esp_err_t chord_post_handler (httpd_req_t *req)
 *	@brief 		An HTTP POST handler.
 *	@param[in]	*req : un http_req_t pointer.
 *	@return		
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t chord_post_handler(httpd_req_t *req)
{

    char buf[50];

    char param[3];

    int oldchordValue = chordControlSurface;

    int ret, remaining = req->content_len;

    int iTemp;

    ESP_LOGI(TAG, "Entering ----> chord_post_handler()\n");
    ESP_LOGI(TAG, "method: %d\n", req->method);
    ESP_LOGI(TAG, "uri: %s\n", req->uri);
    
    while (remaining > 0) {

        /* Read the data for the request */

        if ((ret = httpd_req_recv(req, buf,

                        MIN(remaining, sizeof(buf)))) <= 0) {

            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {

                /* Retry receiving if timeout occurred */

                continue;

            }

            return ESP_FAIL;

        }

        /* Log data received */

        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");

        ESP_LOGI(TAG, "%.*s", ret, buf);

        ESP_LOGI(TAG, "====================================");

        param[0]=buf[11];
        param[1]=buf[12];
        param[2]='\0';

        iTemp = atoi(param);

        if(iTemp > 0)
        {

            chordControlSurface = iTemp;

            sprintf(buf, "Changing chord from %d mm to %d mm\n", oldchordValue, chordControlSurface);
        }
        else
            sprintf(buf, "ERROR : chord must be a positive value\n");

        /* Send response to the client */
        httpd_resp_set_type(req,"text/plain");
        httpd_resp_send(req, buf, strlen(buf));

        remaining -= ret;

    }

    ESP_LOGI(TAG, "Exit    ----> chord_post_handler()\n");

    return ESP_OK;

} /* end chord_post_handler() */

httpd_uri_t chord = {

    .uri       = "/chord",

    .method    = HTTP_POST,

    .handler   = chord_post_handler,

    .user_ctx  = NULL

};

/**
 *	@fn 	    httpd_handle_t start_webserver (void)
 *	@brief 		Start the http server and stet the uris handles
 *	@param[in]	void
 *	@return		NULL or a pointer on http_handle_t server.
 */
httpd_handle_t start_webserver(void)
{

    httpd_handle_t server = NULL;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server

    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);

    if (httpd_start(&server, &config) == ESP_OK) {

        // Set URI handlers

        ESP_LOGI(TAG, "Registering URI handlers");

        httpd_register_uri_handler(server, &main_page);

        httpd_register_uri_handler(server, &bootstrap_min_css_uri);

        httpd_register_uri_handler(server, &bootstrap_min_js_uri);

        httpd_register_uri_handler(server, &jquery_3_3_1_min_js_uri);

        httpd_register_uri_handler(server, &chord);

        httpd_register_uri_handler(server, &sensors);

        httpd_register_uri_handler(server, &sensor2);

        return server;

    }

    ESP_LOGI(TAG, "Error starting httpd server!");

    return NULL;

}

/**
 *	@fn 	    vod stop_webserver (httpd_handle_t server)
 *	@brief 		stop the httpd web server.
 *	@param[in]	http_handle_t server.
 *	@return		void.
 */
void stop_webserver(httpd_handle_t server)
{

    // Stop the httpd server

    httpd_stop(server);

} /* end stop_webserver() */

/**
 *	@fn 	    esp_err_t event_handler(void *ctx, system_event_t *event).
 *	@brief 		task launch the function to initialize handler event .
 *	@param[in]	*ctx : httpd_handler_t pointer.
 *	@param[in]	*event : system_event_t event pointer.
 *	@return		ESP_OK
 */
 static esp_err_t event_handler(void *ctx, system_event_t *event)
{

    httpd_handle_t *server = (httpd_handle_t *) ctx;

    switch(event->event_id) {

        case SYSTEM_EVENT_AP_START:

	        ESP_LOGI(TAG, "SYSTEM_EVENT_AP_START:ESP32 is started in AP mode\n");

            if (*server == NULL) {

	            *server = start_webserver();

    	    }

		break;
	
	    case SYSTEM_EVENT_AP_STACONNECTED:

            ESP_LOGI(TAG, "SYSTEM_EVENT_AP_STACONNECTED: station:"MACSTR" join, AID=%d",

                    MAC2STR(event->event_info.sta_connected.mac),

                    event->event_info.sta_connected.aid);

		    xEventGroupSetBits(wifi_event_group, CLIENT_CONNECTED_BIT);

		    break;


    	case SYSTEM_EVENT_AP_STADISCONNECTED:

            ESP_LOGI(TAG, "SYSTEM_EVENT_AP_STADISCONNECTED\n");

	    	xEventGroupSetBits(wifi_event_group, CLIENT_DISCONNECTED_BIT);

            ESP_LOGI(TAG, "station:"MACSTR"leave, AID=%d",

                MAC2STR(event->event_info.sta_disconnected.mac),

                 event->event_info.sta_disconnected.aid);

		    break;		

            case SYSTEM_EVENT_AP_STOP :

                ESP_LOGI(TAG, "SYSTEM_EVENT_AP_STOP:ESP32 is stop in AP mode\n");
            	
                if (*server) {

    	        stop_webserver(*server);

        	    *server = NULL;

	            }	
                break;

            case SYSTEM_EVENT_AP_STAIPASSIGNED :

                ESP_LOGI(TAG, "SYSTEM_EVENT_AP_STAIPASSIGNED\n");

                break;
                
        default:

            ESP_LOGI(TAG,"UNKNOW_EVENT:event->event_id = %d\n", event->event_id);

            break;

    }

    return ESP_OK;

} /* end event_handler() */

/**
 *	@fn 	    static void start_dhcp_server(void *arg);
 *	@brief 		DHCP Server initialisation.
 *	@param[in]	void*
 *	@return		void.
 */
static void start_dhcp_server(){ 

    	// initialize the tcp stack

	    tcpip_adapter_init();

        // stop DHCP server

        ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));

        // assign a static IP to the network interface

        tcpip_adapter_ip_info_t info;

        memset(&info, 0, sizeof(info));

        IP4_ADDR(&info.ip, 192, 168, 1, 1);

        //ESP acts as router, so gw addr will be its own addr
        IP4_ADDR(&info.gw, 192, 168, 1, 1);
        
        IP4_ADDR(&info.netmask, 255, 255, 255, 0);

        ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info));

        // start the DHCP server   

        ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));

        ESP_LOGI(TAG, "DHCP server started \n");
}

/**
 *	@fn 	    static void initialise_wifi_in_ap(void *arg);
 *	@brief 		wifi initialisation.
 *	@param[in]	void*
 *	@return		void.
 */
static void initialise_wifi_in_ap(void *arg)
{

    /*--- disable wifi driver logging ---*/
    esp_log_level_set("wifi", ESP_LOG_NONE);

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, arg));

    wifi_event_group = xEventGroupCreate();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t wifi_config = {

        .ap = {

            .ssid = AP_WIFI_SSID,
                        
            .channel = 0,

            .authmode = WIFI_AUTH_OPEN,

            .ssid_hidden = 0,

            .max_connection = 3,

            .beacon_interval = 100

        },

    };

    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.ap.ssid);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));

    ESP_ERROR_CHECK(esp_wifi_start());

} /* end initialise wifi */

/**
 *	@fn 	    task_http_server.
 *	@brief 		task launch the function to initialize the http server .
 *	@param[in]	void*
 *	@return		void.
 */
void task_http_server(void *ignore)

{

    static httpd_handle_t server = NULL;

    /*--- Initialize nvs partition ---*/
    esp_err_t ret = nvs_flash_init();
    
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {

      ESP_ERROR_CHECK(nvs_flash_erase());

      ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    /*--- start dhcp_server to serve stations ---*/
    start_dhcp_server();

    /*--- start wifi driver in AP mode ---*/
    initialise_wifi_in_ap(&server);

    /*--- infinite loop to serve wifi event and http request ---*/
	while(1)
	{
    	vTaskDelay(300/portTICK_PERIOD_MS);
	} /* end while() */

    /*--- this part will not be executed but we stick the free rtos recomendation ---*/
	vTaskDelete(NULL);

}