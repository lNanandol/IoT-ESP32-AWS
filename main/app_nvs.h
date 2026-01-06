/*
 * app_nvs.h
 *
 *  Created on: Mon Jan 05 2026
 *      Author: Fernando Garcia
 */
 
#ifndef MAIN_APP_NVS_H_
#define MAIN_APP_NVS_H_

#include <stdbool.h>
#include "esp_err.h"

/**
 * @brief Saves station mode WiFi credentials to NVS.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t app_nvs_save_sta_creds(void);

/**
 * @brief Loads the previously savedcredentials from NVS.
 * @return true if credentials were loaded successfully, false otherwise.
 */
bool app_nvs_load_sta_creds(void);

/**
 * @brief Clears station mode credentials from NVS.
 * @return ESP_OK on success, or an error code on failure. 
 */
esp_err_t app_nvs_clear_sta_creds(void);

#endif /* MAIN_APP_NVS_H_ */
