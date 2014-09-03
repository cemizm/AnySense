/*
 * module_mavlink.c
 *
 *  Created on: 29.08.2014
 *      Author: cem
 */

#include "module_mavlink_priv.h"

uint16_t mavlink_pack_nextData(mavlink_message_t* msg, uint8_t* currentValue)
{

	uint16_t len = 0;

	switch (*currentValue)
	{
	case 0:
		len = mavlink_pack_heartbeat(msg);
		break;
	case 1:
		len = mavlink_pack_sys_status(msg);
		break;
	case 2:
		len = mavlink_pack_gps(msg);
		break;
	case 3:
		len = mavlink_pack_vfr_hud(msg);
		break;
	case 4:
		len = mavlink_pack_attitude(msg);
		break;
	case 5:
		len = mavlink_pack_rc_out(msg);
		break;
	}

	*currentValue = *currentValue + 1;

	if (*currentValue > 5)
		*currentValue = 0;

	return len;
}

uint16_t mavlink_pack_heartbeat(mavlink_message_t* msg)
{
	return mavlink_msg_heartbeat_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, MAVLINK_TYPE, MAVLINK_AP_TYPE, MAV_MODE_PREFLIGHT,
			simpleTelemtryData.mode, simpleTelemtryData.mode == flightMode_failsafe ? MAV_STATE_CRITICAL : MAV_STATE_ACTIVE);
}

uint16_t mavlink_pack_sys_status(mavlink_message_t* msg)
{
	return mavlink_msg_sys_status_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, MAVLINK_SENSORS, MAVLINK_SENSORS, MAVLINK_SENSORS,
			0, simpleTelemtryData.battery, 0, 0, 0, 0, simpleTelemtryData.packets_lost, simpleTelemtryData.packets_drop,
			simpleTelemtryData.packets_corrupted, 0);
}

uint16_t mavlink_pack_gps(mavlink_message_t* msg)
{
	//TODO: fix missing values
	return mavlink_msg_gps_raw_int_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, simpleTelemtryData.fixType,
			simpleTelemtryData.lat * 10000000, simpleTelemtryData.lon * 10000000, simpleTelemtryData.alt * 1000,
			simpleTelemtryData.hdop * 100, simpleTelemtryData.vdop * 100, simpleTelemtryData.speed * 100,
			simpleTelemtryData.cog * 100, simpleTelemtryData.numSat);
}
uint16_t mavlink_pack_vfr_hud(mavlink_message_t* msg)
{
	//TODO: fix missing values
	return mavlink_msg_vfr_hud_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, simpleTelemtryData.speed,
			simpleTelemtryData.heading, (simpleTelemtryData.throttle + 1000) / 20, simpleTelemtryData.alt,
			simpleTelemtryData.vsi);
}

uint16_t mavlink_pack_attitude(mavlink_message_t* msg)
{
	//TODO: fix missing values
	return mavlink_msg_attitude_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, simpleTelemtryData.roll,
			simpleTelemtryData.pitch, 0, 0, 0, 0);
}

uint16_t mavlink_pack_rc_out(mavlink_message_t* msg)
{
	return mavlink_msg_rc_channels_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, 10, simpleTelemetry_getRCIn(0),
			simpleTelemetry_getRCIn(1), simpleTelemetry_getRCIn(2), simpleTelemetry_getRCIn(3), simpleTelemetry_getRCIn(4),
			simpleTelemetry_getRCIn(5), simpleTelemetry_getRCIn(6), simpleTelemetry_getRCIn(7), simpleTelemetry_getRCIn(8),
			simpleTelemetry_getRCIn(9), UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX,
			UINT16_MAX, 255);
}

