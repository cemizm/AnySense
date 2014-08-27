/*
 * config_priv.h
 *
 *  Created on: 25.08.2014
 *      Author: cem
 */

#ifndef CONFIG_PRIV_H_
#define CONFIG_PRIV_H_

#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "crc.h"
#include "timer.h"
#include "hardware.h"

#include "stm32f0xx_flash.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_gpio.h"



#define CONFIG_BASEADRESS 		0x0801F800
#define CONFIG_LENGTH 	 		2048

#define RETRY_TIMEOUT			5

enum config_state_enum {
	config_state_waiting_ack, config_waiting_cmd
};

enum config_state_enum state = config_state_waiting_ack;

uint8_t retry = 0;

static void checkConnection();
int32_t config_load();
int32_t config_save();

DelayedCallbackInfo* info;

static volatile uint8_t running;

#endif /* CONFIG_PRIV_H_ */
