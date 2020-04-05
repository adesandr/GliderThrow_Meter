/**
 * 
 *  @file       Esp_mad.h
 *  @author     Alain Désandré - alain.desandre@wanadoo.fr
 *  @version    1.0
 *  @date       28 octobre 2018
 *  @brief      Various define used by esp-mad project
 *  @details    This file is shared by Esp_mad_server and Esp_mad_clients
 * 
 *              Version History :
 *                  - 23/05/2019 - V1.0.1 - new gpio management
 */
 
#ifndef  _ESP_MAD_H_
#define _ESP_MAD_H_

#define PI  3.14159265359

#define LOLIN_32                    /* Update to LOLIN_32_LITE or LOLIN_32 depending your ESP32 dev. board */

#ifdef X_ESP32
    #define PIN_SDA 15
    #define PIN_CLK 14
    #define BLINK_GPIO  GPIO_NUM_13 /* If you prototype with this board, connect a LED on this pin because this board doesn't embark a led in its circuit */
#endif

#ifdef LOLIN_32_LITE
    #define PIN_SDA 23
    #define PIN_CLK 19
    #define BLINK_GPIO  GPIO_NUM_22
#endif

#ifdef LOLIN_32
    #define PIN_SDA 21
    #define PIN_CLK 22
    #define BLINK_GPIO  GPIO_NUM_5
#endif

#define AP_WIFI_SSID        "ESP_MAD" 

#endif /* _ESP_MAD_H_ */
 