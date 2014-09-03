/*
 * module_mavlink_priv.h
 *
 *  Created on: 29.08.2014
 *      Author: cem
 */

#ifndef MODULE_MAVLINK_PRIV_H_
#define MODULE_MAVLINK_PRIV_H_

#include "module_mavlink.h"

uint16_t mavlink_pack_heartbeat(mavlink_message_t* msg);
uint16_t mavlink_pack_sys_status(mavlink_message_t* msg);
uint16_t mavlink_pack_gps(mavlink_message_t* msg);
uint16_t mavlink_pack_vfr_hud(mavlink_message_t* msg);
uint16_t mavlink_pack_attitude(mavlink_message_t* msg);
uint16_t mavlink_pack_rc_out(mavlink_message_t* msg);

#endif /* MODULE_MAVLINK_PRIV_H_ */
