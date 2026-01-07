/*
 * wifi_reset_button.c
 *
 *  Created on: Tue Jan 06 2026
 *      Author: Fernando Garcia
 */

#include "esp_rom_gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "tasks_common.h"
#include "wifi_app.h"
#include "wifi_reset_button.h"

static const char TAG[] = "wifi_reset_button";

// Semaphore handle for the button press
SemaphoreHandle_t wifi_reset_semaphore = NULL;

/**
 * @brief ISR handler for the wifi reset BOOT button
 * @param arg parameter which can be passed to the isr handler
 */
void IRAM_ATTR wifi_reset_button_isr_handler(void* arg)
{
    // Notify the button task
    xSemaphoreGiveFromISR(wifi_reset_semaphore, NULL);
}

/**
 * @brief Wifi retest button reacts to a BOOT button by sending a message 
 *        to the wifi application task to clear saved credentials
 * @param pvParam parameter which can be passed to the task
 */
void wifi_reset_button_task(void *pvParam)
{
    for(;;)
    {
        // Wait until the semaphore is given by the ISR
        if(xSemaphoreTake(wifi_reset_semaphore, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI(TAG, "wifi_reset_button_task: Button pressed, clearing saved WiFi credentials");

            // Send message to wifi application to clear saved credentials
            wifi_app_send_message(WIFI_APP_MSG_USER_REQUESTED_STA_DISCONNECT);

            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
} 
 
void wifi_reset_button_config(void)
{
    // Creates the binry semaphore
    wifi_reset_semaphore = xSemaphoreCreateBinary();

    // Configure the button and set the direction
    esp_rom_gpio_pad_select_gpio(WIFI_RESET_BUTTON_GPIO);
    gpio_set_direction(WIFI_RESET_BUTTON_GPIO, GPIO_MODE_INPUT);

    // Enable interrupt on the negative edge 
    gpio_set_intr_type(WIFI_RESET_BUTTON_GPIO, GPIO_INTR_NEGEDGE);

    // Create the Wifi reset button task
    xTaskCreatePinnedToCore(
        &wifi_reset_button_task, 
        "wifi_reset_button_task", 
        WIFI_RESET_BUTTON_TASK_STACK_SIZE, 
        NULL, 
        WIFI_RESET_BUTTON_TASK_PRIORITY, 
        NULL, 
        WIFI_RESET_BUTTON_TASK_CORE_ID
    );

    // Intall GPIO isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    // Attach the interrupt server routine
    gpio_isr_handler_add(WIFI_RESET_BUTTON_GPIO, wifi_reset_button_isr_handler, NULL);
}