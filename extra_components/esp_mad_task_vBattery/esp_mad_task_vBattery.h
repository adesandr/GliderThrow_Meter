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

	/*------------------------------------------
	 * DEFINE
	 *------------------------------------------*/
	#define TEMPO_READ_BATT_VOLTAGE	30000       	// Battery Voltage is read each 30s 	
	#define NB_ADC_SAMPLE   		64          	// Samples number used to read the ADC. Recommanded in the API Guide
	#define ADC_UNIT				ADC_UNIT_1		// Fisrt ADC unit is used	
	#define ADC_CHANNEL				ADC_CHANNEL_7	// VBAT Sense on ESP_MAD board is connected to IO35, so channel 7 
	#define VREF					1000			// 1100 mv Theorical Vref
	#define K_FACTOR				0.30			// Attenuation factor, empiric rule with a multimeter
	#define ADC_DMAX				4095			// 4095 unit for 12 bits resolution

#endif