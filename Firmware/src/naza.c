/*
 * naza.c
 *
 *  Created on: 25.08.2014
 *      Author: cem
 *
 * CAN ID 0x7F8 MSG ID 0x0921 LENGTH 0x000C - GPS Module Version________NOT USED
 * CAN ID 0x7F8 MSG ID 0x0922 LENGTH 0x0000 - PMU Heartbeat ?!
 * CAN ID 0x7F8 MSG ID 0x1003 LENGTH 0x003A - GPS DATA
 *
 * CAN ID 0x118 MSG ID 0x1004 LENGTH 0x0006 - Compass DATA______________NOT USED
 *
 * CAN ID 0x108 MSG ID 0x1000 LENGTH 0x0034 - TBD_______________________NOT USED
 * CAN ID 0x108 MSG ID 0x1007 LENGTH 0x0000 - OSD Heartbeat ?!
 * CAN ID 0x108 MSG ID 0x1009 LENGTH 0x00B8 - RAW IO Data
 *
 * CAN ID 0x090 MSG ID 0x1002 LENGTH 0x0078 - OSD Data Message
 * CAN ID 0x090 MSG ID 0x1001 LENGTH 0x000E - RAW Gyro Data_____________NOT USED
 */

#include "naza_priv.h"

void naza_initialize()
{
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
	def.NVIC_IRQChannelPriority = 0;
	NVIC_Init(&def);
}

void naza_start()
{

	naza_msg_box_id = CoCreateQueue(naza_msg_queue, CHANNEL_COUNT,
	EVENT_SORT_TYPE_PRIO);

	assert_param(naza_msg_box_id != E_CREATE_FAIL);

	naza_main_task_id = CoCreateTask(naza_main_task, NULL, 0, &naza_main_task_stk[MAIN_TASK_STACK_SIZE- 1], MAIN_TASK_STACK_SIZE);

	assert_param(naza_main_task_id != E_CREATE_FAIL);

	CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);
	CAN_ITConfig(CAN, CAN_IT_FOV0, ENABLE);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
//Naza main task... Handling incoming messages
static void naza_main_task(void* pData)
{

	StatusType status;
	struct naza_channel* channel;

	U64 nextOSDHeartbeat = CoGetOSTime();
	U64 tmpTick = 0;

	CanTxMsg Heartbeat1 = { 0x108, 0, CAN_Id_Standard, CAN_RTR_Data, 8, { 0x55, 0xAA, 0x55, 0xAA, 0x07, 0x10, 0x00, 0x00 } };
	CanTxMsg Heartbeat2 = { 0x108, 0, CAN_Id_Standard, CAN_RTR_Data, 4, { 0x66, 0xCC, 0x66, 0xCC } };

	float cosRoll = 0;
	float sinRoll = 0;
	float cosPitch = 0;
	float sinPitch = 0;
	float magCalX = 0;
	float magCalY = 0;
	float magCalZ = 0;
	float magCompX = 0;
	float magCompY = 0;

	float nVel = 0;
	float eVel = 0;

	float ndop = 0;
	float edop = 0;

	while (1)
	{

		channel = CoPendQueueMail(naza_msg_box_id, delay_ms(200), &status);

		tmpTick = CoGetOSTime();
		if (tmpTick < simpleTelemtryData.lastHeartbeat + delay_sec(3))
		{ //PMU available

			if (tmpTick > nextOSDHeartbeat)
			{ //No OSD available, fake OSD Heartbeat

				CAN_Transmit(CAN, &Heartbeat1);
				CAN_Transmit(CAN, &Heartbeat2);

				nextOSDHeartbeat = tmpTick + delay_sec(2); //next heartbeat in 2 sec
			}
		}

		if (status == E_OK)
		{

			if (channel->msg.header.id == 0x1002)
			{ //osd
				struct msg_osd* osd = (struct msg_osd*) &channel->msg.bytes;

				cosRoll = cosf(simpleTelemtryData.roll);
				sinRoll = sinf(simpleTelemtryData.roll);
				cosPitch = cosf(simpleTelemtryData.pitch);
				sinPitch = sinf(simpleTelemtryData.pitch);
				magCalX = osd->magCalX;
				magCalY = osd->magCalY;
				magCalZ = osd->magCalZ;
				magCompX = magCalX * cosPitch + magCalZ * sinPitch;
				magCompY = magCalX * sinRoll * sinPitch + magCalY * cosRoll - magCalZ * sinRoll * cosPitch;

				simpleTelemtryData.headingNc = -(atan2f(magCalY, magCalX) / M_PI * 180.0);
				if (simpleTelemtryData.headingNc < 0)
					simpleTelemtryData.headingNc += 360;
				simpleTelemtryData.heading = -(atan2f(magCompY, magCompX) / M_PI * 180.0);
				if (simpleTelemtryData.heading < 0)
					simpleTelemtryData.heading += 360;

				simpleTelemtryData.numSat = osd->numSat;
				simpleTelemtryData.gpsAlt = osd->altGps;
				simpleTelemtryData.lat = osd->lat / M_PI * 180.0;
				simpleTelemtryData.lon = osd->lon / M_PI * 180.0;
				simpleTelemtryData.alt = osd->altBaro;

				nVel = osd->northVelocity;
				eVel = osd->eastVelocity;

				simpleTelemtryData.speed = sqrtf(nVel * nVel + eVel * eVel);
				simpleTelemtryData.cog = (atan2f(eVel, nVel) / M_PI * 180);
				if (simpleTelemtryData.cog < 0)
					simpleTelemtryData.cog += 360;

				simpleTelemtryData.vsi = -osd->downVelocity;

			}
			else if (channel->msg.header.id == 0x1003)
			{ //gps

				struct msg_gps* gps = (struct msg_gps*) &channel->msg.bytes;

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
				simpleTelemtryData.vdop = (float) gps->vdop / 100;
				ndop = (float) gps->ndop / 100;
				edop = (float) gps->edop / 100;
				simpleTelemtryData.hdop = sqrtf(ndop * ndop + edop * edop);

				switch (gps->fixType)
				{
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
				if ((simpleTelemtryData.fixType != fixType_No) && (gps->fixStatus & 0x02))
					simpleTelemtryData.fixType = fixType_DGPS;

			}
			else if (channel->msg.header.id == 0x1009)
			{ //raw io
				struct msg_raw_io* raw_io = (struct msg_raw_io*) &channel->msg.bytes;
				simpleTelemtryData.battery = raw_io->batVolt;

				for (uint8_t j = 0; j < 10; j++)
				{
					simpleTelemtryData.rcIn[j] = raw_io->rcIn[j];
				}

				simpleTelemtryData.battery = raw_io->batVolt;
				simpleTelemtryData.roll = raw_io->roll;
				simpleTelemtryData.pitch = raw_io->pitch;
				simpleTelemtryData.mode = (enum flightMode) raw_io->flightMode;
				simpleTelemtryData.throttle = raw_io->actThroIn;

				simpleTelemtryData.homeLat = raw_io->homeLat / M_PI * 180.0;
				simpleTelemtryData.homeLon = raw_io->homeLon / M_PI * 180.0;
				simpleTelemtryData.homeAltBaro = raw_io->homeAltBaro;
			}
			else if (channel->msg.header.id == 0x0922)
			{ //PMU Heartbeat
				simpleTelemtryData.lastHeartbeat = CoGetOSTime();
			}
			else if (channel->msg.header.id == 0x1007)
			{ //osd heartbeat available
				nextOSDHeartbeat = CoGetOSTime() + delay_sec(3);
			}

			CHANNEL_FREE(channel);
		}

	}
}

#pragma GCC diagnostic pop

void CEC_CAN_IRQHandler()
{
	CoEnterISR();

	if (CAN_GetITStatus(CAN, CAN_IT_FMP0) != RESET)
	{
		//new message...
		CanRxMsg msg;
		CAN_Receive(CAN, CAN_FIFO0, &msg);

		struct naza_channel* channel = NULL;

		uint8_t byte = 0;

		for (uint8_t i = 0; i < msg.DLC; i++)
		{
			if (channel == NULL)
			{
				for (int chId = 0; chId < CHANNEL_COUNT; chId++)
				{
					if (chans[chId].state != Processing && chans[chId].id == msg.StdId)
					{
						channel = &chans[chId];
						break;
					}
				}
				if (channel == NULL)
				{
					for (int chId = 0; chId < CHANNEL_COUNT; chId++)
					{
						if (chans[chId].state == None)
						{
							channel = &chans[chId];
							CHANNEL_MAP(channel, msg.StdId);
							break;
						}
					}
				}
				if (channel == NULL) //No Channels left, drop packet...
				{
					simpleTelemtryData.packets_drop += 1;
					break;
				}
			}

			byte = msg.Data[i];

			if (channel->state == Streaming)
			{
				channel->msg.bytes[channel->buffer_position++] = byte;

				// Cannot exceed Buffer Size
				if (channel->buffer_position == CHANNEL_SIZE)
				{
					CHANNEL_CLEAR(channel);
					simpleTelemtryData.packets_corrupted += 1;
				}

				// Check if received data exceeds announced length
				if (channel->buffer_position > 3 && channel->buffer_position > channel->msg.header.length + 8)
				{
					CHANNEL_CLEAR(channel);
					simpleTelemtryData.packets_corrupted += 1;
				}
			}

			//check header
			if (byte == HEADER1)
			{
				if (channel->header == 0)
					channel->header = 1;
				else if (channel->header == 2)
					channel->header = 3;
				else
					channel->header = 0;
			}
			else if (byte == HEADER2)
			{
				if (channel->header == 1)
					channel->header = 2;
				else if (channel->header == 3)
				{
					CHANNEL_STATE(channel, Streaming);
				}
				else
					channel->header = 0;
			}
			else
				channel->header = 0;

			//check footer
			if (byte == FOOTER1)
			{
				if (channel->footer == 0)
					channel->footer = 1;
				else if (channel->footer == 2)
					channel->footer = 3;
				else
					channel->footer = 0;
			}
			else if (byte == FOOTER2)
			{
				if (channel->footer == 1)
					channel->footer = 2;
				else if (channel->footer == 3)
				{

					if (channel->buffer_position == channel->msg.header.length + 8)
					{

						if (channel->msg.header.id == 0x1002 || channel->msg.header.id == 0x1003
								|| channel->msg.header.id == 0x1009 || channel->msg.header.id == 0x1007
								|| channel->msg.header.id == 0x0922)
						{
							channel->state = Processing;

							StatusType t = isr_PostQueueMail(naza_msg_box_id, channel);

							if (t == E_QUEUE_FULL)
							{
								CHANNEL_CLEAR(channel);
								simpleTelemtryData.packets_drop += 1;
							}
							else
								channel = NULL;
						}
						else
						{
							CHANNEL_CLEAR(channel);
						}

					}
					else
					{
						CHANNEL_CLEAR(channel);
						simpleTelemtryData.packets_corrupted += 1;
					}

				}
				else
					channel->footer = 0;
			}
			else
				channel->footer = 0;
		}
	}

	if (CAN_GetITStatus(CAN, CAN_IT_FOV0) != RESET)
	{
		simpleTelemtryData.packets_lost += 1;
	}

	CoExitISR();
}

