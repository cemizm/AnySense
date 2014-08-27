/*
 * naza.c
 *
 *  Created on: 25.08.2014
 *      Author: cem
 */

#include "naza_priv.h"

void naza_initialize() {
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;

	/* Enable Clocks */
	NAZA_CAN_GPIO_RX_ClockCmd(NAZA_CAN_GPIO_RX_Clock, ENABLE);
	NAZA_CAN_GPIO_TX_ClockCmd(NAZA_CAN_GPIO_TX_Clock, ENABLE);

	/* Alternate function configuration */
	GPIO_PinAFConfig(NAZA_CAN_GPIO_RX_Port, NAZA_CAN_GPIO_RX_AF_Pin,
	NAZA_CAN_GPIO_RX_AF);
	GPIO_PinAFConfig(NAZA_CAN_GPIO_TX_Port, NAZA_CAN_GPIO_TX_AF_Pin,
	NAZA_CAN_GPIO_TX_AF);

	/* GPIO Initialization */
	/* RX */
	GPIO_InitStructure.GPIO_Pin = NAZA_CAN_GPIO_RX_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(NAZA_CAN_GPIO_RX_Port, &GPIO_InitStructure);
	/* TX */
	GPIO_InitStructure.GPIO_Pin = NAZA_CAN_GPIO_TX_Pin;
	GPIO_Init(NAZA_CAN_GPIO_TX_Port, &GPIO_InitStructure);

	/* Enable CAN clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, ENABLE);

	/* CAN register init */
	CAN_DeInit(CAN);
	CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = ENABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;

	/* CAN Baudrate = 1MBps (CAN clocked at 48 MHz) */
	CAN_InitStructure.CAN_BS1 = CAN_BS1_11tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;
	CAN_InitStructure.CAN_Prescaler = 3;
	CAN_Init(CAN, &CAN_InitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	NVIC_InitTypeDef def;
	def.NVIC_IRQChannelCmd = ENABLE;
	def.NVIC_IRQChannel = CEC_CAN_IRQn;
	def.NVIC_IRQChannelPriority = 3;
	NVIC_Init(&def);

	nextPMUCheck = 0;
	nextOSDCheck = 0;

	heartbeat_timer = Timer_RegisterEx(&naza_heartbeat, 2000, 100);

	CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);
}

//TODO: adjust heartbeat time if receive heartbeat from osd
//heartbeat_timer->nextCallback = Timer_millis() + 2000;
static void naza_heartbeat() {

	DEBUG_TOGGLE_RED();

	unsigned long millis = Timer_millis();
	if (millis > nextPMUCheck) { //PMU HeartBeat 3 sec ago, naza disconnected?
		Timer_Adjust(heartbeat_timer, 1500);  //try again in 1,5 sec
		nextPMUCheck = millis; //take account of arithmetic overflow
		return; //no need for heartbeat
	}

	CAN_Transmit(CAN, (CanTxMsg*) &Heartbeat1);
	CAN_Transmit(CAN, (CanTxMsg*) &Heartbeat2);

}

static void naza_process(CanRxMsg* msg) {

	CAN_ITConfig(CAN, CAN_IT_FMP0, DISABLE);

	struct naza_channel* channel;

	LL_FOREACH(channels, channel)
	{
		if (channel->id == msg->StdId)
			break;
	}
	if (!channel) {
		channel = malloc(sizeof(struct naza_channel));

		channel->id = msg->StdId;
		CHANNEL_SET_STATE(channel, None);
		channel->next = NULL;

		LL_APPEND(channels, channel);
	}

	uint8_t byte = 0;

	for (uint8_t i = 0; i < msg->DLC; i++) {

		byte = msg->Data[i];

		if (channel->state == Streaming) {
			CHANNEL_BUFFER(channel, byte);

			// Cannot exceed Buffer Size
			if (channel->buffer_position == CHANNEL_SIZE) {
				CHANNEL_SET_STATE(channel, None);
			}

			// Check if received data exceeds announced length
			if (channel->buffer_position > 3
					&& channel->buffer_position
							> channel->msg.header.length + 8) {
				CHANNEL_SET_STATE(channel, None);

			}
		}

		//check header
		if (byte == HEADER1) {
			if (channel->header == 0)
				channel->header = 1;
			else if (channel->header == 2)
				channel->header = 3;
			else
				channel->header = 0;
		} else if (byte == HEADER2) {
			if (channel->header == 1)
				channel->header = 2;
			else if (channel->header == 3) {
				CHANNEL_SET_STATE(channel, Streaming);
			} else
				channel->header = 0;
		}

		//check footer
		if (byte == FOOTER1) {
			if (channel->footer == 0)
				channel->footer = 1;
			else if (channel->footer == 2)
				channel->footer = 3;
			else
				channel->footer = 0;
		} else if (byte == FOOTER2) {
			if (channel->footer == 1)
				channel->footer = 2;
			else if (channel->footer == 3) {

				if (channel->buffer_position
						== channel->msg.header.length + 8) {
					naza_process_data(channel->msg.bytes);
				}

				CHANNEL_SET_STATE(channel, None);
			} else
				channel->footer = 0;
		}
	}

	CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);
}

static void naza_process_data(uint8_t* data) {
	//TODO: Process Telemetry Data

	uint16_t* id = (uint16_t*) data;
	uint16_t* length = id + 1;
	uint8_t* bytes = (uint8_t*) (length + 1);

	if (*id == 0x1002) { //osd
		struct msg_osd* osd = (struct msg_osd*) bytes;

		float cosRoll = cos(tmpRollRad);
		float sinRoll = sin(tmpRollRad);
		float cosPitch = cos(tmpPitchRad);
		float sinPitch = sin(tmpPitchRad);
		float magCalX = osd->magCalX;
		float magCalY = osd->magCalY;
		float magCalZ = osd->magCalZ;
		float magCompX = magCalX * cosPitch + magCalZ * sinPitch;
		float magCompY = magCalX * sinRoll * sinPitch + magCalY * cosRoll
				- magCalZ * sinRoll * cosPitch;

		simpleTelemtryData.headingNc = atan2(magCalY, magCalX) / M_PI * 180.0;
		if (simpleTelemtryData.headingNc < 0)
			simpleTelemtryData.headingNc += 360.0;

		simpleTelemtryData.heading = atan2(magCompY, magCompX) / M_PI * 180.0;
		if (simpleTelemtryData.heading < 0)
			simpleTelemtryData.heading += 360.0;

		simpleTelemtryData.numSat = osd->numSat;
		simpleTelemtryData.gpsAlt = osd->altGps;
		simpleTelemtryData.lat = osd->lat / M_PI * 180.0;
		simpleTelemtryData.lon = osd->lon / M_PI * 180.0;
		simpleTelemtryData.alt = osd->altBaro;
		float nVel = osd->northVelocity;
		float eVel = osd->eastVelocity;
		simpleTelemtryData.speed = sqrt(nVel * nVel + eVel * eVel);
		simpleTelemtryData.cog = atan2(eVel, nVel) / M_PI * 180;
		if (simpleTelemtryData.cog < 0)
			simpleTelemtryData.cog += 360.0;
		simpleTelemtryData.vsi = -osd->downVelocity;

	} else if (*id == 0x1003) { //gps

		struct msg_gps* gps = (struct msg_gps*) bytes;

		uint32_t dateTime = gps->dateTime;
		simpleTelemtryData.second = dateTime & 0b00111111;
		dateTime >>= 6;
		simpleTelemtryData.minute = dateTime & 0b00111111;
		dateTime >>= 6;
		simpleTelemtryData.hour = dateTime & 0b00001111;
		dateTime >>= 4;
		simpleTelemtryData.day = dateTime & 0b00011111;
		dateTime >>= 5;
		if (simpleTelemtryData.hour > 7)
			simpleTelemtryData.day++;
		simpleTelemtryData.month = dateTime & 0b00001111;
		dateTime >>= 4;
		simpleTelemtryData.year = dateTime & 0b01111111;
		simpleTelemtryData.gpsVsi = -gps->downVelocity;
		simpleTelemtryData.vdop = (double) gps->vdop / 100;
		double ndop = (double) gps->ndop / 100;
		double edop = (double) gps->edop / 100;
		simpleTelemtryData.hdop = sqrt(ndop * ndop + edop * edop);

		switch (gps->fixType) {
		case 2:
			simpleTelemtryData.fixType = fixType_2D;
			break;
		case 3:
			simpleTelemtryData.fixType = fixType_3D;
			break;
		default:
			simpleTelemtryData.fixType = fixType_No;
			break;
		}
		if ((simpleTelemtryData.fixType != fixType_No)
				&& (gps->fixStatus & 0x02))
			simpleTelemtryData.fixType = fixType_DGPS;

	} else if (*id == 0x1009) { //raw io
		struct msg_raw_io* raw_io = (struct msg_raw_io*) bytes;
		simpleTelemtryData.battery = raw_io->batVolt;

		for (uint8_t j = 0; j < 10; j++) {
			simpleTelemtryData.rcIn[j] = raw_io->rcIn[j];
		}

		simpleTelemtryData.battery = raw_io->batVolt;
		tmpRollRad = raw_io->roll;
		tmpPitchRad = raw_io->pitch;
		simpleTelemtryData.roll = (int8_t) (tmpRollRad * 180.0 / M_PI);
		simpleTelemtryData.pitch = (int8_t) (tmpPitchRad * 180.0 / M_PI);
		simpleTelemtryData.mode = (enum flightMode) raw_io->flightMode;

	} else if (*id == 0x0922) { //PMU Heartbeat
		DEBUG_TOGGLE_GREEN();
		nextPMUCheck = Timer_millis() + 3000;
	} else if (*id == 0x1007) { //osd heartbeat available
		Timer_Adjust(heartbeat_timer, 5000); //check again in 5sec
	}

}

void CEC_CAN_IRQHandler() {
	if (CAN_GetITStatus(CAN, CAN_IT_FMP0) != RESET) {
		//new message...
		CanRxMsg msg;
		CAN_Receive(CAN, CAN_FIFO0, &msg);
		naza_process(&msg);
	}
}
