/*
 * SPI.h
 *
 * Created on: Sat Feb 07 2026
 * Author: Fernando Garcia
 */

#ifndef MAIN_SPI_H_
#define MAIN_SPI_H_

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// PIN definitions for SPI (ESP32 DevKit V1 VSPI)
#define SPI_MOSI_PIN    23
#define SPI_MISO_PIN    19
#define SPI_SCLK_PIN    18
#define SPI_CS_PIN      5

// SPI Slave Configuration
#define SPI_MODE        0
#define SPI_QUEUE_SIZE  3
#define SPI_FLAGS       0

// SPI BUS configuration 
#define SPI_BUS_WP      -1
#define SPI_BUS_HD      -1

/**
 * @brief SPI MSG IDs for the SPI application task
 */
typedef enum {
    SPI_APP_MSG_START = 0,
    SPI_APP_MSG_TRANSMIT_RECEIVE,
    SPI_APP_MSG_TX_COMPLETE,
    SPI_APP_MSG_ERROR,
} spi_app_message_e;

/**
 * @brief Structure for SPI application message queue
 */
typedef struct {
    spi_app_message_e msgID;
} spi_app_queue_message_t;

/**
 * @brief Data structure for sensor DHT11 TX to STM32
 */
typedef struct {
    float humidity;
    float temperature;
} dht11_data_t;

/**
 * @brief Data structure for sensor MQ135 RX from STM32
 */
typedef struct {
    uint16_t mq135_value;
    uint16_t status;
} mq135_data_t;

// --- PROTOTYPES ---

/**
 * @brief Starts the SPI application task and initializes the SPI slave interface
 */
void spi_app_start(void);

/**
 * @brief Sends a message to the SPI application task queue
 * @param msgID ID of the message to send, from the spi_app_message_e enum
 * @return BaseType_t pdPASS if the message was successfully sent to the queue, otherwise pdFAIL
 */
BaseType_t spi_app_send_message(spi_app_message_e msgID);

/**
 * @brief Returns the latest MQ135 sensor value received from the STM32
 */
uint16_t spi_app_get_mq135_value(void);

#endif /* MAIN_SPI_H_ */