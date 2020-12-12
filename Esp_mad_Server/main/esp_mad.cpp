/**
 * @file    esp_mad.cpp
 * @author  Alain Désandré
 * @version 1.0.3
 * @date    28 octobre 2018
 * @brief   esp_mad project main files.
 *
 * @details The MAD application is intended to provide a ESP32-based tool
 *          for visualizing the deflection (in mm and degree) of control surface
 *          on a remote-controlled model.
 *          ESP_MAD is composed of four files :
 *              . esp_mad.ccp : main function.
 *              . esp_mad_task_measure.cpp : MPU6050 management
 *              . esp_mad_task_http_server.c : http server
 *              . esp_mad.h : Globals define
 *              . esp_html.h : HTML page and Jquery/Javascript function.
 * 
 *          Version history
 *              23/05/2019 : new version of gpio management, post V3.3 esp-idf
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <esp_log.h>
#include <esp_err.h>
#include "driver/gpio.h"
#include <Esp_mad.h>
#define DEFINE_VARIABLES
#include <Esp_mad_Globals_Variables.h>

extern "C" {
	void app_main(void);
    void task_http_server(void*);
}

extern void task_measure(void*);
extern void task_http_server(void*);

/**
 *	@fn 	    app_main(void)
 *	@brief 		esp_mad project entry point function.
 *	@param[in]	void*
 *	@return		void.
 */
void app_main(void)
{
    int iDelay = 100;

    /*--- two tasks are launched. One task handle the MPU6050 measurement and   ---*/
    /*--- the other one is a pretty simple http server to deal with the browser ---*/
    /*--- requests.										                        ---*/
    xTaskCreate(&task_measure, "measure_task", 8192, NULL, 5, NULL);

	/*--- Wait for stabilisation, not mandatory ---*/
    vTaskDelay(500/portTICK_PERIOD_MS);

    xTaskCreate(&task_http_server, "http_server_task", 8192, NULL, 5, NULL);

    /*---  Configure the IOMUX register for pad BLINK_GPIO (some pads are ---*/
    /*---  muxed to GPIO on reset already, but some default to other      ---*/
    /*---  functions and need to be switched to GPIO. Consult the         ---*/
    /*---  Technical Reference for a list of pads and their default       ---*/
    /*---  functions.). Blinky led.                                       ---*/
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = (gpio_int_type_t)GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set
    io_conf.pin_bit_mask = (1ULL<<BLINK_GPIO);
    //disable pull-down mode
    io_conf.pull_down_en = (gpio_pulldown_t)0;
    //disable pull-up mode
    io_conf.pull_up_en = (gpio_pullup_t)0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
 
	while(1) 
	{
        /*--- Blinky period set to 100 ms before calibration 2s after ---*/
        if(BInit && iDelay != 2000)
            iDelay = 2000;

        /* Blink off (output low) */
        gpio_set_level((gpio_num_t)BLINK_GPIO, 0);
		
        vTaskDelay(pdMS_TO_TICKS(iDelay));
		
        /* Blink on (output high) */
        gpio_set_level((gpio_num_t)BLINK_GPIO, 1);
		
        vTaskDelay(pdMS_TO_TICKS(iDelay));

    } /* End while */

} /* end app_main() */
