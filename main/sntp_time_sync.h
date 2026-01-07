/*
 * sntp_time_sync.h
 *
 *  Created on: Tue Jan 06 2026
 *      Author: Fernando Garcia
 */

#ifndef MAIN_SNTP_TIME_SYNC_H_
#define MAIN_SNTP_TIME_SYNC_H_

/**
 * @brief Starts the NTP Server time synchronization task
 */
void sntp_time_sync_task_start(void);

/**
 * @brief Returns local time if set
 * @return local time buffer
 */
char* sntp_time_sync_get_time(void);

#endif /* MAIN_SNTP_TIME_SYNC_H_ */