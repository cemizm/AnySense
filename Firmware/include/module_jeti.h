/*
 * module_jeti.h
 *
 *  Created on: 09.02.2015
 *      Author: cem
 */

#ifndef MODULE_JETI_H_
#define MODULE_JETI_H_

#include "hardware.h"
#include "stm32f0xx_gpio.h"

void module_jeti_initializeConfig(void* config);
void module_jeti_start(const struct hardware_port_cfg* port, uint8_t* config);


#endif /* MODULE_JETI_H_ */
