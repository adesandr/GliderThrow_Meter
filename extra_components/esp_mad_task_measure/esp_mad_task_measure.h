/**
 * @file      esp_mad_task_measure.h
 * @author    Alain Désandré - alain.desandre@wanadoo.fr
 * @date      December 24th 2020
 * @brief     interface for the esp_mad_task_measure component.
 *
 * @details   Protypes and define for the component.
 * 
 */

#ifndef _ESP_MAD_TASK_MEASURE_H_

#define _ESP_MAD_TASK_MEASURE_H_

	/*------------------------------------------
	 * PROTYPES
	 *------------------------------------------*/
	void meansensors(void);
	void calibration(void);
	void InitMPU6050(void);
	void task_measure(void*);

#endif