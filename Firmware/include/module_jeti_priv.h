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

#define MODULE_JETI_LABEL_INIT		5
#define MODULE_JETI_LABEL_UPDATE	5

#define MODULE_JETI_EX_MAX_SIZE		29
#define MODULE_JETI_EX_HEADER_SIZE	8
#define MODULE_JETI_EX_CRC_SIZE		1

#define MODULE_JETI_EX_CONTENT_SIZE	MODULE_JETI_EX_MAX_SIZE - MODULE_JETI_EX_HEADER_SIZE - MODULE_JETI_EX_CRC_SIZE

#define MODULE_JETI_EX_PACKETS		15
#define MODULE_JETI_EX_LABELS		16

struct Jeti_Config
{
	uint8_t version;

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

struct Jeti_Simple_Packet
{

}__attribute__((packed, aligned(1)));

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
	uint16_t degree:13;
	uint8_t latlon:1; //lat=0; lon=1;
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
	uint8_t data[4];
}__attribute__((packed, aligned(1)));

struct Jeti_EX_Packet
{
	uint8_t length;
	struct Jeti_EX_Data data;
};

static struct Jeti_Session* jeti_sessions;


static const struct Jeti_EX_Text module_jeti_labels[MODULE_JETI_EX_LABELS] =
{														   //1234567890
	{	.id = 0, .length_desc=8, .length_unit=0, 	.labels="AnySense"},
	{	.id = 1, .length_desc=8, .length_unit=0, 	.labels="Latitude"},
	{	.id = 2, .length_desc=9, .length_unit=0, 	.labels="Longitude"},
	{	.id = 3, .length_desc=10, .length_unit=0, 	.labels="Satelliten" ""},
	{	.id = 4, .length_desc=9, .length_unit=0, 	.labels="Flugmodus"},
	{	.id = 5, .length_desc=10, .length_unit=3, 	.labels="Geschwndg." "kmh"},
	{	.id = 6, .length_desc=10, .length_unit=1, 	.labels="Entfernung" "m"},
	{	.id = 7, .length_desc=5, .length_unit=1, 	.labels="Hoehe" "m"},
	{	.id = 8, .length_desc=5, .length_unit=3, 	.labels="Vario" "m/s"},
	{	.id = 9, .length_desc=7, .length_unit=1, 	.labels="Kompass" "\xB0"},
	{	.id = 10, .length_desc=10, .length_unit=1, 	.labels="Flugricht." "\xB0"},
	{	.id = 11, .length_desc=8, .length_unit=1, 	.labels="Spannung" "V"},
	{	.id = 12, .length_desc=5, .length_unit=1, 	.labels="Strom" "A"},
	{	.id = 13, .length_desc=10, .length_unit=2, 	.labels="Kapazitaet" "Ah"},
	{	.id = 14, .length_desc=10, .length_unit=1, 	.labels="Lipo Zelle" "V"},
	{	.id = 15, .length_desc=10, .length_unit=2, 	.labels="Temp. Lipo" "\xB0" "C"},
};

static struct Jeti_EX_Packet module_jeti_packets[MODULE_JETI_EX_PACKETS] =
{
		{	.length = 5, .data = { .id = 1, .dataType = 9}}, //lat
		{	.length = 5, .data = { .id = 2, .dataType = 9}}, //lon
		{	.length = 2, .data = { .id = 3, .dataType = 0}}, //sats
		{	.length = 2, .data = { .id = 4, .dataType = 0}}, //mode
		{	.length = 3, .data = { .id = 5, .dataType = 1}}, //speed
		{	.length = 4, .data = { .id = 6, .dataType = 4}}, //distance
		{	.length = 4, .data = { .id = 7, .dataType = 4}}, //alt
		{	.length = 3, .data = { .id = 8, .dataType = 1}}, //vario
		{	.length = 3, .data = { .id = 9, .dataType = 1}}, //heading
		{	.length = 3, .data = { .id = 10, .dataType = 1}},//cog
		{	.length = 3, .data = { .id = 11, .dataType = 1}},//volt
		{	.length = 3, .data = { .id = 12, .dataType = 1}},//current
		{	.length = 3, .data = { .id = 13, .dataType = 1}},//capacity
		{	.length = 3, .data = { .id = 14, .dataType = 1}},//cell
		{	.length = 3, .data = { .id = 15, .dataType = 1}},//temp
};

void module_jeti_task(void* pData);

static void module_jeti_tim_callback(uint8_t* id);

#endif /* MODULE_JETI_PRIV_H_ */
