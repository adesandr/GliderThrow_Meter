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
 * @remarks 1/08/2025 - Ported to NETIF
 *          NETIF component is a successor of the tcpip_adapter, former network interface abstraction,
 *          which has become deprecated since IDF v4.1.
 *          esp_event_loop has been also deprecated. esp_event is the new component.
 *
 */

/*-----------------------------------------
 *-            INCLUDES
 *-----------------------------------------*/
#include <esp_wifi.h>
// #include <esp_event_loop.h>
#include <esp_event.h>
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
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_mac.h"
#include <Esp_mad.h>
#include <Esp_mad_Globals_Variables.h>

/*-----------------------------------------
 *-            LOCALS VARIABLES
 *-----------------------------------------*/
float travel2 = 0.0;
float angle2 = 0.0;

float maxiTravelSensor1 = 0.0;
float miniTravelSensor1 = 0.0;
float maxiTravelSensor2 = 0.0;
float miniTravelSensor2 = 0.0;

float voltage2 = 0.0;

extern const uint8_t esp_html_start[] asm("_binary_esp_html_start");
extern const uint8_t esp_html_end[] asm("_binary_esp_html_end");

extern const uint8_t bootstrap_min_css_start[] asm("_binary_bootstrap_min_css_start");
extern const uint8_t bootstrap_min_css_end[] asm("_binary_bootstrap_min_css_end");

extern const uint8_t bootstrap_min_js_start[] asm("_binary_bootstrap_min_js_start");
extern const uint8_t bootstrap_min_js_end[] asm("_binary_bootstrap_min_js_end");

extern const uint8_t jquery_3_3_1_min_js_start[] asm("_binary_jquery_3_3_1_min_js_start");
extern const uint8_t jquery_3_3_1_min_js_end[] asm("_binary_jquery_3_3_1_min_js_end");

static const char *TAG = "Esp_Server->";

/**
 *	@fn 	    esp_err_t main_page_get_handler (httpd_req_t *req)
 *	@brief 		An HTTP GET handler for the main esp.html page.
 *	@param[in]	*req : un http_req_t pointer.
 *	@return		ESP_OK
 */
esp_err_t main_page_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Entering ----> main_page_get-handler()\n");

    httpd_resp_set_type(req, "text/html");

    httpd_resp_send(req, (const char *)esp_html_start, (esp_html_end - 1) - esp_html_start);

    ESP_LOGI(TAG, "Exit    ----> main_page_get-handler()\n");

    return ESP_OK;

} /* end main_page_get_handler() */

httpd_uri_t main_page = {

    .uri = "/",

    .method = HTTP_GET,

    .handler = main_page_get_handler,

    .user_ctx = NULL

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

    httpd_resp_set_type(req, "text/css");

    httpd_resp_send(req, (const char *)bootstrap_min_css_start, (bootstrap_min_css_end - 1) - bootstrap_min_css_start);

    ESP_LOGI(TAG, "Exit    ----> bootstrap_min_css_handler()\n");

    return ESP_OK;

} /* end bootstrap_min_css_handler() */

httpd_uri_t bootstrap_min_css_uri = {

    .uri = "/bootstrap.min.css",

    .method = HTTP_GET,

    .handler = bootstrap_min_css_handler,

    .user_ctx = NULL

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

    httpd_resp_set_type(req, "application/javascript");

    httpd_resp_send(req, (const char *)bootstrap_min_js_start, (bootstrap_min_js_end - 1) - bootstrap_min_js_start);

    ESP_LOGI(TAG, "Exit    ----> Bootstrap_min_js Requested()\n");

    return ESP_OK;

} /* end bootstrap_min_js_handler() */

httpd_uri_t bootstrap_min_js_uri = {

    .uri = "/bootstrap.min.js",

    .method = HTTP_GET,

    .handler = bootstrap_min_js_handler,

    .user_ctx = NULL

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

    httpd_resp_set_type(req, "application/javascript");

    httpd_resp_send(req, (const char *)jquery_3_3_1_min_js_start, (jquery_3_3_1_min_js_end - 1) - jquery_3_3_1_min_js_start);

    ESP_LOGI(TAG, "Exit     ----> jquery_3_3_1_min_js Requested()\n");

    return ESP_OK;

} /* end jquery_3_3_1_min_js_handler() */

httpd_uri_t jquery_3_3_1_min_js_uri = {

    .uri = "/jquery-3.3.1.min.js",

    .method = HTTP_GET,

    .handler = jquery_3_3_1_min_js_handler,

    .user_ctx = NULL};

/**
 *	@fn 	    esp_err_t sensors_get_handler (httpd_req_t *req)
 *	@brief 		An HTTP GET handler to serve, travel, angle and delta of both sensors
 *	@param[in]	*req : an http_req_t pointer.
 *	@return		ESP_OK
 */
esp_err_t sensors_get_handler(httpd_req_t *req)
{

    char *buf;

    size_t buf_len;

    float DeltaTravel;
    float DeltaAngle;

    float voltage1 = 0.0;

    /* Get header value string length and allocate memory for length + 1,

     * extra byte for null termination */

    ESP_LOGI(TAG, "Entering ----> sensor_get_handler()\n");

    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;

    if (buf_len > 1)
    {

        buf = malloc(buf_len);

        /* Copy null terminated value string into buffer */

        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK)
        {

            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }

        free(buf);
    }

    buf = malloc(250);

    memset(buf, 0, sizeof(buf) - 1);

    /*--- Compute Min, Max and Deltas for both sensors ---*/
    DeltaTravel = g_travel - travel2;
    DeltaAngle = g_angle - angle2;

    if (g_travel > maxiTravelSensor1)
        maxiTravelSensor1 = g_travel;
    if (g_travel < miniTravelSensor1)
        miniTravelSensor1 = g_travel;

    if (travel2 > maxiTravelSensor2)
        maxiTravelSensor2 = travel2;
    if (travel2 < miniTravelSensor2)
        miniTravelSensor2 = travel2;

    /*--- compute voltage in volt ---*/
    voltage1 = g_voltage / 1000.0;

    ESP_LOGI(TAG, "voltage1 %f - voltage2 %f", voltage1, voltage2);

    /*--- Preparing the buffer request in json format ---*/
    sprintf(buf, "{\"travel1\":%0.1f,\"travel2\":%0.1f,\"DeltaTravel\":%0.1f,\"angle1\":%0.1f,\"angle2\":%0.1f,\"DeltaAngle\":%0.1f,\"maxiTravelSensor1\":%0.1f,\"miniTravelSensor1\":%0.1f, \"maxiTravelSensor2\":%0.1f, \"miniTravelSensor2\":%0.1f,\"voltage1\":%0.2f, \"voltage2\":%0.2f}", (g_travel < 0 ? (-1 * g_travel) : g_travel), (travel2 < 0 ? (-1 * travel2) : travel2), (DeltaTravel < 0 ? (-1 * DeltaTravel) : DeltaTravel), (g_angle < 0 ? (-1 * g_angle) : g_angle), (angle2 < 0 ? (-1 * angle2) : angle2), (DeltaAngle < 0 ? (-1 * DeltaAngle) : DeltaAngle), (-1 * miniTravelSensor1), maxiTravelSensor1, (-1 * miniTravelSensor2), maxiTravelSensor2, voltage1, voltage2);

    ESP_LOGI(TAG, "[len = %d]  \n", strlen(buf));

    ESP_LOGI(TAG, "json = %s\n", buf);

    httpd_resp_set_type(req, "text/plain");

    /*--- Send the request ---*/
    httpd_resp_send(req, buf, strlen(buf));

    free(buf);

    ESP_LOGI(TAG, "Exit    ----> sensor_get_handler()\n");

    return ESP_OK;

} /* end sensors_get_handler() */

httpd_uri_t sensors = {

    .uri = "/sensors",

    .method = HTTP_GET,

    .handler = sensors_get_handler,

    .user_ctx = NULL

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

    memset(buf, 0, sizeof(buf) - 1);

    while (remaining > 0)
    {

        /* Read the data for the request */

        if ((ret = httpd_req_recv(req, buf,

                                  MIN(remaining, sizeof(buf)))) <= 0)
        {

            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {

                /* Retry receiving if timeout occurred */

                continue;
            }

            return ESP_FAIL;
        }

        /* Log data received */

        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");

        ESP_LOGI(TAG, "%.*s", ret, buf);

        ESP_LOGI(TAG, "====================================");

        /*--- Parse Json buffer received form client ---*/
        sensor2_json = cJSON_Parse(buf);
        json_angle = cJSON_GetObjectItemCaseSensitive(sensor2_json, "angle");
        angle2 = json_angle->valuedouble;
        json_voltage = cJSON_GetObjectItemCaseSensitive(sensor2_json, "voltage");
        voltage2 = json_voltage->valuedouble;
        cJSON_Delete(sensor2_json);

        /*--- Compute travel2 ---*/
        travel2 = g_chordControlSurface * sin((angle2 * (2.0 * PI) / 360.0) / 2.0) * 2.0;

        ESP_LOGI(TAG, "angle2 : %.1f - travel2 : %.1f - voltage2 : %.2f\n", angle2, travel2, voltage2);

        /* Send response to the client, by default 200 OK status in the mime type */
        httpd_resp_send(req, NULL, 0);

        remaining -= ret;
    }

    ESP_LOGI(TAG, "Exit ----> sensor2_post_handler()\n");

    return ESP_OK;

} /* end sensor2_post_handler() */

httpd_uri_t sensor2 = {

    .uri = "/sensor2",

    .method = HTTP_POST,

    .handler = sensor2_post_handler,

    .user_ctx = NULL

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

    int oldchordValue = g_chordControlSurface;

    int ret, remaining = req->content_len;

    int iTemp;

    ESP_LOGI(TAG, "Entering ----> chord_post_handler()\n");
    ESP_LOGI(TAG, "method: %d\n", req->method);
    ESP_LOGI(TAG, "uri: %s\n", req->uri);

    while (remaining > 0)
    {

        /* Read the data for the request */

        if ((ret = httpd_req_recv(req, buf,

                                  MIN(remaining, sizeof(buf)))) <= 0)
        {

            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {

                /* Retry receiving if timeout occurred */

                continue;
            }

            return ESP_FAIL;
        }

        /* Log data received */

        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");

        ESP_LOGI(TAG, "%.*s", ret, buf);

        ESP_LOGI(TAG, "====================================");

        param[0] = buf[11];
        param[1] = buf[12];
        param[2] = '\0';

        iTemp = atoi(param);

        if (iTemp > 0)
        {

            g_chordControlSurface = iTemp;

            sprintf(buf, "Changing chord from %d mm to %d mm\n", oldchordValue, g_chordControlSurface);
        }
        else
            sprintf(buf, "ERROR : chord must be a positive value\n");

        /* Send response to the client */
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_send(req, buf, strlen(buf));

        remaining -= ret;
    }

    ESP_LOGI(TAG, "Exit    ----> chord_post_handler()\n");

    return ESP_OK;

} /* end chord_post_handler() */

httpd_uri_t chord = {

    .uri = "/chord",

    .method = HTTP_POST,

    .handler = chord_post_handler,

    .user_ctx = NULL

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

    if (httpd_start(&server, &config) == ESP_OK)
    {

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
 *	@fn 	    esp_err_t wifi_event_handler(void *ctx, system_event_t *event).
 *	@brief 		task launch the function to initialize handler event .
 *	@param[in]	*ctx : httpd_handler_t pointer.
 *	@param[in]	*event : system_event_t event pointer.
 *	@return		ESP_OK
 */
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                                    int32_t event_id, void *event_data)
{

 if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d, reason=%d",
                 MAC2STR(event->mac), event->aid, event->reason);
    }

 } /* end event_handler() */

/**
 *	@fn 	    static void initialise_wifi_in_ap(void *arg);
 *	@brief 		wifi initialisation.
 *	@param[in]	void*
 *	@return		void.
 */
static void initialise_wifi_in_ap(void)
{

    /*--- disable wifi driver logging ---*/
    esp_log_level_set("wifi", ESP_LOG_NONE);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = AP_WIFI_SSID,
            .ssid_len = strlen(AP_WIFI_SSID),
            .channel = 0,
            .max_connection = 3,
            .authmode = WIFI_AUTH_OPEN,
            .beacon_interval = 100},
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.ap.ssid);

} /* end initialise_wifi_in_ap */

/**
 *	@fn 	    static esp_err_t stop_server(httpd_handle_t server)
 *	@brief 		Stop the Web_server instance.
 *	@param[in]	httpd_handle_t server
 *	@return		esp_err_t
 */
static esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_stop(server);
}

/**
 *	@fn 	    static void disconnect_handler(void *arg, esp_event_base_t event_base, int32_t event_id,void* event_data)
 *	@brief 		Called when Station is disconnected
 *	@param[in]	see prototype
 *	@return		void
 */
static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop http server");
        }
    }
}

/**
 *	@fn 	    static void connect_handler(void *arg, esp_event_base_t event_base, int32_t event_id,void* event_data)
 *	@brief 		Called when Station is connected
 *	@param[in]	see prototype
 *	@return		void
 */
static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

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

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {

        ESP_ERROR_CHECK(nvs_flash_erase());

        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    /*--- start wifi driver in AP mode ---*/
    initialise_wifi_in_ap();

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

    /* Start the server for the first time */
    server = start_webserver();

    /*--- infinite loop to serve wifi event and http request ---*/
    while (1)
    {
        vTaskDelay(300 / portTICK_PERIOD_MS);
    } /* end while() */

    /*--- this part will not be executed but we stick the free rtos reco. ---*/
    vTaskDelete(NULL);
}