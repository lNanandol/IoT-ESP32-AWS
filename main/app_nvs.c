/*
 * app_nvs.c
 *
 *  Created on: Mon Jan 05 2026
 *      Author: Fernando Garcia
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_wifi_types_generic.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "app_nvs.h"
#include "wifi_app.h"

// Tag for logging to the monitor
static const char TAG[] = "nvs";

// NVS namespace used for stations mode credentials
const char app_nvs_sta_creds_namespace[] = "sta_creds";

esp_err_t app_nvs_save_sta_creds(void)
{   
    nvs_handle handle;
    esp_err_t esp_err;
    ESP_LOGI(TAG, "app_nvs_save_sta_creds: Saving station mode credentials to flash");

    wifi_config_t *wifi_sta_config = wifi_app_get_wifi_config();

    if(wifi_sta_config)
    {
        esp_err = nvs_open(app_nvs_sta_creds_namespace, NVS_READWRITE, &handle);
        if(esp_err != ESP_OK)
        {
            printf("app_nvs_save_sta_creds: Error (%s) openning NVS handle!\n", esp_err_to_name(esp_err));
            return esp_err;
        }

        // Set SSID
        esp_err = nvs_set_blob(handle, "ssid", wifi_sta_config->sta.ssid, MAX_SSID_LENGTH);
        if(esp_err != ESP_OK)
        {
            printf("app_nvs_save_sta_creds: Error (%s) setting SSID to NVS!\n", esp_err_to_name(esp_err));
            return esp_err;
        }

        // Set Password
        esp_err = nvs_set_blob(handle, "password", wifi_sta_config->sta.password, MAX_SSID_LENGTH);
        if(esp_err != ESP_OK)
        {
            printf("app_nvs_save_sta_creds: Error (%s) setting PASSWORD to NVS!\n", esp_err_to_name(esp_err));
            return esp_err;
        }

        // Commit credentials to NVS
        esp_err = nvs_commit(handle);
        if(esp_err != ESP_OK)
        {
            printf("app_nvs_save_sta_creds: Error (%s) commiting credentials to NVS!\n", esp_err_to_name(esp_err));
            return esp_err;
        }
        nvs_close(handle);
        ESP_LOGI(TAG, "app_nvs_save_sta_creds: wrote wifi_sta_config: Station SSID: %s Password: %s", wifi_sta_config->sta.ssid, wifi_sta_config->sta.password);
    }

    printf("app_nvs_save_sta_creds: returned ESP_OK\n");
    return ESP_OK;

}

bool app_nvs_load_sta_creds(void)
{
    nvs_handle handle;
    esp_err_t esp_err;

    ESP_LOGI(TAG, "app_nvs_load_sta_creds: Loading credentials from flash");

    if(nvs_open(app_nvs_sta_creds_namespace, NVS_READONLY, &handle) == ESP_OK)
    {
        wifi_config_t *wifi_sta_config = wifi_app_get_wifi_config();

        if(wifi_sta_config == NULL)
        {
            wifi_sta_config = (wifi_config_t *)malloc(sizeof(wifi_config_t));
        }
        memset(wifi_sta_config, 0x00, sizeof(wifi_config_t));

        // Allocate buffer
        size_t wifi_config_size = sizeof(wifi_config_t);
        uint8_t *wifi_config_buff = (uint8_t*)malloc(sizeof(uint8_t) * wifi_config_size);
        memset(wifi_config_buff, 0x00, sizeof(wifi_config_size));

        // Load SSID
        wifi_config_size = sizeof(wifi_sta_config->sta.ssid);
        esp_err = nvs_get_blob(handle, "ssid", wifi_config_buff, &wifi_config_size);
        if(esp_err != ESP_OK)
        {
            free(wifi_config_buff);
            printf("app_nvs_load_sta_creds: (%s) no station SSID found in NVS!\n", esp_err_to_name(esp_err));
            return false;
        }
        memcpy(wifi_sta_config->sta.ssid, wifi_config_buff, wifi_config_size);

        // Load Passw0
        wifi_config_size = sizeof(wifi_sta_config->sta.password);
        esp_err = nvs_get_blob(handle, "password", wifi_config_buff, &wifi_config_size);
        if(esp_err != ESP_OK)
        {
            free(wifi_config_buff);
            printf("app_nvs_load_sta_creds: (%s) retreiving passrword!\n", esp_err_to_name(esp_err));
            return false;
        }
        memcpy(wifi_sta_config->sta.password, wifi_config_buff, wifi_config_size);

        free(wifi_config_buff);
        nvs_close(handle);

        printf("app_nvs_load_sta_creds: SSID: %s Password: %s\n", wifi_sta_config->sta.ssid, wifi_sta_config->sta.password);
        return wifi_sta_config->sta.ssid[0] != '\0';

    }
    else
    {
        return false;
    }

    return ESP_OK;
}

esp_err_t app_nvs_clear_sta_creds(void)
{
    nvs_handle handle;
    esp_err_t esp_err;
    ESP_LOGI(TAG, "app_nvs_clear_sta_creds: Clearing station mode credentials from flash");

    esp_err = nvs_open(app_nvs_sta_creds_namespace, NVS_READWRITE, &handle);
    if(esp_err != ESP_OK)
    {
        printf("app_nvs_clear_sta_creds: Error (%s) openning NVS handle!\n", esp_err_to_name(esp_err));
        return esp_err;
    }

    // Erase credentials
    esp_err = nvs_erase_all(handle);
    if(esp_err != ESP_OK)
    {
        printf("app_nvs_clear_sta_creds: Error (%s) erasing station mode credentials!\n", esp_err_to_name(esp_err));
        return esp_err;
    }

    // Commit clearing credentials from NVS
    esp_err = nvs_commit(handle);
    if(esp_err != ESP_OK)
    {
        printf("app_nvs_clear_sta_creds: Error (%s) NVS commit!\n", esp_err_to_name(esp_err));
        return esp_err;
    }
    nvs_close(handle);

    printf("app_nvs_clear_sta_creds: returned ESP_OK\n");
    return ESP_OK;
}