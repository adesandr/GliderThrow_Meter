/**
 * 
 *  \file       Esp_mad.h
 *  \author     Alain Désandré - alain.desandre@wanadoo.fr
 *  \version    1.0
 *  \date       28 octobre 2018
 *  \brief      Various define used by esp-mad project
 *  \details    This file is shared by Esp_mad_server and Esp_mad_clients
 * 
 */
 
#ifndef  _ESP_MAD_H_
#define _ESP_MAD_H_

#define PI  3.14159265359

#define X_ESP32                 /* Update to LOLIN_32_LITE or LOLIN_32 depending your ESP32 dev. board */

#ifdef X_ESP32
    #define PIN_SDA 15
    #define PIN_CLK 14
    #define BLINK_GPIO  13
#endif

#ifdef LOLIN_32_LITE
    #define PIN_SDA 23
    #define PIN_CLK 19
    #define BLINK_GPIO  22
#endif

#ifdef LOLIN_32
    #define PIN_SDA 22
    #define PIN_CLK 21
    #define BLINK_GPIO  5
#endif

#define AP_WIFI_SSID        "ESP_MAD" 

#endif /* _ESP_MAD_H_ */
 