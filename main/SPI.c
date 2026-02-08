/*
 * SPI.c
 *
 * Created on: Sat Feb 07 2026
 * Author: Fernando Garcia
 */

#include "SPI.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "DHT11.h" 
#include "tasks_common.h"
#include <string.h>

static const char *TAG = "SPI_APP";

/* Private Variables */
static QueueHandle_t xSPI_App_QueueHandle = NULL;
static dht11_data_t  g_tx_data; // Buffer to send to STM32
static mq135_data_t  g_rx_data; // Buffer to receive from STM32

/**
 * @brief SPI Slave Task
 * This task stays blocked until a message is received or the Master starts a clock.
 */
static void spi_app_task(void *pvParameters)
{
    spi_app_queue_message_t msg;
    esp_err_t ret;

    // Bus Configuration
    spi_bus_config_t buscfg = {
        .mosi_io_num = SPI_MOSI_PIN,
        .miso_io_num = SPI_MISO_PIN,
        .sclk_io_num = SPI_SCLK_PIN,
        .quadwp_io_num = SPI_BUS_WP,
        .quadhd_io_num = SPI_BUS_HD,
    };

    // Slave Interface Configuration
    spi_slave_interface_config_t slvcfg = {
        .mode = SPI_MODE,
        .spics_io_num = SPI_CS_PIN,
        .queue_size = SPI_QUEUE_SIZE,
        .flags = SPI_FLAGS,
    };

    // Initialize SPI Slave
    ret = spi_slave_initialize(VSPI_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "SPI Slave initialized on VSPI (CLK:18, MISO:19, MOSI:23, CS:5)");

    // Initialize transaction structure
    spi_slave_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));

    for (;;)
    {
        // Wait for control messages (from spi_app_send_message)
        if (xQueueReceive(xSPI_App_QueueHandle, &msg, portMAX_DELAY))
        {
            switch (msg.msgID)
            {
                case SPI_APP_MSG_START:
                    ESP_LOGI(TAG, "SPI Application sequence starting...");
                    break;

                case SPI_APP_MSG_TRANSMIT_RECEIVE:
                    // Update TX buffer with latest sensors values
                    g_tx_data.temperature = getTemperature(); 
                    g_tx_data.humidity = getHumidity();

                    // Prepare the hardware transaction (Size in BITS)
                    // We sync based on the largest struct (dht11_data_t = 8 bytes)
                    transaction.length = sizeof(dht11_data_t) * 8;
                    transaction.tx_buffer = &g_tx_data;
                    transaction.rx_buffer = &g_rx_data;

                    /* This call blocks the task until the STM32 (Master) 
                       pulls CS LOW and generates the clock pulses.
                    */
                    ret = spi_slave_transmit(VSPI_HOST, &transaction, portMAX_DELAY);

                    if (ret == ESP_OK)
                    {
                        spi_app_send_message(SPI_APP_MSG_TX_COMPLETE);
                    }
                    else
                    {
                        spi_app_send_message(SPI_APP_MSG_ERROR);
                    }
                    break;

                case SPI_APP_MSG_TX_COMPLETE:
                    ESP_LOGI(TAG, "Sync OK! Received MQ135: %u", g_rx_data.mq135_value);
                    
                    // Ready for the next sync cycle from Master
                    spi_app_send_message(SPI_APP_MSG_TRANSMIT_RECEIVE);
                    break;

                case SPI_APP_MSG_ERROR:
                    ESP_LOGE(TAG, "SPI Slave transaction failed");
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    spi_app_send_message(SPI_APP_MSG_TRANSMIT_RECEIVE);
                    break;

                default:
                    break;
            }
        }
    }
}

void spi_app_start(void)
{
    // Create the control queue
    xSPI_App_QueueHandle = xQueueCreate(10, sizeof(spi_app_queue_message_t));
    
    // Start the SPI task on Core 1 (leaving Core 0 for WiFi/System)
    xTaskCreatePinnedToCore(
        &spi_app_task, 
        "spi_app_task", 
        SPI_APP_TASK_STACK_SIZE, 
        NULL, 
        SPI_APP_TASK_PRIORITY, 
        NULL, 
        SPI_APP_TASK_CORE_ID
    );

    // Initial message to kickstart the state machine
    spi_app_send_message(SPI_APP_MSG_START);
    spi_app_send_message(SPI_APP_MSG_TRANSMIT_RECEIVE);
}

BaseType_t spi_app_send_message(spi_app_message_e msgID)
{
    spi_app_queue_message_t msg;
    msg.msgID = msgID;
    if (xSPI_App_QueueHandle != NULL)
    {
        return xQueueSend(xSPI_App_QueueHandle, &msg, portMAX_DELAY);
    }
    return pdFAIL;
}

uint16_t spi_app_get_mq135_value(void)
{
    return g_rx_data.mq135_value;
}
