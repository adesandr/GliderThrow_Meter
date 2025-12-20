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
#include "driver/adc.h"
//#include "esp_adc/adc_oneshot.h"
#include "esp_adc_cal.h"
//#include "esp_adc/adc_cali.h"
//#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"
#include "esp_adc_cal_types_legacy.h"
#include "esp_mad_task_vBattery.h"
#include "Esp_mad.h"
#include "Esp_mad_Globals_Variables.h"

/*-----------------------------------------
 *-            LOCALS VARIABLES        
 *-----------------------------------------*/
                                                                         
static const adc_channel_t channel = ADC_CHANNEL_7;			/* VBAT Sense on ESP_MAD board is connected to IO35, so channel 7                                 */
static const adc_atten_t atten = ADC_ATTEN_DB_12;		    /* 11 dB attenuation as we use a 4,2 V max battery lipo, divided by 2 because bridge divisor used */   															
static const adc_unit_t unit = ADC_UNIT_1;				    /* ADC1 is used													                                  */
static uint32_t adc_reading = 0;

static const char tagd[] = "task_vBattery->";

/**
 *	@fn 		static void check_efuse(void)
 *  @brief		Check if TP and Vref is burned into eFuse	
 *	@param[in]	void
 *	@return		void	
 * 
 */
static void check_efuse(void)
{
    //Check TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }

    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
}

/**
 *	@fn 		static void print_char_val_type(esp_adc_cal_value_t)
 *  @brief		Print the caracterization method used for calibration	
 *	@param[in]	esp_adc_cal_value_t
 *	@return		void	
 * 
 */
static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        ESP_LOGI(tagd, "Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        ESP_LOGI(tagd, "Characterized using eFuse Vref\n");
    } else {
        ESP_LOGI(tagd, "Characterized using Default Vref\n");
    }
}

/**
 *	@fn 		void task_vBattery(void*)
 *  @brief		Periodic Battery's voltage measurement	
 *	@param[in]	void*
 *	@return		void	
 * 
 */
void task_vBattery(void* ignore) {
    
    /*--- Check if Two Point or Vref are burned into eFuse ---*/
    check_efuse();

    /*--- ADC Initialization with 12 bit resolution ---*/
	ESP_LOGI(tagd,"ADC initialization ...");
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, atten);

    /*--- Characterize ADC ---*/
    esp_adc_cal_characteristics_t *adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));  
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);

	/*--- Infinite loop ---*/
	while(1){
	    
		/*--- ADC Reading ---*/
		adc_reading = 0;
        for (int i = 0; i < NB_ADC_SAMPLE; i++) {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);
        }
        adc_reading /= NB_ADC_SAMPLE;
        
		/*--- ADC convertion to voltage in mV 																	---*/
		/*--- note : voltage is multiply by 2 because we have a brigde resistor divider of 100kOhm each on IO35 ---*/
		/*--- see the ESP_MAD_BOARD schematic for more information												---*/
        g_voltage = (esp_adc_cal_raw_to_voltage(adc_reading, adc_chars))*2;
        ESP_LOGI(tagd, "Raw: %d\tVoltage: %dmV\n", (int)adc_reading, (int)g_voltage);

		/*--- In order to reduce the power consumption, voltage is read in low frequency						---*/
		vTaskDelay(30000/portTICK_PERIOD_MS);
		
		}

    /*--- this part will not be executed but we stick the free rtos recomandation ---*/
	vTaskDelete(NULL);

} /* end task_measure() */
