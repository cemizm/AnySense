/*
 * module_multiplex.h
 *
 *  Created on: 19.08.2015
 *      Author: cem
 */

#ifndef MODULE_MULTIPLEX_H_
#define MODULE_MULTIPLEX_H_


#include "hardware.h"
#include "stm32f0xx_gpio.h"

void module_multiplex_initializeConfig(void* config);
void module_multiplex_start(const struct hardware_port_cfg* port, uint8_t* config);


#endif /* MODULE_MULTIPLEX_H_ */
