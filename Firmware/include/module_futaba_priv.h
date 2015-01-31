/*
 * module_futaba_priv.h
 *
 *  Created on: 03.01.2015
 *      Author: cem
 */

#ifndef MODULE_FUTABA_PRIV_H_
#define MODULE_FUTABA_PRIV_H_

#include "module_futaba.h"

#include "simpletelemtry.h"

#include "utlist.h"
#include "fifo.h"
#include "CoOS.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MODULE_FUTABA_TASK_STACK		256
#define MODULE_FUTABA_FRAME_SIZE		25
#define MODULE_FUTABA_BUFFER_SIZE		(MODULE_FUTABA_FRAME_SIZE * 3)

#define MODULE_FUTABA_SLOTS				32
#define MODULE_FUTABA_SLOT_DATA			2
#define MODULE_FUTABA_SLOTS_FRAME		8
#define MODULE_FUTABA_SLOT_SET			0b00000100

#define MODULE_FUTABA_TIMING_START		(49440 - 1)
#define MODULE_FUTABA_TIMING_DATA		(15840 - 1)

#define MODULE_FUTABA_DATA_START		0x0F

enum Futaba_Values
{
	Futaba_Values_None = 0,
	Futaba_Values_Vario = 1,
	Futaba_Values_Kompass = 2,
	Futaba_Values_Current = 3,
	Futaba_Values_GPS = 4,
	Futaba_Values_Cell = 5,
	Futaba_Values_Sats = 6,
	Futaba_Values_Flightmode = 7,
	Futaba_Values_Temperatur = 8,
};

enum Futaba_Sections
{
	Futaba_Sections_None = 0xFF,
	Futaba_Sections_NonTelemetry = 0x00,
	Futaba_Sections_FirstMain = 0x04,
	Futaba_Sections_SecondMain = 0x14,
	Futaba_Sections_ThridMain = 0x24,
	Futaba_Sections_ForthMain = 0x34
};

struct Futaba_Config
{
	uint8_t version;
	enum fixType minFix;

	enum Futaba_Values timeSlots[MODULE_FUTABA_SLOTS];

}__attribute__((packed, aligned(1)));

struct Futaba_Session
{
	const struct hardware_port_cfg* port;
	struct Futaba_Config* config;

	uint8_t rxBuffer[MODULE_FUTABA_BUFFER_SIZE];
	uint16_t rxPointer;

	FIFO64_t txBuffer;

	uint32_t slotHasData;

	uint8_t slotData[MODULE_FUTABA_SLOTS][MODULE_FUTABA_SLOT_DATA];
	uint8_t currentSlot;
	U64 lastFrame;

	OS_TID task_id;
	OS_STK task_stack[MODULE_FUTABA_TASK_STACK];

	struct Futaba_Session* next;
};

struct Futaba_Sensor_SBS01G
{
	uint32_t UTC :17;
	uint32_t Latitude :26;
	uint8_t LatitudeSign :1;
	uint32_t Longitude :27;
	uint8_t LongitudeSign :1;
	uint16_t Speed :9;
	uint8_t GPSFix :1;
	uint8_t GPSStrength :2; //84
	uint16_t Unknown :12;
	uint32_t AltitudeU :6;
	uint32_t Altitude :13;
	uint8_t VarioU :3;
	uint16_t Vario :9;
	uint8_t BaroStatus :1;
}__attribute__((packed, aligned(1)));

struct Futaba_Sensor_CURR1678
{
	uint8_t CurrentSign:1;
	uint8_t DataValid:1;
	uint16_t Current:14;
	uint16_t Voltage:16;
	int16_t Capacity:16;
}__attribute__((packed, aligned(1)));

static struct Futaba_Session* futaba_sessions;

static const uint8_t Futaba_Slots[MODULE_FUTABA_SLOTS] = { 0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3,
		0x33, 0xB3, 0x73, 0xF3, 0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB };

void module_futaba_task(void* pData);

static void module_futaba_rx_callback(uint8_t* id);
static void module_futaba_tx_callback(uint8_t* id);
static void module_futaba_tim_callback(uint8_t* id);

#endif /* MODULE_FUTABA_PRIV_H_ */
