/*
 * SPort_priv.h
 *
 *  Created on: 22.08.2014
 *      Author: cem
 */

#ifndef MODULE_SPORT_PRIV_H_
#define MODULE_SPORT_PRIV_H_

#include "module_SPort.h"
#include "simpletelemtry.h"

#include "utlist.h"
#include "fifo.h"
#include "CoOS.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

enum SPort_State
{
	WAITING_START_FRAME, WAITING_SESNOR_ID
};

#define SPORT_TASK_STACK				128

#define SPORT_STARTSTOP					0x7E
#define SPORT_DATAFRAME					0x10
#define SPORT_CONFIG_CURRENT_VERSION	((uint16_t)0x0002)
#define SPORT_VALUES_MAX				20

/* Types */

struct SPort_Packet
{
	uint8_t Header;
	uint16_t Id;
	uint32_t Value;

}__attribute__((packed, aligned(1)));

typedef enum
{
	SENSOR_ALT = 0x0100,
	SENSOR_VARIO = 0x0110,
	SENSOR_CURR = 0x0200,
	SENSOR_VFAS = 0x0210,
	SENSOR_CELLS = 0x0300,
	SENSOR_T1 = 0x0400,
	SENSOR_T2 = 0x0410,
	SENSOR_RPM = 0x0500,
	SENSOR_FUEL = 0x0600,
	SENSOR_ACCX = 0x0700,
	SENSOR_ACCY = 0x0710,
	SENSOR_ACCZ = 0x0720,
	SENSOR_GPS_LONG_LATI = 0x0800,
	SENSOR_GPS_ALT = 0x0820,
	SENSOR_GPS_SPEED = 0x0830,
	SENSOR_GPS_COURS = 0x0840,
	SENSOR_GPS_TIME_DATE = 0x0850,
	SENSOR_A3 = 0x0900,
	SENSOR_A4 = 0x0910,
	SENSOR_AIR_SPEED = 0x0a00,
} SensorId;

enum telemetryValue
{
	tv_none = 0,
	tv_lon_lat = 1,
	tv_alt = 2,
	tv_gpsAlt = 3,
	tv_speed = 4,
	tv_gps_fix = 5,
	tv_numSat = 6,
	tv_heading = 7,
	tv_haadingNC = 8,
	tv_cog = 9,
	tv_vsi = 10,
	tv_gpsVsi = 11,
	tv_hdop = 12,
	tv_vdop = 13,
	tv_pitch = 14,
	tv_roll = 15,
	tv_battery = 16,
	tv_throttle = 17,
	tv_flight_mode = 18,
	tv_lastHeartbeat = 19,
	tv_gpsTime = 20,
	tv_alt_relative = 21,
};

struct SPort_Config
{
	uint8_t sensorId;
	uint16_t version;
	enum fixType minFix;
	enum telemetryValue map[SPORT_VALUES_MAX];
}__attribute__((packed, aligned(1)));

struct SPort_SessionStruct
{
	const struct hardware_port_cfg* port;
	enum SPort_State currentState;

	struct SPort_Config* config;

	FIFO64_t send_buffer;

	uint8_t currentValue;
	struct SPort_Packet tmp;
	int32_t tmp_bytes[4];
	uint8_t tmp_length;

	struct SPort_SessionStruct* next;
};

typedef struct SPort_SessionStruct SPortSession;

/* members */

static SPortSession* sessions;

static const uint16_t mapping[] = { SENSOR_T1, SENSOR_T2, SENSOR_RPM, SENSOR_FUEL, SENSOR_ALT, SENSOR_VARIO, SENSOR_ACCX,
		SENSOR_ACCY, SENSOR_ACCZ, SENSOR_CURR, SENSOR_VFAS, SENSOR_CELLS, SENSOR_GPS_LONG_LATI, SENSOR_GPS_ALT, SENSOR_GPS_SPEED,
		SENSOR_GPS_COURS, SENSOR_GPS_TIME_DATE, SENSOR_A3, SENSOR_A4, SENSOR_AIR_SPEED};

static const struct SPort_Config sport_defaultConfig =
{
	.sensorId = 0,
	.version = SPORT_CONFIG_CURRENT_VERSION,
	.minFix = fixType_2D,
	.map =
	{	tv_numSat, tv_gps_fix, tv_none, tv_none, tv_alt, tv_vsi, tv_roll, tv_pitch, tv_none, tv_none,
		tv_battery, tv_none, tv_lon_lat, tv_gpsAlt, tv_speed, tv_heading, tv_gpsTime, tv_none, tv_none, tv_none},
};

/* functions */

void sport_task(void* pdata);
void sport_getValue(enum telemetryValue val, enum fixType minFixTyp, int32_t * result, uint8_t* len);

static void RX_Callback(uint8_t* id);
static void TX_Callback(uint8_t* id);

#endif /* MODULE_SPORT_PRIV_H_ */
