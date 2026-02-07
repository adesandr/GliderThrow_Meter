/**
 * @file      esp_mad_task_vBattery.c
 * @author    Alain Désandré - alain.desandre@wanadoo.fr
 * @date      june 5th 2021
 * @brief     This task process the measurement of the voltage of the battery
 *
 * @details   This task handles the periodic measurement of the voltage of the battery
 * 
 */

/*-----------------------------------------
 *-            INCLUDES        
 *-----------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"

#include "esp_mad_task_vBattery.h"
#include "Esp_mad.h"
#include "Esp_mad_Globals_Variables.h"

/*-----------------------------------------
 *-            LOCALS VARIABLES        
 *-----------------------------------------*/
static int adc_reading = 0;		    // Used to temporary read the ADC value                     

static const char tagd[] = "task_vBattery->";

/**
 *	@fn 		void task_vBattery(void*)
 *  @brief		Periodic Battery's voltage measurement	
 *	@param[in]	void*
 *	@return		void	
 * 
 */
void task_vBattery(void* ignore) {

    /*--- ADC1 Init ---*/
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));
    
    /*--- Configure ADC Channel ---*/
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,        // 12-bit resolution (0-4095)
        .atten  = ADC_ATTEN_DB_12,          // ~3,3v full scale voltage
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL, &config));

 	/*--- Infinite loop ---*/
	while(1){
	    
		/*--- ADC Reading ---*/
		adc_reading = 0;
        int sample = 0;
        for (int i = 0; i < NB_ADC_SAMPLE; i++) {
                ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL, &sample));
                adc_reading += sample;
        }
        adc_reading /= NB_ADC_SAMPLE;
        ESP_LOGI(tagd, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT + 1, ADC_CHANNEL, adc_reading);

        /*--- Voltage conversion, see adc oneshot AP Guide for explanation ---*/
        g_voltage = ((adc_reading * (VREF/K_FACTOR)) / ADC_DMAX);
        ESP_LOGI(tagd, "ADC%d Channel[%d] voltage %dmV:", ADC_UNIT + 1, ADC_CHANNEL, g_voltage);
    
		/*--- In order to reduce the power consumption, voltage is read in low frequency						---*/
		vTaskDelay(TEMPO_READ_BATT_VOLTAGE/portTICK_PERIOD_MS);
		
		}
   
    /*--- Tear down ---*/
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));

    /*--- this part will not be executed but we stick the free rtos recomandation ---*/
	vTaskDelete(NULL);

} /* end task_measure() */
