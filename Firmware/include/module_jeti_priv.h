/*
 * module_jeti_priv.h
 *
 *  Created on: 09.02.2015
 *      Author: cem
 */

#ifndef MODULE_JETI_PRIV_H_
#define MODULE_JETI_PRIV_H_

#include "module_jeti.h"

#include "simpletelemtry.h"

#include "config.h"
#include "utlist.h"
#include "fifo.h"
#include "CoOS.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define MODULE_JETI_TASK_STACK		256

#define MODULE_JETI_TIM_PRESCALE	16 - 1
#define MODULE_JETI_TIM_SAMPLE		312 - 1

#define MODULE_JETI_CRC_POLY		0x07

#define MODULE_JETI_EX_DELAY		19

#define MODULE_JETI_LABEL_INIT		5
#define MODULE_JETI_LABEL_UPDATE	5

#define MODULE_JETI_ALRM_DELAY		10
#define MODULE_JETI_ALRM_CHECK_RATE	3
#define MODULE_JETI_ALRM_COUNT		6
#define MODULE_JETI_ALRM_MAX_TYPE	4

#define MODULE_JETI_LABEL_MAX_NAME	10
#define MODULE_JETI_LABEL_MAX_UNIT	3

#define MODULE_JETI_EX_MAX_SIZE		29
#define MODULE_JETI_EX_HEADER_SIZE	8
#define MODULE_JETI_EX_CRC_SIZE		1

#define MODULE_JETI_EX_CONTENT_SIZE	MODULE_JETI_EX_MAX_SIZE - MODULE_JETI_EX_HEADER_SIZE - MODULE_JETI_EX_CRC_SIZE

#define MODULE_JETI_EX_PACKETS		15
#define MODULE_JETI_EX_LABELS		16

#define MODULE_JETI_TELEMETRY_MAX	19

#define MODULE_JETI_EX_START_BYTE	0x7E

enum Jeti_Tone
{
	Jeti_Tone_NoReminder = 0x22, 	//without reminder tone (Vario)
	Jeti_Tone_Remind = 0x23,		//with reminder tone (standard alarm)
};

struct Jeti_Alarms
{
	uint8_t seperator; 			//FIX: 0x7E
	uint8_t length :4; 			//number of bytes following (always 2),
	uint8_t ex :4; 				//can be any number.
	enum Jeti_Tone reminder;	//Reminder Tone
	uint8_t morse; 			//ASCII letter to be signalized by Morse alarm

}__attribute__((packed, aligned(1)));

struct Jeti_EX_Header
{
	uint8_t seperator; 			//FIX: 0x7E
	uint8_t exId;				//FIX: 0x9F Distinct identification of an EX packet, N could be an arbitrary number.
	uint8_t length :6; 			//Length of a packet (number of bytes following)
	uint8_t type :2; 			//Packet type; 1 – Data protocol, 0 - Text protocol
	uint16_t manufactureId;		//Upper part of a serial number, Manufacturer ID (Little Endian)
	uint16_t deviceId;			//Lower part of a serial number, Device ID (Little Endian)
	uint8_t reserved;			//FIX: 0x00 reserved
	uint8_t data[MODULE_JETI_EX_CONTENT_SIZE];
}__attribute__((packed, aligned(1)));

struct Jeti_Type6_t
{
	uint8_t data :5;
	uint8_t decimalpoint :2;
	uint8_t sign :1;
}__attribute__((packed, aligned(1)));

struct Jeti_Type14_t
{
	uint16_t data :13;
	uint8_t decimalpoint :2;
	uint8_t sign :1;
}__attribute__((packed, aligned(1)));

struct Jeti_Type22_t
{
	uint32_t data :21;
	uint8_t decimalpoint :2;
	uint8_t sign :1;
}__attribute__((packed, aligned(1)));

struct Jeti_Type30_t
{
	uint32_t data :29;
	uint8_t decimalpoint :2;
	uint8_t sign :1;
}__attribute__((packed, aligned(1)));

struct Jeti_TypeGPS
{
	uint16_t minutes;
	uint16_t degree :13;
	uint8_t latlon :1; //lat=0; lon=1;
	uint8_t ns_ew :1; //North/East = 0; South/West = 1
	uint8_t sign :1; //unused??
}__attribute__((packed, aligned(1)));

struct Jeti_EX_Text
{
	uint8_t id;
	uint8_t length_unit :3;
	uint8_t length_desc :5;
	uint8_t labels[MODULE_JETI_EX_CONTENT_SIZE - 1];
}__attribute__((packed, aligned(1)));

struct Jeti_EX_Data
{
	uint8_t dataType :4;
	uint8_t id :4;
	union
	{
		uint8_t data[4];
		struct Jeti_Type6_t t_u6;
		struct Jeti_Type14_t t_u14;
		struct Jeti_Type22_t t_u22;
		struct Jeti_Type30_t t_u30;
		struct Jeti_TypeGPS t_gps;
	} data;
}__attribute__((packed, aligned(1)));

struct Jeti_EX_Packet
{
	uint8_t length;
	struct Jeti_EX_Data data;
};

enum Jeti_Telemetry_Value
{
	Jeti_Telemetry_Value_None = 0,
	Jeti_Telemetry_Value_Latitude = 1,
	Jeti_Telemetry_Value_Longitude = 2,
	Jeti_Telemetry_Value_Satellite = 3,
	Jeti_Telemetry_Value_GPS_Fix = 4,
	Jeti_Telemetry_Value_Flightmode = 5,
	Jeti_Telemetry_Value_Speed = 6,
	Jeti_Telemetry_Value_Distance = 7,
	Jeti_Telemetry_Value_Altitude = 8,
	Jeti_Telemetry_Value_VSpeed = 9,
	Jeti_Telemetry_Value_Compass = 10,
	Jeti_Telemetry_Value_COG = 11,
	Jeti_Telemetry_Value_HomeDirection = 12,
	Jeti_Telemetry_Value_Voltage = 13,
	Jeti_Telemetry_Value_Current = 14,
	Jeti_Telemetry_Value_Capacity = 15,
	Jeti_Telemetry_Value_LipoVoltage = 16,
	Jeti_Telemetry_Value_LipoTemp = 17,
	Jeti_Telemetry_Value_Home_Latitude = 18,
	Jeti_Telemetry_Value_Home_Longitude = 19,

};

struct Jeti_Telemetry_Text
{
	uint8_t name[MODULE_JETI_LABEL_MAX_NAME];
	uint8_t unit[MODULE_JETI_LABEL_MAX_UNIT];
};

static const struct Jeti_Telemetry_Text module_jeti_telemetry_labels[MODULE_JETI_TELEMETRY_MAX + 1] =
{
	{	.name = "AnySense", .unit = ""},
	{	.name = "Latitude", .unit = ""},
	{	.name = "Longitude", .unit = ""},
	{	.name = "Satelliten", .unit = ""},
	{	.name = "GPS Fix", .unit = ""},
	{	.name = "Flugmodus", .unit = ""},
	{	.name = "Geschwndg.", .unit = "kmh"},
	{	.name = "Entfernung", .unit = "m"},
	{	.name = "Hoehe", .unit = "m"},
	{	.name = "Vario", .unit = "m/s"},
	{	.name = "Kompass", .unit = "\xB0"},
	{	.name = "Flugricht.", .unit = "\xB0"},
	{	.name = "Homericht.", .unit = "\xB0"},
	{	.name = "Spannung", .unit = "V"},
	{	.name = "Strom", .unit = "A"},
	{	.name = "Kapazitaet", .unit = "Ah"},
	{	.name = "Lipo Zelle", .unit = "V"},
	{	.name = "Lipo Temp.", .unit = "\xB0" "C"},
	{	.name = "Home Lat.", .unit = ""},
	{	.name = "Home Long.", .unit = ""},
};

enum Jeti_Alarm_Type
{
	Jeti_Alarm_Type_None = 0,
	Jeti_Alarm_Type_Altitude = 1,
	Jeti_Alarm_Type_Capacity = 2,
	Jeti_Alarm_Type_Distance = 3,
	Jeti_Alarm_Type_Battery = 4,
};

struct Jeti_Alarm_Config
{
	uint16_t minMaxValue;
	uint8_t event;
	enum Jeti_Alarm_Type alarmType; //min=0; max=1
}__attribute__((packed, aligned(1)));

struct Jeti_Config
{
	uint8_t version;
	enum Jeti_Telemetry_Value values[MODULE_JETI_EX_PACKETS];
	struct Jeti_Alarm_Config alarms[MODULE_JETI_ALRM_COUNT];
}__attribute__((packed, aligned(1)));

struct Jeti_Session
{
	const struct hardware_port_cfg* port;
	struct Jeti_Config* config;

	FIFO64_t txBuffer;
	uint8_t lastCommand;

	uint8_t currentData;
	uint8_t pos;
	uint8_t parity;
	uint64_t lastBit;

	OS_FlagID flag;
	OS_TID task_id;
	OS_STK task_stack[MODULE_JETI_TASK_STACK];

	struct Jeti_Session* next;
};

static struct Jeti_Session* jeti_sessions;

void module_jeti_task(void* pData);

static void module_jeti_tim_callback(uint8_t* id);

int jeti_cmp_alarm(const void * a, const void * b);

#endif /* MODULE_JETI_PRIV_H_ */
