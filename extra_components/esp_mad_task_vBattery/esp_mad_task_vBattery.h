/**
 * @file      esp_mad_task_vBattery.h
 * @author    Alain Désandré - alain.desandre@wanadoo.fr
 * @date      june 5th 2021
 * @brief     interface for the esp_mad_task_vBattery component.
 *
 * @details   Protypes and define for the component.
 * 
 */

#ifndef _ESP_MAD_TASK_VBATTERY_H_

#define _ESP_MAD_TASK_VBATTERY_H_

	/*------------------------------------------
	 * PROTYPES
	 *------------------------------------------*/
	void task_vBattery(void*);
	static void print_char_val_type(esp_adc_cal_value_t);

	/*------------------------------------------
	 * DEFINE
	 *------------------------------------------*/
	#define DEFAULT_VREF    1100        /* As default 1100 mV for ADC calibration as recommended by espressif */
	#define NB_ADC_SAMPLE   64          /* Samples number used to read the ADC */ 

#endif