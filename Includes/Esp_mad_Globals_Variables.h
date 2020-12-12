/**
 * 
 *  @file       Esp_mad_Globals_Variables.h
 *  @author     Alain Désandré - alain.desandre@wanadoo.fr
 *  @version    1.0
 *  @date       12 décembre 2020
 *  @brief      This files define all the Globals Variables used in the project.
 *  @details    This file is shared by Esp_mad_server and Esp_mad_clients.
 * 
 *              Version History :
 *                  - 12/12/2020    :   Creation
 */
 

#ifndef  _ESP_MAD_GLOBALS_VARIABLES_H_
#define _ESP_MAD_GLOBALS_VARIABLES_H_

#ifdef DEFINE_VARIABLES
    #define EXTERN /*nothing */
    #define INITIALIZER(...)   = __VA_ARGS__
#else
    #define EXTERN extern
    #define INITIALIZER(...)    /* nothing */
#endif /* DEFINE_VARIABLES */

EXTERN bool BInit INITIALIZER(0);                /* Boolean used to indicated if the MPU6050 is calibrated or not    */
EXTERN float travel INITIALIZER(0.0);            /* Store the travel measure by the MPU6050                          */
EXTERN float angle INITIALIZER(0.0);             /* Store the angle measure by the MPU6050                           */                           
EXTERN int chordControlSurface INITIALIZER(50);  /* Store the chord of the Control surface in mm. 50 mm by default   */

#endif /* _ESP_MAD_GLOBALS_VARIABLES_H_ */
 