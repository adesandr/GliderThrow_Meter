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
#include <esp_log.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_mad_task_vBattery.h"
#include <Esp_mad.h>
#include <Esp_mad_Globals_Variables.h>

/*-----------------------------------------
 *-            LOCALS VARIABLES        
 *-----------------------------------------*/
static esp_adc_cal_characteristics_t adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_7;					/* VBAT Sense on ESP_MAD board is connected to IO35, so channel 7 */
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;				/* Maximum resolution : 12 bits									  */
static const adc_atten_t atten = ADC_ATTEN_DB_11;					/* 11 dB attenuation as we use a 4,2 V max battery lipo, divided  */
																	/* per two as we used a bridge resistor divider.				  */
static const adc_unit_t unit = ADC_UNIT_1;							/* ADC1 is used													  */
static uint32_t adc_reading = 0;

static const char tagd[] = "task_vBattery->";

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
        ESP_LOGI(tagd, "Characterized using Two Point Value");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        ESP_LOGI(tagd, "Characterized using eFuse Vref");
    } else {
        ESP_LOGI(tagd, "Characterized using Default Vref");
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

	/*--- ADC Initialization ---*/
	ESP_LOGI(tagd,"ADC initialization ...");
	adc1_config_width(width);
    adc1_config_channel_atten((adc1_channel_t)channel, atten);

	/*--- ADC Calibration ---*/
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, &adc_chars);
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
        voltage = (esp_adc_cal_raw_to_voltage(adc_reading, &adc_chars))*2;
        ESP_LOGI(tagd, "Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);

		/*--- In order to reduce the power consumption, voltage is read in low frequency						---*/
		vTaskDelay(30000/portTICK_PERIOD_MS);
		
		}

    /*--- this part will not be executed but we stick the free rtos recomandation ---*/
	vTaskDelete(NULL);

} /* end task_measure() */
