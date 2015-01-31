/*
 * module_futaba.h
 *
 *  Created on: 03.01.2015
 *      Author: cem
 */

#ifndef MODULE_FUTABA_H_
#define MODULE_FUTABA_H_


#include "hardware.h"
#include "stm32f0xx_gpio.h"

void module_futaba_initializeConfig(void* config);
void module_futaba_start(const struct hardware_port_cfg* port, uint8_t* config);

#endif /* MODULE_FUTABA_H_ */
