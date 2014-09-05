/*
 * parser_mavlink.h
 *
 *  Created on: 29.08.2014
 *      Author: cem
 */

#ifndef MODULE_MAVLINK_H_
#define MODULE_MAVLINK_H_

#include "common/mavlink.h"
#include "hardware.h"

#define MAVLINK_TX_BUFFER_SIZE					3
#define MAVLINK_TX_BUFFER_SENDING				0b001
#define MAVLINK_TX_BUFFER_READY					0b010

#define MAVLINK_SYSTEM_ID						0xCE
#define MAVLINK_COMP_ID							MAV_COMP_ID_UART_BRIDGE
#define MAVLINK_TYPE							MAV_TYPE_GENERIC
#define MAVLINK_AP_TYPE							MAV_AUTOPILOT_INVALID
#define MAVLINK_SENSORS							MAV_SYS_STATUS_SENSOR_3D_GYRO | MAV_SYS_STATUS_SENSOR_3D_ACCEL | \
												MAV_SYS_STATUS_SENSOR_3D_MAG | MAV_SYS_STATUS_SENSOR_ABSOLUTE_PRESSURE | \
												MAV_SYS_STATUS_SENSOR_GPS | MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION | \
												MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS | MAV_SYS_STATUS_SENSOR_RC_RECEIVER

struct mavlink_rx_buffer
{
	mavlink_message_t message;
	uint8_t inUse;
};

uint16_t mavlink_pack_nextData(mavlink_message_t* msg, uint8_t* currentValue);

void mavlink_initializeConfig(void* config);
void mavlink_start(const struct hardware_port_cfg* port, uint8_t* config);

#endif /* MODULE_MAVLINK_H_ */
