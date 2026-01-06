/*
 * wifi_reset_button.h
 *
 *  Created on: Tue Jan 06 2026
 *      Author: Fernando Garcia
 */

#ifndef MAIN_WIFI_RESET_BUTTON_H_
#define MAIN_WIFI_RESET_BUTTON_H_

// Default interact flag
#define ESP_INTR_FLAG_DEFAULT   0

//Wifi reset button GPIO
#define WIFI_RESET_BUTTON_GPIO  0

/**
 * @brief Configures Wifi reset button and interrupt configuration 
 */
void wifi_reset_button_config(void);

#endif