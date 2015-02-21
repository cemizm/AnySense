/*
 * module_jeti.c
 *
 *  Created on: 09.02.2015
 *      Author: cem
 */

#include "module_jeti_priv.h"

void module_jeti_initializeConfig(void* config)
{
	struct Jeti_Config* cfg = (struct Jeti_Config*) config;

	if (cfg->version != 1)
	{
		cfg->version = 1;
	}
}

void module_jeti_start(const struct hardware_port_cfg* port, uint8_t* config)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;
	NVIC_InitTypeDef NVIC_InitType;

	port->tx.clock.cmd(port->tx.clock.periph, ENABLE);

	GPIO_InitTypeDef initStr = port->tx.init;
	initStr.GPIO_Mode = GPIO_Mode_OUT;
	initStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(port->tx.gpio, &initStr);

	/* GPIO Initialization */
	GPIO_WriteBit(port->tx.gpio, port->tx.init.GPIO_Pin, Bit_RESET);
	GPIO_Init(port->tx.gpio, (GPIO_InitTypeDef *) &initStr);

	struct Jeti_Session* session = (struct Jeti_Session*) malloc(sizeof(struct Jeti_Session));

	session->port = port;
	session->next = NULL;
	session->config = (struct Jeti_Config*) config;
	session->currentData = 0;
	session->pos = 0;
	session->parity = 0;

	FIFO_init(session->txBuffer);

	LL_APPEND(jeti_sessions, session);

	/* Initialize & Enable Interrupts */
	hardware_register_callback2(port, NULL, NULL, &module_jeti_tim_callback, (uint8_t*) session);

	session->flag = CoCreateFlag(Co_TRUE, Co_FALSE);

	session->task_id = CoCreateTask(module_jeti_task, session, 1, &session->task_stack[MODULE_JETI_TASK_STACK - 1],
			MODULE_JETI_TASK_STACK);

	NVIC_InitType.NVIC_IRQChannel = port->timer.nvic_ch;
	NVIC_InitType.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitType.NVIC_IRQChannelPriority = PRIORITY_TELEMETRY;
	NVIC_Init(&NVIC_InitType);

	//Timer
	port->timer.rcc.cmd(port->timer.rcc.periph, ENABLE);

	TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBase_InitStructure.TIM_Prescaler = MODULE_JETI_TIM_PRESCALE;
	TIM_TimeBase_InitStructure.TIM_Period = MODULE_JETI_TIM_SAMPLE;
	TIM_TimeBaseInit(port->timer.tim, &TIM_TimeBase_InitStructure);

	TIM_ClearITPendingBit(port->timer.tim, TIM_IT_Update);
	TIM_ITConfig(port->timer.tim, TIM_IT_Update, ENABLE);
}

void module_jeti_task(void* pData)
{
	struct Jeti_Session* session = (struct Jeti_Session*) pData;

	uint8_t length = 0;

	U64 tick = CoGetOSTime();
	U64 initPhase = tick + delay_sec(MODULE_JETI_LABEL_INIT);
	U64 nextLabel = tick;

	uint8_t ledEnabled = session->port == &usart_port1;
	U64 next_toggle = tick - 1;

	uint8_t currentPacket = 0;
	uint8_t currentLabel = 0;
	uint8_t dataReady = 0;
	uint8_t packetStarted = 0;

	uint8_t* exContent = NULL;
	uint8_t simpleText[32] = { 0x00 };
	uint8_t exData[MODULE_JETI_EX_CONTENT_SIZE] = { 0x00 };

	uint8_t crc = 0;

	//01234567890123456789012345678901
	snprintf((char *) &simpleText, 32, "    AnySense        FW %d.%d.%d", (uint8_t) (FIRMWARE_VERSION >> 16),
			(uint8_t) (FIRMWARE_VERSION >> 8), (uint8_t) FIRMWARE_VERSION);

	struct Jeti_EX_Header header = { .seperator = 0x7E, .exId = 0x9F, .manufactureId = 0xA4CE, .deviceId = U_ID_1, };

	uint8_t i, j = 0;
	int8_t tmp8 = 0;
	int16_t tmp16 = 0;
	int32_t tmp32 = 0;
	double tmpDouble = 0;

	struct Jeti_TypeGPS* tmpGPS = NULL;
	struct Jeti_Type6_t* tmp6 = NULL;
	struct Jeti_Type14_t* tmp14 = NULL;
	struct Jeti_Type22_t* tmp22 = NULL;

	while (1)
	{
		tick = CoGetOSTime();

		length = 0;

		//Fill next EX Content
		if (nextLabel < tick || initPhase > tick)
		{
			// EX Text
			header.type = 0;
			const struct Jeti_EX_Text* label = &module_jeti_labels[currentLabel++];
			exContent = (uint8_t*) label;
			length = label->length_desc + label->length_unit + 2;

			currentLabel = currentLabel % MODULE_JETI_EX_LABELS;

			nextLabel = tick + delay_sec(MODULE_JETI_LABEL_UPDATE);
		}
		else
		{
			// EX Data
			header.type = 1;

			memset(exData, 0, MODULE_JETI_EX_CONTENT_SIZE);

			exContent = exData;

			packetStarted = currentPacket;

			while ((length + module_jeti_packets[currentPacket].length) < MODULE_JETI_EX_CONTENT_SIZE)
			{
				if (ledEnabled && tick > next_toggle)
				{
					hardware_led_toggle_red();
					next_toggle = tick + delay_ms(200);
				}

				const struct Jeti_EX_Packet* packet = &module_jeti_packets[currentPacket++];
				dataReady = 1;

				//update packet
				switch (packet->data.id)
				{
				case 1: //Lat
					if (simpleTelemetry_validGPS())
					{
						tmpGPS = (struct Jeti_TypeGPS*) packet->data.data;
						tmpDouble = simpleTelemtryData.lat;
						tmpGPS->latlon = 0;
						tmpGPS->ns_ew = tmpDouble < 0 ? 1 : 0;

						if (tmpDouble < 0)
							tmpDouble *= -1;

						tmp16 = tmpDouble;
						tmpGPS->degree = tmp16;
						tmp16 = (tmpDouble - tmp16) * 60000;
						tmpGPS->minutes = tmp16;
					}
					else
						dataReady = 0;
					break;
				case 2: //lon
					if (simpleTelemetry_validGPS())
					{
						tmpGPS = (struct Jeti_TypeGPS*) packet->data.data;
						tmpDouble = simpleTelemtryData.lon;
						tmpGPS->latlon = 1;
						tmpGPS->ns_ew = tmpDouble < 0 ? 1 : 0;

						if (tmpDouble < 0)
							tmpDouble *= -1;

						tmp16 = tmpDouble;
						tmpGPS->degree = tmp16;
						tmp16 = (tmpDouble - tmp16) * 60000;
						tmpGPS->minutes = tmp16;
					}
					else
						dataReady = 0;
					break;
				case 3: //sat
					tmp6 = (struct Jeti_Type6_t*) packet->data.data;
					tmp6->sign = 0;
					tmp6->decimalpoint = 0;
					tmp6->data = simpleTelemtryData.numSat;
					break;
				case 4: //mode
					tmp6 = (struct Jeti_Type6_t*) packet->data.data;
					tmp6->sign = 0;
					tmp6->decimalpoint = 0;
					tmp6->data = simpleTelemtryData.mode;
					break;
				case 5: //speed
					if (simpleTelemetry_validGPS())
					{
						tmp14 = (struct Jeti_Type14_t*) packet->data.data;
						tmp16 = simpleTelemtryData.speed * 36;
						tmp14->sign = tmp16 < 0 ? 1 : 0;
						tmp14->decimalpoint = 1;
						tmp14->data = tmp16;
					}
					else
						dataReady = 0;
					break;
				case 6: //distance
					if (simpleTelemetry_validGPS() && simpleTelemetry_isHomePointSet())
					{
						tmp22 = (struct Jeti_Type22_t*) packet->data.data;
						tmp32 = simpleTelemetry_getDistance() * 100;
						tmp22->sign = tmp32 < 0 ? 1 : 0;
						tmp22->decimalpoint = 2;
						tmp22->data = tmp32;
					}
					else
						dataReady = 0;
					break;
				case 7: //alt
				{
					tmp22 = (struct Jeti_Type22_t*) packet->data.data;
					tmp32 = simpleTelemetry_getRelativeHeight() * 100;
					tmp22->sign = tmp32 < 0 ? 1 : 0;
					tmp22->decimalpoint = 2;
					tmp22->data = tmp32;
				}
					break;
				case 8: //vsi
				{
					tmp14 = (struct Jeti_Type14_t*) packet->data.data;
					tmp16 = simpleTelemtryData.vsi * 100;
					tmp14->sign = tmp16 < 0 ? 1 : 0;
					tmp14->decimalpoint = 2;
					tmp14->data = tmp16;
				}
					break;
				case 9: //heading
				{
					tmp14 = (struct Jeti_Type14_t*) packet->data.data;
					tmp16 = simpleTelemtryData.heading * 10;
					tmp14->sign = tmp16 < 0 ? 1 : 0;
					tmp14->decimalpoint = 1;
					tmp14->data = tmp16;
				}
					break;
				case 10: //cog
					if (simpleTelemetry_validGPS())
					{
						tmp14 = (struct Jeti_Type14_t*) packet->data.data;
						tmp16 = simpleTelemtryData.cog * 10;
						tmp14->sign = tmp16 < 0 ? 1 : 0;
						tmp14->decimalpoint = 1;
						tmp14->data = tmp16;
					}
					else
						dataReady = 0;
					break;
				case 11: //voltage
				{
					tmp14 = (struct Jeti_Type14_t*) packet->data.data;
					tmp16 = simpleTelemtryData.battery / 10;
					tmp14->sign = tmp16 < 0 ? 1 : 0;
					tmp14->decimalpoint = 2;
					tmp14->data = tmp16;
				}
					break;
				case 12: //current
				{
					tmp14 = (struct Jeti_Type14_t*) packet->data.data;
					tmp16 = simpleTelemtryData.current * 10;
					tmp14->sign = tmp16 < 0 ? 1 : 0;
					tmp14->decimalpoint = 1;
					tmp14->data = tmp16;
				}
					break;
				case 13: //capa
				{
					tmp14 = (struct Jeti_Type14_t*) packet->data.data;
					tmp16 = simpleTelemtryData.capacity_current / 10;
					tmp14->sign = tmp16 < 0 ? 1 : 0;
					tmp14->decimalpoint = 2;
					tmp14->data = tmp16;
				}
					break;
				case 14: //cell
				{
					tmp14 = (struct Jeti_Type14_t*) packet->data.data;
					tmp16 = simpleTelemetry_getLowestCell(0) / 10;
					tmp14->sign = tmp16 < 0 ? 1 : 0;
					tmp14->decimalpoint = 2;
					tmp14->data = tmp16;
				}
					break;
				case 15: //cell temp.
				{
					tmp14 = (struct Jeti_Type14_t*) packet->data.data;
					tmp14->sign = simpleTelemtryData.temp1 < 0 ? 1 : 0;
					tmp14->decimalpoint = 1;
					tmp14->data = simpleTelemtryData.temp1;
				}
					break;
				default:
					dataReady = 0;
					break;
				}

				if (dataReady)
				{
					for (i = 0; i < packet->length; i++)
						exData[length + i] = ((uint8_t*) &packet->data)[i];

					length += packet->length;
				}

				currentPacket = currentPacket % MODULE_JETI_EX_PACKETS;

				if (currentPacket == packetStarted)
					break;
			}
		}

		crc = 0;

		//EX Header
		header.length = 6 + length;
		for (i = 0; i < sizeof(header); i++)
		{

			FIFO64_write(session->txBuffer, ((uint8_t* )&header)[i]);

			if (i >= 2)
			{
				crc ^= ((uint8_t*) &header)[i];

				for (j = 0; j < 8; j++)
				{
					crc = (crc & 0x80) ? MODULE_JETI_CRC_POLY ^ (crc << 1) : (crc << 1);
				}

			}
		}

		//EX Content
		for (i = 0; i < length; i++)
		{
			FIFO64_write(session->txBuffer, exContent[i]);

			crc ^= exContent[i];

			for (j = 0; j < 8; j++)
			{
				crc = (crc & 0x80) ? MODULE_JETI_CRC_POLY ^ (crc << 1) : (crc << 1);
			}
		}
		FIFO64_write(session->txBuffer, crc);

		//SimpleText part
		FIFO64_write(session->txBuffer, 0xFE);
		for (i = 0; i < 32; i++)
			FIFO64_write(session->txBuffer, simpleText[i]);
		FIFO64_write(session->txBuffer, 0xFF);

		session->lastBit = (0xFFFFFFFFFFFFFFFF)
				^ ((uint64_t) (1 | (1 << (header.length + 3)) | ((uint64_t) 1 << (header.length + 3 + 33))));

		if (session->pos == 0)
		{
			session->currentData = FIFO64_read(session->txBuffer);
			TIM_Cmd(session->port->timer.tim, ENABLE);
		}

		CoWaitForSingleFlag(session->flag, 0);
		CoTickDelay(delay_ms(20));
	}
}

static void module_jeti_tim_callback(uint8_t* id)
{
	struct Jeti_Session* session = (struct Jeti_Session*) id;

	//Now lets bang them out ;)

	if (session->pos == 0) //Start Bit
		GPIO_WriteBit(session->port->tx.gpio, session->port->tx.init.GPIO_Pin, Bit_RESET);
	else if (session->pos < 9)
	{
		//Data Bits
		GPIO_WriteBit(session->port->tx.gpio, session->port->tx.init.GPIO_Pin, session->currentData & Bit_SET);
		session->parity ^= (session->currentData & Bit_SET);
		session->currentData = session->currentData >> 1;
	}
	else if (session->pos < 10)
	{
		GPIO_WriteBit(session->port->tx.gpio, session->port->tx.init.GPIO_Pin, session->lastBit & Bit_SET);
		session->parity ^= (session->lastBit & Bit_SET);
		session->lastBit = session->lastBit >> 1;
	}
	else if (session->pos < 11) //Parity Bit
		GPIO_WriteBit(session->port->tx.gpio, session->port->tx.init.GPIO_Pin, session->parity ^ 0x01);
	else if (session->pos < 13) //Stop Bits
		GPIO_WriteBit(session->port->tx.gpio, session->port->tx.init.GPIO_Pin, Bit_SET);

	session->pos++;

	if (session->pos >= 13)
	{
		session->pos = 0;
		session->parity = 0;

		if (FIFO_available(session->txBuffer))
			session->currentData = FIFO64_read(session->txBuffer);
		else
		{
			TIM_Cmd(session->port->timer.tim, DISABLE);
			isr_SetFlag(session->flag);
		}
	}
}

