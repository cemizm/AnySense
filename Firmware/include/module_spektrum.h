/*
 * module_spektrum.h
 *
 *  Created on: 06.08.2015
 *      Author: cem
 */

#ifndef MODULE_SPEKTRUM_H_
#define MODULE_SPEKTRUM_H_

#include "hardware.h"
#include "stm32f0xx_gpio.h"

void module_spektrum_initializeConfig(void* config);
void module_spektrum_start(const struct hardware_port_cfg* port, uint8_t* config);


#endif /* MODULE_SPEKTRUM_H_ */
