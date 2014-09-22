/*
 * module_hott.h
 *
 *  Created on: 12.09.2014
 *      Author: cem
 */

#ifndef MODULE_HOTT_H_
#define MODULE_HOTT_H_

#include "hardware.h"
#include "stm32f0xx_gpio.h"

void hott_initializeConfig(void* config);
void hott_start(const struct hardware_port_cfg* port, uint8_t* config);

#endif /* MODULE_HOTT_H_ */
