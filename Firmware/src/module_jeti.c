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
		cfg->values[0] = Jeti_Telemetry_Value_Latitude;
		cfg->values[1] = Jeti_Telemetry_Value_Longitude;
		cfg->values[2] = Jeti_Telemetry_Value_Satellite;
		cfg->values[3] = Jeti_Telemetry_Value_GPS_Fix;
		cfg->values[4] = Jeti_Telemetry_Value_Speed;
		cfg->values[5] = Jeti_Telemetry_Value_Distance;
		cfg->values[6] = Jeti_Telemetry_Value_Altitude;
		cfg->values[7] = Jeti_Telemetry_Value_VSpeed;
		cfg->values[8] = Jeti_Telemetry_Value_Compass;
		cfg->values[9] = Jeti_Telemetry_Value_HomeDirection;
		cfg->values[10] = Jeti_Telemetry_Value_Voltage;
		cfg->values[11] = Jeti_Telemetry_Value_Current;
		cfg->values[12] = Jeti_Telemetry_Value_Capacity;
		cfg->values[13] = Jeti_Telemetry_Value_LipoVoltage;
		cfg->values[14] = Jeti_Telemetry_Value_LipoTemp;

		cfg->alarms[0].alarmType = Jeti_Alarm_Type_Altitude;
		cfg->alarms[0].minMaxValue = 500;
		cfg->alarms[0].event = 'A';

		cfg->alarms[1].alarmType = Jeti_Alarm_Type_Distance;
		cfg->alarms[1].minMaxValue = 2000;
		cfg->alarms[1].event = 'D';

		cfg->alarms[2].alarmType = Jeti_Alarm_Type_Battery;
		cfg->alarms[2].minMaxValue = 11500;
		cfg->alarms[2].event = 'U';

		cfg->alarms[3].alarmType = Jeti_Alarm_Type_Capacity;
		cfg->alarms[3].minMaxValue = 4500;
		cfg->alarms[3].event = 'I';
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

	U64 tick = CoGetOSTime();
	U64 initPhase = tick + delay_sec(MODULE_JETI_LABEL_INIT);
	U64 nextLabel = tick;

	uint8_t ledEnabled = session->port == &usart_port1;
	U64 next_toggle = tick - 1;

	uint8_t currentPacket = 0;
	uint8_t currentLabel = 0;
	uint8_t dataLength = 0;
	uint8_t packetStarted = 0;

	U64 nextAlarm = tick + delay_sec(MODULE_JETI_ALRM_DELAY);
	uint8_t currentAlarm = 0;
	uint8_t alarmCode = 0;
	uint8_t alarmStarted = 0;

	struct Jeti_Alarm_Config* alarms[MODULE_JETI_ALRM_MAX_TYPE][MODULE_JETI_ALRM_COUNT];
	memset(alarms, 0x00, sizeof(alarms));

	for (uint8_t type = 0; type < MODULE_JETI_ALRM_MAX_TYPE; type++)
	{
		uint8_t tmp = 0;
		for (uint8_t alrm = 0; alrm < MODULE_JETI_ALRM_COUNT; alrm++)
		{
			if (session->config->alarms[alrm].alarmType == (type + 1))
				alarms[type][tmp++] = &session->config->alarms[alrm];
		}

		qsort(alarms[type], tmp, sizeof(struct Jeti_Alarm_Config*), &jeti_cmp_alarm);
	}

	/*
	 struct Jeti_Alarm_Config test = { .minMaxValue = 2, .alarmType = Jeti_Alarm_Type_Altitude, .event = 'A' };
	 struct Jeti_Alarm_Config test2 = { .minMaxValue = 1, .alarmType = Jeti_Alarm_Type_Distance, .event = 'D' };

	 alarms[Jeti_Alarm_Type_Altitude - 1][0] = &test;
	 alarms[Jeti_Alarm_Type_Distance - 1][0] = &test2;
	 */

	uint8_t simpleText[32] = { 0x00 };

	uint8_t crc = 0;

//01234567890123456789012345678901
	snprintf((char *) &simpleText, 32, "    AnySense        FW %d.%d.%d", (uint8_t) (FIRMWARE_VERSION >> 16),
			(uint8_t) (FIRMWARE_VERSION >> 8), (uint8_t) FIRMWARE_VERSION);

	struct Jeti_EX_Header header = { .seperator = MODULE_JETI_EX_START_BYTE, .exId = 0x9F, .manufactureId = 0xA4CE, .deviceId =
	U_ID_1 };

	uint8_t i, j = 0;
	int8_t tmp8 = 0;
	int16_t tmp16 = 0;
	int32_t tmp32 = 0;
	double tmpDouble = 0;

	struct Jeti_EX_Data ex_data;

	while (1)
	{
		tick = CoGetOSTime();

		if (ledEnabled && tick > next_toggle)
		{
			hardware_led_toggle_red();
			next_toggle = tick + delay_ms(200);
		}

		alarmCode = 0;

		if (nextAlarm < tick)
		{
			nextAlarm = tick + delay_sec(MODULE_JETI_ALRM_CHECK_RATE);

			alarmStarted = currentAlarm;

			do
			{
				tmp32 = 0;
				tmp8 = 0; //0=deceed, 1=exceed compare
				switch (currentAlarm + 1)
				{
				case Jeti_Alarm_Type_Altitude:
					tmp32 = simpleTelemetry_getRelativeHeight();
					tmp8 = 1;
					break;
				case Jeti_Alarm_Type_Battery:
					tmp32 = simpleTelemtryData.battery;
					tmp8 = 0;
					break;
				case Jeti_Alarm_Type_Capacity:
					tmp32 = simpleTelemtryData.capacity_current;
					tmp8 = 1;
					break;
				case Jeti_Alarm_Type_Distance:
					tmp32 = simpleTelemetry_getDistance();
					tmp8 = 1;
					break;
				}

				for (i = 0; i < MODULE_JETI_ALRM_COUNT; i++)
				{
					if (alarms[currentAlarm][i] == NULL)
						break;

					if (tmp8 == 0) //deceed value?
					{
						if (tmp32 < alarms[currentAlarm][i]->minMaxValue)
							alarmCode = alarms[currentAlarm][i]->event;
					}
					else // exceed
					{
						if (tmp32 > alarms[currentAlarm][i]->minMaxValue)
							alarmCode = alarms[currentAlarm][i]->event;
					}

					if (alarmCode != 0)
						break;
				}

				currentAlarm = (currentAlarm + 1) % MODULE_JETI_ALRM_MAX_TYPE;
			} while (currentAlarm != alarmStarted && alarmCode == 0);
		}

		header.length = 0;
		memset(header.data, 0x00, MODULE_JETI_EX_CONTENT_SIZE);

		//Fill next EX Content
		if (nextLabel < tick || initPhase > tick)
		{
			// EX Text
			header.type = 0;

			const struct Jeti_Telemetry_Text text = module_jeti_telemetry_labels[
					currentLabel == 0 ? 0 : session->config->values[currentLabel - 1]];

			struct Jeti_EX_Text* label = (struct Jeti_EX_Text*) header.data;
			label->id = currentLabel;
			label->length_desc = strlen((const char*) text.name);
			label->length_unit = strlen((const char*) text.unit);

			memcpy(label->labels, text.name, label->length_desc);
			memcpy(&label->labels[label->length_desc], text.unit, label->length_unit);

			header.length = label->length_desc + label->length_unit + 2;
			currentLabel = (currentLabel + 1) % MODULE_JETI_EX_LABELS;
			nextLabel = tick + delay_sec(MODULE_JETI_LABEL_UPDATE);
		}
		else if (alarmCode == 0)
		{
			// EX Data
			header.type = 1;
			packetStarted = currentPacket;
			memset(&ex_data, 0x00, sizeof(ex_data));

			do
			{
				dataLength = 0;

				enum Jeti_Telemetry_Value tv = session->config->values[currentPacket];

				ex_data.id = currentPacket + 1;

				switch (tv)
				{
				case Jeti_Telemetry_Value_Latitude: //Lat
					if (simpleTelemetry_validGPS())
					{
						ex_data.dataType = 9;
						dataLength = 5;

						tmpDouble = simpleTelemtryData.lat;
						ex_data.data.t_gps.latlon = 0;
						ex_data.data.t_gps.ns_ew = tmpDouble < 0 ? 1 : 0;

						if (tmpDouble < 0)
							tmpDouble *= -1;

						tmp16 = tmpDouble;
						ex_data.data.t_gps.degree = tmp16;
						tmp16 = (tmpDouble - tmp16) * 60000;
						ex_data.data.t_gps.minutes = tmp16;
					}
					break;
				case Jeti_Telemetry_Value_Longitude: //lon
					if (simpleTelemetry_validGPS())
					{
						ex_data.dataType = 9;
						dataLength = 5;

						tmpDouble = simpleTelemtryData.lon;
						ex_data.data.t_gps.latlon = 1;
						ex_data.data.t_gps.ns_ew = tmpDouble < 0 ? 1 : 0;

						if (tmpDouble < 0)
							tmpDouble *= -1;

						tmp16 = tmpDouble;
						ex_data.data.t_gps.degree = tmp16;
						tmp16 = (tmpDouble - tmp16) * 60000;
						ex_data.data.t_gps.minutes = tmp16;
					}
					break;
				case Jeti_Telemetry_Value_Satellite: //sat
					ex_data.dataType = 0;
					dataLength = 2;

					ex_data.data.t_u6.sign = 0;
					ex_data.data.t_u6.decimalpoint = 0;
					ex_data.data.t_u6.data = simpleTelemtryData.numSat;
					break;
				case Jeti_Telemetry_Value_GPS_Fix: //gps fix
					ex_data.dataType = 0;
					dataLength = 2;

					ex_data.data.t_u6.sign = 0;
					ex_data.data.t_u6.decimalpoint = 0;
					ex_data.data.t_u6.data = simpleTelemtryData.fixType;
					break;
				case Jeti_Telemetry_Value_Flightmode: //mode
					ex_data.dataType = 0;
					dataLength = 2;

					ex_data.data.t_u6.sign = 0;
					ex_data.data.t_u6.decimalpoint = 0;
					ex_data.data.t_u6.data = simpleTelemtryData.mode;
					break;
				case Jeti_Telemetry_Value_Speed: //speed
					if (simpleTelemetry_validGPS())
					{
						ex_data.dataType = 1;
						dataLength = 3;

						tmp16 = simpleTelemtryData.speed * 36;
						ex_data.data.t_u14.sign = tmp16 < 0 ? 1 : 0;
						ex_data.data.t_u14.decimalpoint = 1;
						ex_data.data.t_u14.data = tmp16;
					}
					break;
				case Jeti_Telemetry_Value_Distance: //distance
					if (simpleTelemetry_validGPS() && simpleTelemetry_isHomePointSet())
					{
						ex_data.dataType = 4;
						dataLength = 4;

						tmp32 = simpleTelemetry_getDistance() * 100;
						ex_data.data.t_u22.sign = tmp32 < 0 ? 1 : 0;
						ex_data.data.t_u22.decimalpoint = 2;
						ex_data.data.t_u22.data = tmp32;
					}
					break;
				case Jeti_Telemetry_Value_Altitude: //alt
				{
					ex_data.dataType = 4;
					dataLength = 4;

					tmp32 = simpleTelemetry_getRelativeHeight() * 100;
					ex_data.data.t_u22.sign = tmp32 < 0 ? 1 : 0;
					ex_data.data.t_u22.decimalpoint = 2;
					ex_data.data.t_u22.data = tmp32;
				}
					break;
				case Jeti_Telemetry_Value_VSpeed: //vsi
				{
					ex_data.dataType = 1;
					dataLength = 3;

					tmp16 = simpleTelemtryData.vsi * 100;
					ex_data.data.t_u14.sign = tmp16 < 0 ? 1 : 0;
					ex_data.data.t_u14.decimalpoint = 2;
					ex_data.data.t_u14.data = tmp16;
				}
					break;
				case Jeti_Telemetry_Value_Compass: //heading
				{
					ex_data.dataType = 1;
					dataLength = 3;

					tmp16 = simpleTelemtryData.heading * 10;
					ex_data.data.t_u14.sign = tmp16 < 0 ? 1 : 0;
					ex_data.data.t_u14.decimalpoint = 1;
					ex_data.data.t_u14.data = tmp16;
				}
					break;
				case Jeti_Telemetry_Value_COG: //cog
					if (simpleTelemetry_validGPS())
					{
						ex_data.dataType = 1;
						dataLength = 3;

						tmp16 = simpleTelemtryData.cog * 10;
						ex_data.data.t_u14.sign = tmp16 < 0 ? 1 : 0;
						ex_data.data.t_u14.decimalpoint = 1;
						ex_data.data.t_u14.data = tmp16;
					}
					break;
				case Jeti_Telemetry_Value_HomeDirection: //cog
					if (simpleTelemetry_validGPS() && simpleTelemetry_isHomePointSet())
					{
						ex_data.dataType = 1;
						dataLength = 3;

						tmp16 = simpleTelemetry_calculateAngle() * 10;
						ex_data.data.t_u14.sign = tmp16 < 0 ? 1 : 0;
						ex_data.data.t_u14.decimalpoint = 1;
						ex_data.data.t_u14.data = tmp16;
					}
					break;
				case Jeti_Telemetry_Value_Voltage: //voltage
				{
					ex_data.dataType = 1;
					dataLength = 3;

					tmp16 = simpleTelemtryData.battery / 10;
					ex_data.data.t_u14.sign = tmp16 < 0 ? 1 : 0;
					ex_data.data.t_u14.decimalpoint = 2;
					ex_data.data.t_u14.data = tmp16;
				}
					break;
				case Jeti_Telemetry_Value_Current: //current
				{
					ex_data.dataType = 1;
					dataLength = 3;

					tmp16 = simpleTelemtryData.current * 10;
					ex_data.data.t_u14.sign = tmp16 < 0 ? 1 : 0;
					ex_data.data.t_u14.decimalpoint = 1;
					ex_data.data.t_u14.data = tmp16;
				}
					break;
				case Jeti_Telemetry_Value_Capacity: //capa
				{
					ex_data.dataType = 1;
					dataLength = 3;

					tmp16 = simpleTelemtryData.capacity_current / 10;
					ex_data.data.t_u14.sign = tmp16 < 0 ? 1 : 0;
					ex_data.data.t_u14.decimalpoint = 2;
					ex_data.data.t_u14.data = tmp16;
				}
					break;
				case Jeti_Telemetry_Value_LipoVoltage: //cell
				{
					ex_data.dataType = 1;
					dataLength = 3;

					tmp16 = simpleTelemetry_getLowestCell(0) / 10;
					ex_data.data.t_u14.sign = tmp16 < 0 ? 1 : 0;
					ex_data.data.t_u14.decimalpoint = 2;
					ex_data.data.t_u14.data = tmp16;
				}
					break;
				case Jeti_Telemetry_Value_LipoTemp: //cell temp.
				{
					ex_data.dataType = 1;
					dataLength = 3;

					ex_data.data.t_u14.sign = simpleTelemtryData.temp1 < 0 ? 1 : 0;
					ex_data.data.t_u14.decimalpoint = 1;
					ex_data.data.t_u14.data = simpleTelemtryData.temp1;
				}
					break;
				case Jeti_Telemetry_Value_Home_Latitude: //Lat
					if (simpleTelemetry_isHomePointSet())
					{
						ex_data.dataType = 9;
						dataLength = 5;

						tmpDouble = simpleTelemtryData.homeLat;
						ex_data.data.t_gps.latlon = 0;
						ex_data.data.t_gps.ns_ew = tmpDouble < 0 ? 1 : 0;

						if (tmpDouble < 0)
							tmpDouble *= -1;

						tmp16 = tmpDouble;
						ex_data.data.t_gps.degree = tmp16;
						tmp16 = (tmpDouble - tmp16) * 60000;
						ex_data.data.t_gps.minutes = tmp16;
					}
					break;
				case Jeti_Telemetry_Value_Home_Longitude: //lon
					if (simpleTelemetry_isHomePointSet())
					{
						ex_data.dataType = 9;
						dataLength = 5;

						tmpDouble = simpleTelemtryData.homeLon;
						ex_data.data.t_gps.latlon = 1;
						ex_data.data.t_gps.ns_ew = tmpDouble < 0 ? 1 : 0;

						if (tmpDouble < 0)
							tmpDouble *= -1;

						tmp16 = tmpDouble;
						ex_data.data.t_gps.degree = tmp16;
						tmp16 = (tmpDouble - tmp16) * 60000;
						ex_data.data.t_gps.minutes = tmp16;
					}
					break;
				default:
					dataLength = 0;
					break;
				}

				if (header.length + dataLength > MODULE_JETI_EX_CONTENT_SIZE)
					break;

				memcpy(&header.data[header.length], &ex_data, dataLength);

				header.length += dataLength;
				currentPacket = (currentPacket + 1) % MODULE_JETI_EX_PACKETS;

			} while (currentPacket != packetStarted);

		}

		if (alarmCode != 0)
		{
			FIFO64_write(session->txBuffer, MODULE_JETI_EX_START_BYTE);
			FIFO64_write(session->txBuffer, 0x92);
			FIFO64_write(session->txBuffer, 0x23);
			FIFO64_write(session->txBuffer, alarmCode);

			session->lastBit = (0xFFFFFFFFFFFFFFFF) ^ ((uint64_t) (1 | (1 << 4) | ((uint64_t) 1 << (4 + 33))));
		}
		else
		{
			crc = 0;
			header.length += 6;
			for (i = 0; i < (header.length + 2); i++)
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

			FIFO64_write(session->txBuffer, crc);

			session->lastBit = (0xFFFFFFFFFFFFFFFF)
					^ ((uint64_t) (1 | (1 << (header.length + 3)) | ((uint64_t) 1 << (header.length + 3 + 33))));
		}

		//SimpleText part
		FIFO64_write(session->txBuffer, 0xFE);
		for (i = 0; i < 32; i++)
			FIFO64_write(session->txBuffer, simpleText[i]);
		FIFO64_write(session->txBuffer, 0xFF);

		if (session->pos == 0)
		{
			session->currentData = FIFO64_read(session->txBuffer);
			TIM_Cmd(session->port->timer.tim, ENABLE);
		}

		CoWaitForSingleFlag(session->flag, 0);
		CoTickDelay(delay_ms(MODULE_JETI_EX_DELAY));
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

int jeti_cmp_alarm(const void * a, const void * b)
{
	struct Jeti_Alarm_Config* av1 = (struct Jeti_Alarm_Config*) *((struct Jeti_Alarm_Config**) a);
	struct Jeti_Alarm_Config* av2 = (struct Jeti_Alarm_Config*) *((struct Jeti_Alarm_Config**) b);

	if (av1 == NULL || av1->alarmType == Jeti_Alarm_Type_None)
		return 1;

	if (av2 == NULL || av2->alarmType == Jeti_Alarm_Type_None)
		return -1;

	switch (av1->alarmType)
	{
	case Jeti_Alarm_Type_Altitude:
		return av2->minMaxValue - av1->minMaxValue;
	case Jeti_Alarm_Type_Battery:
		return av1->minMaxValue - av2->minMaxValue;
	case Jeti_Alarm_Type_Capacity:
		return av2->minMaxValue - av1->minMaxValue;
	case Jeti_Alarm_Type_Distance:
		return av2->minMaxValue - av1->minMaxValue;
	default:
		return 1;
	}


	return 0;
}
