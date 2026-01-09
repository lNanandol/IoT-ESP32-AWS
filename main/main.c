/*
	Application entry point
*/

#include "esp_err.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "tasks_common.h"
#include "wifi_app.h"
#include "DHT11.h"
#include "wifi_reset_button.h"
#include "sntp_time_sync.h"

static const char TAG[] = "main";

int aws_iot_demo_main(int argc, char ** argv);

void wifi_application_connected_events(void)
{
	ESP_LOGI(TAG, "WiFi application connected");
	sntp_time_sync_task_start();
    aws_iot_demo_main(0, NULL);
}

void app_main(void)
{
	// Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	
	// Start WiFi
	wifi_app_start();

	// Configure wifi reset button
	wifi_reset_button_config();

	// DHT22 Start
	DHT11_task_start();

	// Set connceted event callback 
	wifi_app_set_callback(&wifi_application_connected_events);
}
