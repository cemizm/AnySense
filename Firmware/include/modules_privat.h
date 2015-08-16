/*
 * modules_privat.h
 *
 *  Created on: 29.08.2014
 *      Author: cem
 */

#ifndef MODULES_PRIVAT_H_
#define MODULES_PRIVAT_H_

#include "CoOS.h"

#include "modules.h"
#include "hardware.h"
#include "config.h"
#include "fifo.h"
#include "UniAdapter/mavlink.h"
#include "simpletelemtry.h"

#include <stdlib.h>

//modules
#include "module_SPort.h"
#include "module_mavlink.h"
#include "module_hott.h"
#include "module_futaba.h"
#include "module_jeti.h"
#include "module_spektrum.h"

#define CONFIGMANAGER_TASK_STACK		512
#define WAIT_RETRY						5
#define RX_QUEUE_SIZE					2

mavlink_status_t tmp_mavlink_status;

struct mavlink_rx_buffer rx_buffer[RX_QUEUE_SIZE];


OS_STK configManager_task_stack[CONFIGMANAGER_TASK_STACK];
OS_EventID command_box_id;
void* command_queue[RX_QUEUE_SIZE];

OS_TID configManager_task_id;

FIFO512_t tx_buffer;

void modules_initialize_config(struct portParserStruct* parser);

void configManager_task(void* pdata);
void configManager_start(struct portParserStruct* parser, const struct hardware_port_cfg* port);

static void RX_Callback(uint8_t* id);
static void TX_Callback(uint8_t* id);

#endif /* MODULES_PRIVAT_H_ */
