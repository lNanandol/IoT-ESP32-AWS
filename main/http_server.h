/*
 * http_server.h
 *
 *  Created on: Dec 29, 2025
 *      Author: Fernando Garcia
 */

#ifndef MAIN_HTTP_SERVER_H_
#define MAIN_HTTP_SERVER_H_

#include "freertos/FreeRTOS.h"

#define OTA_UPDATE_PENDING 		0
#define OTA_UPDATE_SUCCESSFUL 	1
#define OTA_UPDATE_FAILED	   -1

/**
 * @brief Connection status for WiFi
 */
typedef enum http_server_wifi_connect_status
{
	NONE = 0,
	HTTP_WIFI_STATUS_CONNECTING,
	HTTP_WIFI_STATUS_CONNECT_FAIL,
	HTTP_WIFI_STATUS_CONNECT_SUCCESS,
	HTTP_WIFI_STATUS_DISCONNECTED,
} http_server_wifi_connect_status_e;

/*
	Messages for the HTTP Monitor
*/
typedef enum http_server_message
{
	HTTP_MSG_WIFI_CONNECT_INIT = 0,
	HTTP_MSG_WIFI_CONNECT_SUCCESS, 	
	HTTP_MSG_WIFI_CONNECT_FAIL,
	HTTP_MSG_OTA_UPDATE_SUCCESSFUL,
	HTTP_MSG_OTA_UPDATE_FAILED,
	HTTP_MSG_WIFI_USER_DISCONNECT,
	HTTP_MSG_TIME_SERVICE_INITIALIZED,
} http_server_message_e;

/*
	Structure for the message queue
*/
typedef struct http_server_queue_message
{
	http_server_message_e msgID;
} http_server_queue_message_t;

/**
 * Sends a message to the queue
 * @param msgID message ID from the http_server_message_e enum.
 * @return pdTRUE if an item was successfuly sent to the queue, otherwise pdFASLE
 * @note Expand the param list based on your requirements
 */
BaseType_t http_server_monitor_send_message (http_server_message_e msgID);

/**
 * Start the HTTP Server
 */
void http_server_start(void);

/**
 * Stops the HTTP Server  
 */
void http_server_stop(void);

/**
 * @brief Timer callback function which call esp_restart upon sucessful firmware update
 */
void http_server_fw_update_reset_callback(void *arg);

#endif /* MAIN_HTTP_SERVER_H_ */
