#ifndef DHT11_H_  
#define DHT11_H_

#define DHT_OK 				0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR  -2

#define DHT_GPIO			25

/**
 * Starts DHT22 sensor task
 */
void DHT11_task_start(void);

// == function prototypes =======================================

void 	setDHTgpio(int gpio);
void 	errorHandler(int response);
int 	readDHT();
float 	getHumidity();
float 	getTemperature();
int 	getSignalLevel( int usTimeOut, bool state );

#endif
