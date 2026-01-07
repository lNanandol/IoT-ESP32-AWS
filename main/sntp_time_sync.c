/*
 * sntp_time_sync.c
 *
 *  Created on: Tue Jan 06 2026
 *      Author: Fernando Garcia
 */

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/apps/sntp.h"

#include "tasks_common.h"
#include "http_server.h"
#include "sntp_time_sync.h"
#include "wifi_app.h"

static const char TAG[] = "sntp_time_sync";

// SNTP operate mode set status
static bool sntp_op_mode_set = false;

/**
 * @brief Initialize SNTP service using SNTP_OPMODE_POLL server
 */
static void sntp_time_sync_init_sntp(void)
{
    ESP_LOGI(TAG, "Time is not set yet. Initializing SNTP");
    
    if(!sntp_op_mode_set)
    {
        // Set the operating mode
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_op_mode_set = true;
    }

    sntp_setservername(0, "pool.ntp.org");
    
    // Initialize SNTP service
    sntp_init();

    // Let the http_server know service is initialized 
    http_server_monitor_send_message(HTTP_MSG_TIME_SERVICE_INITIALIZED);
}

/**
 * @brief Gets the current time, if the current time is not up to date,
 * the sntp_time_sync_init_sntp function is called to update it.
 */
static void sntp_time_sync_obtain_time(void)
{
    time_t now = 0;
    struct tm time_info = { 0 };

    time(&now);
    localtime_r(&now, &time_info);

    // Check the time in case we need to initialize SNTP
    if(time_info.tm_year < (2020 - 1900))
    {
        sntp_time_sync_init_sntp();

        //Set the local time zone 
        setenv("TZ", "GMT-6", 1);
        tzset();
    }
}

/**
 * @brief SNTP Time Synchronization Task
 * @param arg pvParam 
 */
static void sntp_time_sync_task(void *pvParam)
{
    while(1)
    {
        sntp_time_sync_obtain_time();
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

char* sntp_time_sync_get_time(void)
{
    static char time_buffer[100] = {0};
    time_t now = 0;
    struct tm time_info = { 0 };

    time(&now);
    localtime_r(&now, &time_info);

    if(time_info.tm_year < (2020 - 1900))
    {
        ESP_LOGI(TAG, "Time is not set yet. Returning empty string");
    }
    else
    {
        strftime(time_buffer, sizeof(time_buffer), "%d.%m.%Y %H:%M:%S", &time_info);
        ESP_LOGI(TAG, "Current time: %s", time_buffer);
    }
    return time_buffer;
}

void sntp_time_sync_task_start(void)
{
    xTaskCreatePinnedToCore(
        &sntp_time_sync_task,
        "sntp_time_sync_task",
        SNTP_TIME_SYNC_TASK_STACK_SIZE,
        NULL,
        SNTP_TIME_SYNC_TASK_PRIORITY,
        NULL,
        SNTP_TIME_SYNC_TASK_CORE_ID
    );
}