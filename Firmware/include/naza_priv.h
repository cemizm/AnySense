/*
 * naza_priv.h
 *
 *  Created on: 25.08.2014
 *      Author: cem
 */

#ifndef NAZA_PRIV_H_
#define NAZA_PRIV_H_

#include "CoOS.h"
#include "naza.h"
#include "hardware.h"
#include "simpletelemtry.h"

#include "stm32f0xx_can.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CHANNEL_COUNT					6
#define CHANNEL_SIZE 					256
#define MAIN_TASK_STACK_SIZE			128

#define HEADER1 			0x55
#define HEADER2				0xAA
#define FOOTER1				0x66
#define FOOTER2				0xCC

enum channel_state
{
	None = 0, InUse = 1, Streaming = 2, Processing = 3
};

struct naza_header
{
	uint16_t id;
	uint16_t length;
}__attribute__((packed));

union naza_message
{
	uint8_t bytes[CHANNEL_SIZE];
	struct naza_header header;
};

struct msg_osd
{
	struct naza_header header;
	double lon;           // longitude (radians)
	double lat;           // lattitude (radians)
	float altGps;        // altitude from GPS (meters)
	float accX;          // accelerometer X axis data (??)
	float accY;          // accelerometer Y axis data (??)
	float accZ;          // accelerometer Z axis data (??)
	float gyrX;          // gyroscope X axis data (??)
	float gyrY;          // gyroscope Y axis data (??)
	float gyrZ;          // gyroscope Z axis data (??)
	float altBaro;       // altitude from barometric sensor (meters)
	float unk0[7];
	float northVelocity; // averaged northward velocity or 0 when less than 5 satellites locked (m/s)
	float eastVelocity; // averaged eastward velocity or 0 when less than 5 satellites locked (m/s)
	float downVelocity;  // downward velocity (barometric) (m/s)
	float unk1[3];
	int16_t magCalX;       // calibrated magnetometer X axis data
	int16_t magCalY;       // calibrated magnetometer Y axis data
	int16_t magCalZ;       // calibrated magnetometer Y axis data
	uint8_t unk2[10];
	uint8_t numSat;        // number of locked satellites
	uint8_t unk3;
	uint16_t seqNum;       // sequence number - increases with every message
}__attribute__((packed));

struct msg_gps
{
	struct naza_header header;
	uint32_t dateTime;      // date/time
	uint32_t lon;           // longitude (x10^7, degree decimal)
	uint32_t lat;           // lattitude (x10^7, degree decimal)
	uint32_t altGps;        // altitude from GPS (millimeters)
	uint32_t hae;          // horizontal accuracy estimate (millimeters)
	uint32_t vae;           // vertical accuracy estimate (millimeters)
	uint8_t unk0[4];
	int32_t northVelocity; // northward velocity (cm/s)
	int32_t eastVelocity;  // eastward velocity (cm/s)
	int32_t downVelocity;  // downward velocity (cm/s)
	uint16_t pdop;          // position DOP (x100)
	uint16_t vdop; // vertical DOP (see uBlox NAV-DOP message for details)
	uint16_t ndop; // northing DOP (see uBlox NAV-DOP message for details)
	uint16_t edop; // easting DOP (see uBlox NAV-DOP message for details)
	uint8_t numSat;        // number of locked satellites
	uint8_t unk1;
	uint8_t fixType; // fix type (0 - no lock, 2 - 2D lock, 3 - 3D lock, not sure if other values can be expected - see uBlox NAV-SOL message for details)
	uint8_t unk2;
	uint8_t fixStatus; // fix status flags (see uBlox NAV-SOL message for details)
	uint8_t unk3[3];
	uint16_t seqNum;   // sequence number - increases with every message
}__attribute__((packed));

struct msg_raw_io
{
	struct naza_header header;
	uint8_t unk1[4];
	uint16_t motorOut[8];  // motor output (M1/M2/M3/M4/M5/M6/F1/F2)
	uint8_t unk2[4];
	int16_t rcIn[10]; // RC controller input (order: unused/A/E/R/U/T/unused/X1/X2/unused)
	uint8_t unk3[11];
	uint8_t flightMode; // (0 - manual, 1 - GPS, 2 - failsafe, 3 - atti)
	uint8_t unk4[8];
	double homeLat;      // home lattitude (radians)
	double homeLon;      // home longitude (radians)
	float homeAltBaro; // home altitude from barometric sensor plus 20m (meters)
	uint16_t seqNum; // sequence number - increases with every message
	uint8_t unk5[2];
	float stabRollIn; // attitude stabilizer roll input (-1000~1000)
	float stabPitchIn; // attitude stabilizer pitch input (-1000~1000)
	float stabThroIn; // altitude stabilizer throttle input (-1000~1000)
	uint8_t unk6[4];
	float actAileIn; // actual aileron input, mode and arm state dependent (-1000~1000)
	float actElevIn; // actual elevator input, mode and arm state dependent (-1000~1000)
	float actThroIn; // actual throttle input, mode and arm state dependent (-1000~1000)
	uint16_t batVolt;      // main battery voltage (milivolts)
	uint16_t becVolt;      // BEC voltage (milivolts)
	uint8_t unk7[4];
	uint8_t controlMode; // (0 - GPS/failsafe, 1 - waypoint mode?, 3 - manual, 6 - atti)
	uint8_t unk8[5];
	int16_t gyrScalX;     // ???
	int16_t gyrScalY;     // ???
	int16_t gyrScalZ;     // ???
	uint8_t unk9[32];
	float downVelocity; // downward velocity (m/s)
	float altBaro;      // altitude from barometric sensor (meters)
	float roll;         // roll angle (radians)
	float pitch;        // pitch angle (radians)
}__attribute__((packed));

struct msg_smartBattery
{
	struct naza_header header;									//		0
	uint16_t capacity_design; 		// design capacity (5200mAh)		4
	uint16_t capacity_full; 		// full capacity (5003mAH)			6
	uint16_t capacity_current;		// current capacity (3115mAh)		8
	uint16_t voltage; 				// voltage (11725mV)				10
	int16_t current;				// current (-371mA)					12
	uint8_t percentage_life;		// percentage of life (93%)			14
	uint8_t percentage_charge;		// percentage of charge (62%)		15
	uint16_t temperature;			// (230 * 0.1 degrees Celsius)		16
	uint16_t dischargedCount;		// discharging times (21)			18
	uint16_t serialNumber;			// serial number (6585)				20
	uint16_t cell3;					//cell 3 (3934mV)					22
	uint16_t cell2;					//cell 2 (3934mV)					24
	uint16_t cell1;					//cell 1 (3911mV)					26
	uint8_t unk2[7];											//		28
}__attribute__((packed, aligned(1)));

struct naza_channel
{
	uint32_t id;
	enum channel_state state;
	uint16_t buffer_position;
	uint8_t header;
	uint8_t footer;
	union naza_message msg;
};

struct naza_channel chans[CHANNEL_COUNT];

#define CHANNEL_FREE(channel) \
	channel->header = 0;	\
	channel->footer = 0;	\
	channel->buffer_position = 0; \
	channel->id = 0;			\
	channel->state = None;

#define CHANNEL_CLEAR(channel)	\
	channel->header = 0;						\
	channel->footer = 0;						\
	channel->buffer_position = 0;				\
	channel->state = InUse;					\

#define CHANNEL_MAP(channel, chanId) \
	channel->header = 0;	\
	channel->footer = 0;	\
	channel->buffer_position = 0; \
	channel->id = chanId;			\
	channel->state = InUse;

#define CHANNEL_STATE(channel, newstate) 	\
	channel->header = 0;						\
	channel->footer = 0;						\
	channel->buffer_position = 0;				\
	channel->state = newstate;					\


OS_STK naza_main_task_stk[MAIN_TASK_STACK_SIZE];

OS_EventID naza_msg_box_id;
void* naza_msg_queue[CHANNEL_COUNT];

OS_TID naza_main_task_id;

static void naza_main_task(void* pData);

#endif /* NAZA_PRIV_H_ */
