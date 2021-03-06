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
	WAITING_START_FRAME, WAITING_SESNOR_ID, DATA_FRAME
};

#define SPORT_TASK_STACK				256

#define SPORT_STARTSTOP					0x7E
#define SPORT_DATAFRAME					0x10
#define SPORT_CONFIG_CURRENT_VERSION	((uint16_t)0x0001)
#define SPORT_VALUES_MAX				20
#define SPORT_DATA_SIZE					8

#define SPORT_SENSORS					29
#define SPORT_ACTIVE_SENSORS			5
#define SPORT_ACTIVE_SENSOR_VALUES		10
#define SPORT_ACTIVE_SENSOR_TIMEOUT		20
#define SPORT_ACTIVE_SENSOR_DATA		12
#define SPORT_ACTIVE_SENSOR_CELLS		6

#define SPORT_DATA_U8(packet)   (((uint8_t*)packet)[0]])
#define SPORT_DATA_S32(packet)  (int32_t)packet
#define SPORT_DATA_U32(packet)  packet

#define SPORT_DATA_FLAG_ALT_SET 		0b00000001
#define SPORT_DATA_FLAG_GPS_HOME_SET	0b00000010

#define SPORT_DATA_IS_FLAG_SET(session, flag)	((session->firstFlag & flag) == flag)
#define SPORT_DATA_FLAG_SET(session, flag)		session->firstFlag = session->firstFlag | flag

#define SPORT_CAPACITY_UPDATE_INTERVAL		50 //ms
#define SPORT_CAPACITY_COUNTER_LIMIT 		(36000 / SPORT_CAPACITY_UPDATE_INTERVAL)

/* Types */

struct SPort_Packet
{
	uint8_t Header;
	uint16_t Id;
	uint32_t Value;
	uint8_t crc;
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
	tv_current = 22,
	tv_cells = 23,
	tv_combined = 24,
};

struct SPort_Config
{
	uint8_t sensorId;
	uint16_t version;
	enum fixType minFix;
	enum telemetryValue map[SPORT_VALUES_MAX];
}__attribute__((packed, aligned(1)));

static const uint8_t SPort_SensorIds[] = { 0x00, 0x16, 0xA1, 0xCB, 0x95, 0x83, 0x0D, 0xB7, 0xE4, 0x8E, 0x98, 0xC6, 0xD0, 0xBA,
		0x48, 0xF2, 0x7E, 0x6A, 0x34, 0x22, 0xAC, 0x39, 0x45, 0x2F, 0x1B, 0x67, 0x71, 0xE9, 0x53 };

struct SPort_ActiveSensor_Lipo
{
	uint8_t cells;
	uint16_t cell[SPORT_ACTIVE_SENSOR_CELLS];
};

struct SPort_ActiveSensorValues
{
	uint16_t TypeId;
	uint32_t Value;

	union
	{
		uint8_t data[SPORT_ACTIVE_SENSOR_DATA];
		struct SPort_ActiveSensor_Lipo lipo;
		uint32_t current;
	}payload;
};

struct SPort_ActiveSensor
{
	uint8_t SensorId;
	uint8_t active;
	struct SPort_ActiveSensorValues values[SPORT_ACTIVE_SENSOR_VALUES];
};

struct SPort_LipoData
{
	uint8_t StartCell:4;
	uint8_t TotalCells:4;
	uint16_t Cell1:12;
	uint16_t Cell2:12;
}__attribute__((packed, aligned(1)));

struct SPort_SessionStruct
{
	const struct hardware_port_cfg* port;
	enum SPort_State currentState;

	struct SPort_Config* config;

	FIFO64_t send_buffer;

	struct SPort_Packet rxPacketTemp;
	uint8_t rxPointer;

	uint8_t currentSensorId;
	struct SPort_ActiveSensor activeSensors[SPORT_ACTIVE_SENSORS];

	uint8_t firstFlag; //flag to check for initial value of some value types

	U64 nextCurrentMeasure;
	uint32_t currentElapsed;

	OS_FlagID flag;
	OS_TID task_id;
	OS_STK task_stack[SPORT_TASK_STACK];

	struct SPort_SessionStruct* next;
};

typedef struct SPort_SessionStruct SPortSession;

/* members */

static SPortSession* sessions;

static const uint16_t mapping[] = { SENSOR_T1, SENSOR_T2, SENSOR_RPM, SENSOR_FUEL, SENSOR_ALT, SENSOR_VARIO, SENSOR_ACCX,
		SENSOR_ACCY, SENSOR_ACCZ, SENSOR_CURR, SENSOR_VFAS, SENSOR_CELLS, SENSOR_GPS_LONG_LATI, SENSOR_GPS_ALT, SENSOR_GPS_SPEED,
		SENSOR_GPS_COURS, SENSOR_GPS_TIME_DATE, SENSOR_A3, SENSOR_A4, SENSOR_AIR_SPEED };

static const struct SPort_Config sport_defaultConfig =
{
	.sensorId = 0,
	.version = SPORT_CONFIG_CURRENT_VERSION,
	.minFix = fixType_2D,
	.map =
	{	tv_numSat, tv_gps_fix, tv_none, tv_combined, tv_alt, tv_vsi, tv_roll, tv_pitch, tv_none, tv_current,
		tv_battery,tv_cells, tv_lon_lat, tv_gpsAlt, tv_speed, tv_heading, tv_gpsTime, tv_none, tv_none, tv_none},
};

/* functions */

void sport_task(void* pdata);
void sport_getValue(SPortSession* session, enum telemetryValue val, enum fixType minFixTyp, int32_t * result, uint8_t* len);
uint8_t sport_valueReady(SPortSession* session, enum telemetryValue val, enum fixType minFixTyp);

static void RX_Callback(uint8_t* id);
static void TX_Callback(uint8_t* id);

#endif /* MODULE_SPORT_PRIV_H_ */
