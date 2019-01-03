/**
 * \file    esp_mad_client.cpp
 * \author  Alain Désandré
 * \version 1.0
 * \date    28 octobre 2018
 * \brief   esp_mad project main files.
 *
 * \details The MAD application is intended to provide a ESP32-based tool
 *          for visualizing the deflection (in mm and degree) of control surface
 *          on a remote-controlled model.
 *          ESP_MAD is composed of four files :
 *              . esp_mad.ccp : main function.
 *              . esp_mad_task_measure.cpp : MPU6050 management
 *              . esp_mad_task_http_server.c : http server
 *              . esp_mad.h : Globals define
 *              . esp_html.h : HTML page and Jquery/Javascript function.
 */

#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"
#include "freertos/task.h"
#include <esp_log.h>
#include <esp_err.h>

#define BLINK_GPIO 22

extern "C" {
	void app_main(void);
    void task_http_client(void*);
}

extern bool BInit;

extern void task_measure(void*);
extern void task_http_client(void*);

/**
 *	\fn 	    app_main(void)
 *	\brief 		esp_mad project entry point function.
 *	\param[in]	void*
 *	\return		void.
 */
void app_main(void)
{
    /*--- Blinky period set to 100 ms during init and 2s after ---*/
	int iDelay = 100; 

    /*--- two tasks are launched. One task handle the MPU6050 measurement and   ---*/
    /*--- the other one is a pretty simple http server to deal with the browser ---*/
    /*--- requests. Processing MPU6050 has highest priority                     ---*/
    xTaskCreate(&task_measure, "measure_task", 8192, NULL, 5, NULL);
    
	vTaskDelay(500/portTICK_PERIOD_MS);
    
	xTaskCreate(&task_http_client, "http_client_task", 8192, NULL, 5, NULL);

	/*---  Configure the IOMUX register for pad BLINK_GPIO (some pads are ---*/
    /*---  muxed to GPIO on reset already, but some default to other      ---*/
    /*---  functions and need to be switched to GPIO. Consult the         ---*/
    /*---  Technical Reference for a list of pads and their default       ---*/
    /*---  functions.). Blinky led.                                                    ---*/
    gpio_pad_select_gpio(BLINK_GPIO);

	/* Set the GPIO as a push/pull output */
    gpio_set_direction((gpio_num_t)BLINK_GPIO, GPIO_MODE_OUTPUT);

	while(1) 
	{

        if (BInit && iDelay != 2000)
            iDelay = 2000;

        /* Blink off (output low) */
        gpio_set_level((gpio_num_t)BLINK_GPIO, 0);
		
        vTaskDelay(pdMS_TO_TICKS(iDelay));
		
        /* Blink on (output high) */
        gpio_set_level((gpio_num_t)BLINK_GPIO, 1);
		
        vTaskDelay(pdMS_TO_TICKS(iDelay));

    } /* End while */

} /* end app_main() */
