/**
 * @file    esp_mad_client.cpp
 * @author  Alain Désandré - alain.desandre@wanadoo.fr
 * @date    october 28 2018
 * @brief   esp_mad project main files.
 *
 * @details The MAD application is intended to provide a ESP32-based tool
 *          for visualizing the deflection (in mm and degree) of control surface
 *          on a remote-controlled model.
 *          ESP_MAD is composed of five files :
 *              - esp_mad_client.ccp : main function.
 *              - esp_mad_task_measure.cpp : MPU6050 management
 *              - esp_mad_task_http_client.c : http client
 *              - Esp_mad_Globals_Variables.h : Globals variables declaration & Init.
 *              - Esp_mad.h : Globals define
 */

/*-----------------------------------------
 *-            INCLUDES        
 *-----------------------------------------*/
#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"
#include "freertos/task.h"
#include <esp_log.h>
#include <esp_err.h>
#include "driver/gpio.h"
#include <Esp_mad.h>

/*-----------------------------------------
 *- GLOBALS VARIABLES DECLARATION & INIT        
 *-----------------------------------------*/
#define DEFINE_VARIABLES
#include <Esp_mad_Globals_Variables.h>

/*-----------------------------------------
 *-            EXTERNALS        
 *-----------------------------------------*/
extern "C" {
	void app_main(void);
    void task_http_client(void*);
    void task_vBattery(void*);
}

extern void task_measure(void*);
extern void task_http_client(void*);

/**
 *	@fn 	    app_main(void)
 *	@brief 		esp_mad project entry point function.
 *	@param[in]	void
 *	@return		void.
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

    vTaskDelay(500/portTICK_PERIOD_MS);
    
	xTaskCreate(&task_vBattery, "vBattery_task", 8192, NULL, 5, NULL);

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
