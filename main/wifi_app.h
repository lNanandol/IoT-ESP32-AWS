/*
 * wifi_app.h
 *
 *  Created on: Dec 19, 2025
 *      Author: Fernando Garcia
 */

#ifndef MAIN_WIFI_APP_H_
#define MAIN_WIFI_APP_H_

#include "esp_netif.h"
#include "portmacro.h"

#include "esp_wifi_types_generic.h"

// Callback typedef
typedef void (*wifi_connected_event_callback_t)(void);

//Wifi applications settings
#define WIFI_AP_SSID				"ESP32_AP"			// AP name
#define WIFI_AP_PASSWORD			"ESP32_PASSWORD"	// WiFi access point password
#define WIFI_AP_CHANNEL 			1					// Access point channel 
#define WIFI_AP_SSID_HIDDEN			0					// AP visibility
#define WIFI_AP_MAX_CONNECTIONS		5					// AP mac clients
#define WIFI_AP_BEACON_INTERVAL		100					// AP beacon: 100 milliseconds recommended
#define WIFI_AP_IP					"192.168.0.1"		// AP default IP
#define WIFI_AP_GATEWAY				"192.168.0.1"		// AP default gateway
#define WIFI_AP_NETMASK				"255.255.255.0"		// AP default netmask
#define WIFI_AP_BANDWIDTH			WIFI_BW_HT20		// AP bandwidth 20 MHz (40 MHz is the other option)
#define WIFI_STA_POWER_SAVE			WIFI_PS_NONE		// Power save is not used
#define MAX_SSID_LENGTH				32					// IEEE standard maximum 
#define MAX_PASSWORD_LENGHT			64					// IEEE standard maximum 
#define MAX_CONNECTION_RETRIES		5					// Retry number of disconnect 

// netif object for the Station and Access Point
extern esp_netif_t* esp_netif_sta;
extern esp_netif_t* esp_netif_ap;

/*
	Message IDs for the wifi aplication task
	@note Expand this base in your applications requirements
*/
typedef enum wifi_app_message
{
	WIFI_APP_MSG_START_HTTP_SERVER = 0,
	WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER, 
	WIFI_APP_MSG_STA_CONNECTED_GOT_IP,
	WIFI_APP_MSG_STA_DISCONNECTED,
	WIFI_APP_MSG_USER_REQUESTED_STA_DISCONNECT,
	WIFI_APP_MSG_LOAD_SAVED_CREDENTIALS,
} wifi_app_message_e;

/*
	Structure for the message queue
	@note Expand this base in your applications requirements
*/
typedef struct wifi_app_queue_message
{
	wifi_app_message_e msgID;
} wifi_app_queue_message_t;

/*
	Sends a message to the queue 
	@note msgID message ID from the wifi_app_message_e enum.
	@return pdTRUE if an item was successfully sent to the queue, otherwise pdFALSE 
	@note Expand this base in your applications requirements
*/
BaseType_t wifi_app_send_message(wifi_app_message_e msgID);

/*
	Starts the WiFi RTOS task
*/
void wifi_app_start(void);

/**
 * @brief Gets the WiFi configuration
 */
wifi_config_t* wifi_app_get_wifi_config(void);

/**
 * @brief Sets the callback function
 */
void wifi_app_set_callback(wifi_connected_event_callback_t callback);

/**
 * @brief Calls the callback funtion
 */
void wifi_app_call_callback(void);

#endif /* MAIN_WIFI_APP_H_ */


























