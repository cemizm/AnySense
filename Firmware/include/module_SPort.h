/*
 * SPort.h
 *
 *  Created on: 22.08.2014
 *      Author: cem
 */

#ifndef MODULE_SPORT_H_
#define MODULE_SPORT_H_

#include "hardware.h"

void sport_initializeConfig(void* config);
void sport_start(const struct hardware_port_cfg* port, uint8_t* config);


#endif /* MODULE_SPORT_H_ */
