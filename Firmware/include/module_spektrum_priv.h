/*
 * module_spektrum_priv.h
 *
 *  Created on: 06.08.2015
 *      Author: cem
 */

#ifndef MODULE_SPEKTRUM_PRIV_H_
#define MODULE_SPEKTRUM_PRIV_H_

#include "module_spektrum.h"

#include "simpletelemtry.h"

#include "utlist.h"
#include "fifo.h"
#include "CoOS.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MODULE_SPEKTRUM_TASK_STACK		256
#define MODULE_SPEKTRUM_FRAME_SIZE		16

#define MODULE_SPEKTRUM_SENSOR_ENABLED	10

#define MODULE_SPEKTRUM_SENSOR_CURRENT	0x03
#define MODULE_SPEKTRUM_SENSOR_POWERBOX	0x0A
#define MODULE_SPEKTRUM_SENSOR_GPS1		0x16
#define MODULE_SPEKTRUM_SENSOR_GPS2		0x17
#define MODULE_SPEKTRUM_SENSOR_VARIO	0x40

enum spektrum_sensors
{
	spektrum_sensor_none = 0,
	spektrum_sensor_gps = 1,
	spektrum_sensor_vario = 2,
	spektrum_sensor_current = 4,
	spektrum_sensor_powerbox = 8,
};

struct Spektrum_Config
{
	uint8_t version;
	uint8_t cells;
	uint8_t lowVoltage;
	uint16_t maxCapacity;

	enum spektrum_sensors active_sensors;			//bitmask for sensors

}__attribute__((packed, aligned(1)));

struct Spektrum_Sensor_Current
{
	uint8_t address;
	uint8_t reserved;
	uint16_t current;

}__attribute__((packed, aligned(1)));

struct Spektrum_Sensor_Vario
{
	uint8_t address;
	uint8_t reserved;
}__attribute__((packed, aligned(1)));

struct Spektrum_Sensor_Powerbox
{
	uint8_t address;
	uint8_t reserved;
	uint16_t voltage1;
	uint16_t voltage2;
	uint16_t capacity1;
	uint16_t capacity2;
	uint8_t unused[5];
	uint8_t voltage1_alarm :1;
	uint8_t voltage2_alarm :1;
	uint8_t capacity1_alarm :1;
	uint8_t capacity2_alarm :1;
}__attribute__((packed, aligned(1)));

struct Spektrum_GPS_Degree
{
	uint16_t minfrac;
	uint8_t min;
	uint8_t deg;
}__attribute__((packed, aligned(1)));

struct Spektrum_Sensor_GPS1
{
	uint8_t address;
	uint8_t reserved;
	uint16_t alt; //Altitude = Altitude(0x17) * 1000 + Value (in 0.1m)
	struct Spektrum_GPS_Degree lat;
	struct Spektrum_GPS_Degree lon;
	uint16_t cog;
	uint8_t unkn1;
	uint8_t isNorth :1;
	uint8_t isEast :1;
	uint8_t lonExceed :1;
}__attribute__((packed, aligned(1)));

struct Spektrum_Sensor_GPS2
{
	uint8_t address;
	uint8_t reserved;
	uint16_t speed; // Divide by 10 for Knots. Multiply by 0.185 for Kph and 0.115 for Mph
	uint8_t sec_100th;
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t sats;
	uint8_t alt;

}__attribute__((packed, aligned(1)));

struct Spektrum_Session
{
	const struct hardware_port_cfg* port;
	struct Spektrum_Config* config;

	uint8_t enabledSensors[MODULE_SPEKTRUM_SENSOR_ENABLED];

	OS_TID task_id;
	OS_STK task_stack[MODULE_SPEKTRUM_TASK_STACK];
	OS_FlagID ready;

	uint8_t sensor;

	union
	{
		uint8_t txBuffer[MODULE_SPEKTRUM_FRAME_SIZE];
		struct Spektrum_Sensor_Current current_t;
		struct Spektrum_Sensor_Powerbox powerbox;
		struct Spektrum_Sensor_GPS1 gps1;
		struct Spektrum_Sensor_GPS2 gps2;
		struct Spektrum_Sensor_Vario vario;
	} data;

	struct Spektrum_Session* next;
};

static struct Spektrum_Session* spektrum_sessions;

void module_spektrum_task(void* pData);

static uint8_t toBCD8(uint8_t dec);
static uint16_t toBCD16(uint16_t dec);
static void module_spektrum_fill_gps(struct Spektrum_GPS_Degree* deg, double dec);

static void module_spektrum_irq_dma(uint8_t* pHandle);
static void module_spektrum_irq_i2c(uint8_t* pHandle);

#endif /* MODULE_SPEKTRUM_PRIV_H_ */
