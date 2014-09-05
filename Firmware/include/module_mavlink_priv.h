/*
 * module_mavlink_priv.h
 *
 *  Created on: 29.08.2014
 *      Author: cem
 */

#ifndef MODULE_MAVLINK_PRIV_H_
#define MODULE_MAVLINK_PRIV_H_

#include "module_mavlink.h"
#include "simpletelemtry.h"

#include "CoOS.h"
#include "fifo.h"
#include "utlist.h"

#include <stdlib.h>

uint16_t mavlink_pack_heartbeat(mavlink_message_t* msg);
uint16_t mavlink_pack_sys_status(mavlink_message_t* msg);
uint16_t mavlink_pack_gps(mavlink_message_t* msg);
uint16_t mavlink_pack_vfr_hud(mavlink_message_t* msg);
uint16_t mavlink_pack_attitude(mavlink_message_t* msg);
uint16_t mavlink_pack_rc_out(mavlink_message_t* msg);

#define MODULE_MAVLINK_TASK_STACK				512
#define MODULE_MAVLINK_RX_QUEUE_SIZE			2

struct mavlink_session
{
	const struct hardware_port_cfg* port;

	struct mavlink_rx_buffer rx_buffer[MODULE_MAVLINK_RX_QUEUE_SIZE];
	FIFO512_t tx_buffer;
	mavlink_status_t tmp_status;

	uint8_t* config; //unused

	OS_STK task_stack[MODULE_MAVLINK_TASK_STACK];
	OS_EventID event_id;
	void* queue[MODULE_MAVLINK_RX_QUEUE_SIZE];
	OS_TID task_id;

	struct mavlink_session* next;
};

static struct mavlink_session* mavlink_sessions;

void module_mavlink_task(void* pData);

static void RX_Callback(uint8_t* id);
static void TX_Callback(uint8_t* id);

#endif /* MODULE_MAVLINK_PRIV_H_ */
