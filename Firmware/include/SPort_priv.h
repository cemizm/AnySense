/*
 * SPort_priv.h
 *
 *  Created on: 22.08.2014
 *      Author: cem
 */

#ifndef SPORT_PRIV_H_
#define SPORT_PRIV_H_

#include "SPort.h"
#include "utlist.h"
#include "fifo.h"
#include "simpletelemtry.h"

#include "stm32f0xx_usart.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx.h"

#include <stdlib.h>

enum SPort_State {
	WAITING_START_FRAME, WAITING_SESNOR_ID
};

#define SPORT_STARTSTOP		0x7E
#define SPORT_DATAFRAME		0x10

typedef enum {
	SENSOR_T1 = 0x0400,
	SENSOR_T2 = 0x0410,
	SENSOR_RPM = 0x0500,
	SENSOR_FUEL = 0x0600,
	SENSOR_ALT = 0x0100,
	SENSOR_VARIO = 0x0110,
	SENSOR_ACCX = 0x0700,
	SENSOR_ACCY = 0x0710,
	SENSOR_ACCZ = 0x0720,
	SENSOR_CURR = 0x0200,
	SENSOR_VFAS = 0x0210,
	SENSOR_CELLS = 0x0300,
	SENSOR_GPS_LONG_LATI = 0x0800,
	SENSOR_GPS_ALT = 0x0820,
	SENSOR_GPS_SPEED = 0x0830,
	SENSOR_GPS_COURS = 0x0840,
	SENSOR_GPS_TIME_DATE = 0x0850,
	SENSOR_BETA_VARIO = 0x8030,
	SENSOR_BETA_BARO_ALT = 0x8010,
} SensorId;

struct SPort_Packet {
	uint8_t Header;
	uint16_t Id;
	uint32_t Value;

}__attribute__((packed, aligned(1)));

struct SPort_Config {
	uint8_t sesnorId;
};

struct SPort_SessionStruct {
	const struct hardware_port_cfg* port;
	enum SPort_State currentState;

	struct SPort_Config* config;

	struct SPort_Packet tmp;

	FIFO64_t send_buffer;

	uint8_t currentValue;

	struct SPort_SessionStruct* next;
};

typedef struct SPort_SessionStruct SPortSession;

static SPortSession* sessions;



static void RX_Callback();
static void TX_Callback();
static void SendPacket(SPortSession* session, struct SPort_Packet* packet);

#endif /* SPORT_PRIV_H_ */
