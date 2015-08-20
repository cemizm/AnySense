/*
 * module_multiplex_priv.h
 *
 *  Created on: 19.08.2015
 *      Author: cem
 */

#ifndef MODULE_MULTIPLEX_PRIV_H_
#define MODULE_MULTIPLEX_PRIV_H_

#include "module_multiplex.h"
#include "simpletelemtry.h"

#include "utlist.h"
#include "fifo.h"
#include "CoOS.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MODULE_MULTIPLEX_TASK_STACK			256
#define MODULE_MULTIPLEX_ADRESSES			16
#define MODULE_MULTIPLEX_RESPONSE_SIZE		3
#define MODULE_MULTIPLEX_RECEIVE_TIMEOUT	15

enum Multiplex_Telemetry_Value
{
	Multiplex_Telemetry_Value_None = 0,
	Multiplex_Telemetry_Value_Satellite = 1,
	Multiplex_Telemetry_Value_GPS_Fix = 2,
	Multiplex_Telemetry_Value_Flightmode = 3,
	Multiplex_Telemetry_Value_Speed = 4,
	Multiplex_Telemetry_Value_Distance = 5,
	Multiplex_Telemetry_Value_Altitude = 6,
	Multiplex_Telemetry_Value_VSpeed = 7,
	Multiplex_Telemetry_Value_Compass = 8,
	Multiplex_Telemetry_Value_COG = 9,
	Multiplex_Telemetry_Value_HomeDirection = 10,
	Multiplex_Telemetry_Value_Voltage = 11,
	Multiplex_Telemetry_Value_Current = 12,
	Multiplex_Telemetry_Value_Capacity = 13,
	Multiplex_Telemetry_Value_LipoVoltage = 14,
	Multiplex_Telemetry_Value_LipoTemp = 15,

};

enum Multiplex_Units
{
	Multiplex_Units_Voltage = 1, //0.1V -600 +600
	Multiplex_Units_Current = 2, //0.1A -1000 +1000
	Multiplex_Units_VSpeed = 3, //0,1m/s -500 +500
	Multiplex_Units_Speed = 4, //0,1km/h 0 +6000
	Multiplex_Units_RPM = 5, //100 1/min 0	+500
	Multiplex_Units_Temp = 6, //0,1°C -250 7000
	Multiplex_Units_Dir = 7, //0,1 Grad (°) 0 3600
	Multiplex_Units_Meter = 8, //1m -500 2000
	Multiplex_Units_Percent = 9, //1% Tank 0 +100
	Multiplex_Units_LQI = 10, //1% LQI 0 +100
	Multiplex_Units_Capacity = 11, //1mAh -16000 16000
	Multiplex_Units_Liquid = 12, //1mL 0 16000
	Multiplex_Units_Distance = 13, //0,1km 0 16000
};

struct Multplex_Data_Response
{
	enum Multiplex_Units Unit :4;
	uint8_t Address :4;
	uint8_t Alarm :1;
	int16_t Value :15;
}__attribute__((packed, aligned(1)));

struct Multiplex_Config
{
	uint8_t version;
	enum Multiplex_Telemetry_Value mapping[MODULE_MULTIPLEX_ADRESSES];
}__attribute__((packed, aligned(1)));

struct Multiplex_Session
{
	const struct hardware_port_cfg* port;
	struct Multiplex_Config* config;

	struct Multplex_Data_Response response;

	uint8_t address;
	uint8_t read;

	OS_FlagID flag;
	OS_TID task_id;
	OS_STK task_stack[MODULE_MULTIPLEX_TASK_STACK];

	struct Multiplex_Session* next;
};

static struct Multiplex_Session* multiplex_sessions;

void module_multiplex_task(void* pData);

static void module_multiplex_irq_rx(uint8_t* pHandle);
static void module_multiplex_irq_timeout(uint8_t* pHandle);
static void module_multiplex_irq_dma(uint8_t* pHandle);

#endif /* MODULE_MULTIPLEX_PRIV_H_ */
