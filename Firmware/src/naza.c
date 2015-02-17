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
	/*
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
	 */

	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFFF;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFFFF;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;

	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x090 << 5;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 1;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x108 << 5;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 2;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x7F8 << 5;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 3;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x388 << 5;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 4;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x308 << 5;
	CAN_FilterInit(&CAN_FilterInitStructure);

	NVIC_InitTypeDef def;
	def.NVIC_IRQChannelCmd = ENABLE;
	def.NVIC_IRQChannel = CEC_CAN_IRQn;
	def.NVIC_IRQChannelPriority = PRIORITY_CAN;
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

	U64 nextOSDHeartbeat = CoGetOSTime() + delay_sec(3);
	U64 nextToggle = CoGetOSTime();
	U64 tmpTick = 0;
	U64 spdTick = 0;
	U64 saveHome = 0;

	uint8_t countFS = 0;

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

	float avg[3] = { 0 };
	uint8_t avg_curr = 0;

	enum fc_type fcType = fc_type_naza;

	uint8_t tmpFS = 0;

	while (1)
	{

		channel = CoPendQueueMail(naza_msg_box_id, delay_ms(200), &status);

		tmpTick = CoGetOSTime();
		if (tmpTick < simpleTelemtryData.lastHeartbeat + delay_sec(3))
		{ //FC available

			if (tmpTick > nextOSDHeartbeat)
			{ //No OSD available, fake OSD Heartbeat

				CAN_Transmit(CAN, &Heartbeat1);
				CAN_Transmit(CAN, &Heartbeat2);

				nextOSDHeartbeat = tmpTick + delay_sec(2); //next heartbeat in 2 sec
			}

			if (nextToggle < tmpTick)
			{
				nextToggle = tmpTick + delay_sec(1);
				hardware_led_toggle_green();
			}
		}

		if (status == E_OK)
		{
			simpleTelemtryData.lastHeartbeat = CoGetOSTime();

			if (channel->msg.header.id == 0x1002)
			{ //osd

				struct msg_osd* osd = (struct msg_osd*) &channel->msg.bytes;

				if (osd->mask != 0)
				{
					for (uint8_t i = 4; i < (osd->header.length + 4); i++)
						channel->msg.bytes[i] ^= osd->mask;
				}

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

				if (spdTick < tmpTick)
				{
					nVel = osd->northVelocity;
					eVel = osd->eastVelocity;

					avg[avg_curr++] = sqrtf(nVel * nVel + eVel * eVel);
					avg_curr = avg_curr % 3;

					simpleTelemtryData.speed = (avg[0] + avg[1] + avg[2]) / 3;

					simpleTelemtryData.cog = -(atan2f(eVel, nVel) / M_PI * 180);
					if (simpleTelemtryData.cog < 0)
						simpleTelemtryData.cog += 360;

					spdTick = tmpTick + delay_ms(200);
				}

				if (fcType == fc_type_wookong)
				{
					if (simpleTelemtryData.numSat >= 7)
					{
						if (saveHome == 0)
							saveHome = tmpTick + delay_sec(10);

						if (tmpTick > saveHome && simpleTelemtryData.armed == 0)
						{
							simpleTelemtryData.homeLat = simpleTelemtryData.lat;
							simpleTelemtryData.homeLon = simpleTelemtryData.lon;
							simpleTelemtryData.homeAltBaro = simpleTelemtryData.alt + 20;
						}
					}
					else
						saveHome = 0;

				}

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

				for (uint8_t j = 0; j < 10; j++)
				{
					simpleTelemtryData.rcIn[j] = raw_io->rcIn[j];
				}

				if (channel->id == 0x388)
					fcType = fc_type_wookong;

				if (fcType == fc_type_wookong)
				{
					if (tmpFS == 1)
					{
						simpleTelemtryData.mode = flightMode_failsafe;
					}
					else
					{
						switch (raw_io->controlMode)
						{
						case 3:
							simpleTelemtryData.mode = flightMode_manual;
							break;
						case 6:
							simpleTelemtryData.mode = flightMode_atti;
							break;
						default:
							simpleTelemtryData.mode = flightMode_gps;
							break;
						}
					}
				}
				else
				{
					simpleTelemtryData.homeLat = raw_io->homeLat / M_PI * 180.0;
					simpleTelemtryData.homeLon = raw_io->homeLon / M_PI * 180.0;
					simpleTelemtryData.homeAltBaro = raw_io->homeAltBaro;

					simpleTelemtryData.mode = raw_io->flightMode;
				}

				if (fcType != fc_type_phantom)
				{
					simpleTelemtryData.battery = raw_io->batVolt;
				}

				if (fcType == fc_type_naza)
				{
					simpleTelemtryData.roll = raw_io->roll;
					simpleTelemtryData.pitch = raw_io->pitch;
				}
				else
				{
					simpleTelemtryData.roll = -(raw_io->stabRollIn * ((M_PI / 2) / 1000.0));
					simpleTelemtryData.pitch = -(raw_io->stabPitchIn * ((M_PI / 2) / 1000.0));
				}

				simpleTelemtryData.armed = raw_io->armed;
				simpleTelemtryData.throttle = raw_io->actThroIn;
			}
			else if (channel->msg.header.id == 0x0926)
			{
				struct msg_smartBattery* smart = (struct msg_smartBattery*) &channel->msg.bytes;
				simpleTelemtryData.battery = smart->voltage;
				simpleTelemtryData.cellCount = 3;
				simpleTelemtryData.cells[0] = smart->cell1;
				simpleTelemtryData.cells[1] = smart->cell2;
				simpleTelemtryData.cells[2] = smart->cell3;
				simpleTelemtryData.current = -(((float) smart->current) / 1000.0f);
				simpleTelemtryData.temp1 = ((float) smart->temperature) / 10.0f;
				simpleTelemtryData.capacity_current = smart->capacity_current;
				simpleTelemtryData.percentage_charge = smart->percentage_charge;

				fcType = fc_type_phantom;

			}
			else if (channel->msg.header.id == 0x0909)
			{
				if (channel->msg.bytes[4] == 0x00 && channel->msg.bytes[5] == 0x00 && channel->msg.bytes[6] == 0x00
						&& channel->msg.bytes[7] == 0x00 && channel->msg.bytes[8] == 0xFF && channel->msg.bytes[9] == 0x03)
				{
					//Failsafe
					tmpFS = 1;
					countFS = 0;
				}
				else
				{
					if (countFS > 10)
					{
						tmpFS = 0;
					}
					else
						countFS++;
				}
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
								|| channel->msg.header.id == 0x0926 || channel->msg.header.id == 0x0909)
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

