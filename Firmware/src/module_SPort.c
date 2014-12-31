/*
 * Sport->c
 *
 *  Created on: 22.08.2014
 *      Author: cem
 */

#include "module_SPort_priv.h"

void sport_initializeConfig(void* conf)
{
	struct SPort_Config* scfg = (struct SPort_Config*) conf;

	if (scfg->version != SPORT_CONFIG_CURRENT_VERSION)
	{
		memcpy(conf, &sport_defaultConfig, sizeof(struct SPort_Config));
	}

}

void sport_start(const struct hardware_port_cfg* port, uint8_t* config)
{
	USART_InitTypeDef USART_InitStructure;

	port->rx.clock.cmd(port->rx.clock.periph, ENABLE);
	port->tx.clock.cmd(port->tx.clock.periph, ENABLE);

	port->clock.cmd(port->clock.periph, ENABLE);

	/* GPIO Initialization */
	GPIO_Init(port->rx.gpio, (GPIO_InitTypeDef *) &port->rx.init);
	GPIO_Init(port->tx.gpio, (GPIO_InitTypeDef *) &port->tx.init);

	/* Alternate function configuration */
	GPIO_PinAFConfig(port->rx.gpio, port->rx.pin_source, port->af);
	GPIO_PinAFConfig(port->tx.gpio, port->tx.pin_source, port->af);

	/* USART Initialization */
	USART_InitStructure.USART_BaudRate = 57600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(port->port, &USART_InitStructure);
	USART_InvPinCmd(port->port, USART_InvPin_Rx | USART_InvPin_Tx, ENABLE);
	USART_HalfDuplexCmd(port->port, ENABLE);
	USART_OverrunDetectionConfig(port->port, USART_OVRDetection_Disable);
	USART_Cmd(port->port, ENABLE);

	SPortSession* session = (SPortSession*) malloc(sizeof(SPortSession));

	session->currentState = WAITING_START_FRAME;
	session->port = port;
	session->next = NULL;
	session->config = (struct SPort_Config*) config;
	session->foundSensor = 0;
	session->currentSensor = 0;

	memset(session->foundSensors, 0, SPORT_ACTIVE_SENSORS * 6);
	memset(session->voltageSensors, 0, SPORT_ACTIVE_VOLTAGE_SENSORS * 6);

	FIFO_init(session->send_buffer);

	LL_APPEND(sessions, session);

	/* Initialize & Enable Interrupts */
	hardware_register_callback(port, &RX_Callback, &TX_Callback, (uint8_t*) session);

	NVIC_InitTypeDef def;
	def.NVIC_IRQChannelCmd = ENABLE;
	def.NVIC_IRQChannel = port->nvic_ch;
	def.NVIC_IRQChannelPriority = 1;
	NVIC_Init(&def);

	session->flag = CoCreateFlag(Co_TRUE, Co_FALSE);

	session->task_id = CoCreateTask(sport_task, session, 0, &session->task_stack[SPORT_TASK_STACK - 1], SPORT_TASK_STACK);

	USART_ITConfig(port->port, USART_IT_RXNE, ENABLE);
}

void sport_task(void* pData)
{
	SPortSession* session = (SPortSession*) pData;
	StatusType status;

	uint8_t ledEnabled = session->port == &usart_port1;

	uint8_t startValue = 0;
	uint8_t currentValue = 0;

	struct SPort_Packet packet;
	packet.Header = SPORT_DATAFRAME;

	int32_t values[6];
	uint8_t length = 0;

	U64 ticks = CoGetOSTime();
	U64 rx_seen_check = ticks + delay_sec(2);
	U64 next_toggle = ticks;

	uint16_t delayTime = 100;
	uint8_t currentSensor = 0;
	uint8_t sensorSeek = 0;

	while (1)
	{
		status = CoWaitForSingleFlag(session->flag, delay_ms(delayTime));

		ticks = CoGetOSTime();

		if (status == E_OK)
		{
			rx_seen_check = ticks + delay_ms(500);
			delayTime = 100;

			if (session->config->map[currentValue] != tv_none)
			{
				packet.Id = mapping[currentValue];

				if (length == 0)
					sport_getValue(session->config->map[currentValue], session->config->minFix, values, &length);

				if (length > 0)
				{
					packet.Value = values[length - 1];

					if (ledEnabled && ticks > next_toggle)
					{
						hardware_led_toggle_red();
						next_toggle = ticks + delay_ms(200);
					}

					short crc = 0;
					uint8_t* data = (uint8_t*) &packet;
					uint8_t byte;
					for (uint8_t i = 0; i < 7; i++)
					{
						byte = data[i];
						FIFO64_write(session->send_buffer, byte);
						crc += byte; //0-1FF
						crc += crc >> 8; //0-100
						crc &= 0x00FF;
						crc += crc >> 8; //0-0FF
						crc &= 0x00FF;
					}

					FIFO64_write(session->send_buffer, 0xFF - crc);

					USART_DirectionModeCmd(session->port->port, USART_Mode_Rx, DISABLE);
					USART_ITConfig(session->port->port, USART_IT_TXE, ENABLE);

					length--;
				}
			}

			if (length == 0)
			{
				startValue = (currentValue + 1) % SPORT_VALUES_MAX;

				while (startValue != currentValue
						&& (session->config->map[startValue] == tv_none
								|| !sport_valueReady(session->config->map[startValue], session->config->minFix)))
				{
					startValue = (startValue + 1) % SPORT_VALUES_MAX;
				}

				currentValue = startValue;
			}

		}
		else if (ticks > rx_seen_check)
		{
			//No Receiver seen last 2 sec
			delayTime = 12;
			rx_seen_check = ticks;

			if (ledEnabled && ticks > next_toggle)
			{
				hardware_led_toggle_red();
				next_toggle = ticks + delay_ms(600);
			}

			if (sensorSeek == 1 && session->sensorRespsone == 1)
			{
				session->foundSensors[session->foundSensor] = SPort_SensorIds[currentSensor];
				session->foundSensor++;

				sensorSeek = 0;
			}

			if (session->foundSensor == session->currentSensor)
			{

				uint8_t found = 1;
				do
				{
					currentSensor = (currentSensor + 1) % SPORT_SENSORS;
					found = 0;

					if (session->foundSensor > 0)
					{
						for (uint8_t i = 0; i < session->foundSensor; i++)
						{
							if (session->foundSensors[i] == SPort_SensorIds[currentSensor])
							{
								found = 1;
								break;
							}
						}
					}

				} while (found == 1);

				sensorSeek = 1;

				session->currentSensorId = SPort_SensorIds[currentSensor];

				session->currentSensor = 0;
			}
			else
			{
				session->currentSensorId = session->foundSensors[session->currentSensor];

				session->currentSensor++;
				sensorSeek = 0;
			}

			session->sensorRespsone = 0;

			FIFO64_write(session->send_buffer, SPORT_STARTSTOP);
			FIFO64_write(session->send_buffer, session->currentSensorId);

			USART_DirectionModeCmd(session->port->port, USART_Mode_Rx, DISABLE);
			USART_ITConfig(session->port->port, USART_IT_TXE, ENABLE);
		}
	}
}

void sport_getValue(enum telemetryValue val, enum fixType minFixTyp, int32_t * result, uint8_t* len)
{
	*len = 0;

	if (!sport_valueReady(val, minFixTyp))
		return;

	switch (val)
	{
	case tv_lon_lat:
		if (simpleTelemtryData.lon < 0)
			result[0] = (((abs(simpleTelemtryData.lon * 10000000)) / 100) * 6) | 0xC0000000;
		else
			result[0] = (((abs(simpleTelemtryData.lon * 10000000)) / 100) * 6) | 0x80000000;
		if (simpleTelemtryData.lat < 0)
			result[1] = (((abs(simpleTelemtryData.lat * 10000000)) / 100) * 6) | 0x40000000;
		else
			result[1] = (((abs(simpleTelemtryData.lat * 10000000)) / 100) * 6);
		*len = 2;
		break;
	case tv_alt:
		result[0] = simpleTelemtryData.alt * 100;
		*len = 1;
		break;
	case tv_gpsAlt:
		result[0] = simpleTelemtryData.gpsAlt * 100;
		*len = 1;
		break;
	case tv_speed:
		result[0] = simpleTelemtryData.speed * 1943;
		*len = 1;
		break;
	case tv_gps_fix:
		result[0] = simpleTelemtryData.fixType;
		*len = 1;
		break;
	case tv_numSat:
		result[0] = simpleTelemtryData.numSat;
		*len = 1;
		break;
	case tv_heading:
		result[0] = simpleTelemtryData.heading * 100;
		*len = 1;
		break;
	case tv_haadingNC:
		result[0] = simpleTelemtryData.headingNc * 100;
		*len = 1;
		break;
	case tv_cog:
		result[0] = simpleTelemtryData.cog * 100;
		*len = 1;
		break;
	case tv_vsi:
		result[0] = simpleTelemtryData.vsi * 100;
		*len = 1;
		break;
	case tv_gpsVsi:
		result[0] = simpleTelemtryData.gpsVsi * 100;
		*len = 1;
		break;
	case tv_hdop:
		result[0] = simpleTelemtryData.hdop * 100;
		*len = 1;
		break;
	case tv_vdop:
		result[0] = simpleTelemtryData.hdop * 100;
		*len = 1;
		break;
	case tv_pitch:
		result[0] = (simpleTelemtryData.pitch * 180.0 / M_PI) * 100;
		*len = 1;
		break;
	case tv_roll:
		result[0] = (simpleTelemtryData.roll * 180.0 / M_PI) * 100;
		*len = 1;
		break;
	case tv_battery:
		result[0] = simpleTelemtryData.battery / 10;
		*len = 1;
		break;
	case tv_throttle:
		result[0] = simpleTelemtryData.throttle;
		*len = 1;
		break;
	case tv_flight_mode:
		result[0] = simpleTelemtryData.mode;
		*len = 1;
		break;
	case tv_lastHeartbeat:
		result[0] = (CoGetOSTime() - simpleTelemtryData.lastHeartbeat) / CFG_SYSTICK_FREQ;
		*len = 1;
		break;
	case tv_gpsTime:
		result[0] = simpleTelemtryData.year << 24 | simpleTelemtryData.month << 16 | simpleTelemtryData.day << 8 | 0xff;
		result[1] = simpleTelemtryData.hour << 24 | simpleTelemtryData.minute << 16 | simpleTelemtryData.second << 8;
		*len = 2;
		break;
	case tv_alt_relative:
		result[0] = (simpleTelemtryData.alt - (simpleTelemtryData.homeAltBaro - 20)) * 100;
		*len = 1;
		break;
	case tv_current:
		result[0] = simpleTelemtryData.current * 10;
		*len = 1;
		break;
	case tv_cells:
	{
		uint8_t cs = (simpleTelemtryData.cellCount / 2) + (simpleTelemtryData.cellCount % 2);
		uint8_t cell = 0;
		uint16_t cell1 = 0;
		uint16_t cell2 = 0;
		for (uint8_t i = 0; i < cs; i++)
		{
			cell = i * 2;
			cell1 = simpleTelemtryData.cells[cell] / 10;
			if ((cell + 1) < simpleTelemtryData.cellCount)
				cell2 = simpleTelemtryData.cells[cell + 1] / 10;
			else
				cell2 = 0;

			result[i] = ((cell2 * 5) << 20) | ((cell1 * 5) << 8) | ((simpleTelemtryData.cellCount << 4) & 0xF0) | (cell & 0xF);
		}

		*len = cs;

	}
		break;
	case tv_none:
	default:
		*len = 0;
	}
}

uint8_t sport_valueReady(enum telemetryValue val, enum fixType minFixTyp)
{
	switch (val)
	{
	case tv_lon_lat:
	case tv_gpsTime:
	case tv_gpsVsi:
	case tv_cog:
	case tv_speed:
	case tv_gpsAlt:
		return simpleTelemtryData.fixType >= minFixTyp ? 1 : 0;
	default:
		return 1;
	}
}

static void RX_Callback(uint8_t* id)
{

	SPortSession* session = (SPortSession*) id;

	uint8_t read = USART_ReceiveData(session->port->port);

	switch (session->currentState)
	{
	case WAITING_START_FRAME:
		if (read == SPORT_STARTSTOP)
			session->currentState = WAITING_SESNOR_ID;
		else if (read == SPORT_DATAFRAME)
		{
			session->currentState = DATA_FRAME;
			session->rxPacket.Header = SPORT_DATAFRAME;
			session->rxPointer = 1;
		}
		break;
	case WAITING_SESNOR_ID:
		if (read == session->config->sensorId)
			isr_SetFlag(session->flag);
		session->currentState = WAITING_START_FRAME;
		break;
	case DATA_FRAME:
	{
		uint8_t* data = (uint8_t*) &session->rxPacket;

		data[session->rxPointer++] = read;
		if (session->rxPointer >= SPORT_DATA_SIZE)
		{

			short crc = 0;
			for (uint8_t i = 0; i < SPORT_DATA_SIZE; i++)
			{
				crc += data[i]; //0-1FF
				crc += crc >> 8; //0-100
				crc &= 0x00FF;
				crc += crc >> 8; //0-0FF
				crc &= 0x00FF;
			}
			if (crc == 0x00ff)
			{
				session->sensorRespsone = 1;

				switch (session->rxPacket.Id)
				{
				case SENSOR_CURR:
					simpleTelemtryData.current = ((float)SPORT_DATA_U32(session->rxPacket)) / 10;
					break;
				case SENSOR_CELLS:
				{
					uint32_t lipo = SPORT_DATA_U32(session->rxPacket);

					uint8_t startCell = 0;
					uint8_t battnumber = lipo & 0xF;
					uint8_t cells = (lipo & 0xF0) >> 4;

					for (uint8_t i = 0; i < SPORT_ACTIVE_VOLTAGE_SENSORS; i++)
					{
						if (session->voltageSensors[i].active == 1)
						{
							if (session->voltageSensors[i].SensorId == session->currentSensorId)
							{
								break;
							}
							else
								startCell += session->voltageSensors[i].payload[0];
						}
						else
						{
							session->voltageSensors[i].active = 1;
							session->voltageSensors[i].SensorId = session->currentSensorId;
							session->voltageSensors[i].payload[0] = cells;
							simpleTelemtryData.cellCount += cells;
							break;
						}
					}

					simpleTelemtryData.cells[startCell + battnumber] = (uint16_t) (((lipo & 0x000FFF00) >> 8) / 5) * 10;
					if (battnumber + 1 < cells)
						simpleTelemtryData.cells[startCell + battnumber + 1] = (uint16_t) (((lipo & 0xFFF00000) >> 20) / 5) * 10;
				}
					break;
				case SENSOR_AIR_SPEED:
					//airSpeed = SPORT_DATA_U32(packet);
					break;
				case SENSOR_T1:
					simpleTelemtryData.temp1 = SPORT_DATA_S32(session->rxPacket);
					break;
				case SENSOR_T2:
					simpleTelemtryData.temp2 = SPORT_DATA_S32(session->rxPacket);
					break;
				case SENSOR_RPM:
					//SPORT_DATA_U32(packet) / blades
					break;
				case SENSOR_FUEL:
					//SPORT_DATA_U32(packet)
					break;

				}

				if (!simpleTelemetry_isAlive())
				{
					switch (session->rxPacket.Id)
					{
					case SENSOR_ALT:
						simpleTelemtryData.alt = SPORT_DATA_S32(session->rxPacket) / 100;
						if (!SPORT_DATA_IS_FLAG_SET(session, SPORT_DATA_FLAG_ALT_SET))
						{
							simpleTelemtryData.homeAltBaro = simpleTelemtryData.alt + 20;
							SPORT_DATA_FLAG_SET(session, SPORT_DATA_FLAG_ALT_SET);
						}
						break;
					case SENSOR_VARIO:
						simpleTelemtryData.vsi = SPORT_DATA_S32(session->rxPacket) / 100;
						break;
					case SENSOR_VFAS:
						simpleTelemtryData.battery = SPORT_DATA_U32(session->rxPacket) * 10;
						break;
					case SENSOR_GPS_SPEED:
						simpleTelemtryData.speed = SPORT_DATA_U32(session->rxPacket) / 1943;
						break;
					case SENSOR_GPS_ALT:
						simpleTelemtryData.gpsAlt = SPORT_DATA_S32(session->rxPacket) / 100;
						break;
					case SENSOR_GPS_LONG_LATI:
					{
						uint32_t val = SPORT_DATA_U32(session->rxPacket);
						uint8_t sign = (val & 0xc0000000) >> 30;
						float lonlat = val & 0x0FFFFFFF;

						//(((abs(simpleTelemtryData.lat * 10000000)) / 100) * 6)

						lonlat = ((lonlat / 6) * 100) / 10000000;

						switch (sign)
						{
						case 0:
							simpleTelemtryData.lat = lonlat;
							break;
						case 1:
							simpleTelemtryData.lat = -lonlat;
							break;
						case 2:
							simpleTelemtryData.lon = lonlat;
							break;
						case 3:
							simpleTelemtryData.lon = -lonlat;
							break;
						}

						if (!SPORT_DATA_IS_FLAG_SET(session, SPORT_DATA_FLAG_GPS_HOME_SET))
						{
							if (simpleTelemtryData.lon != 0 && simpleTelemtryData.lat != 0)
							{
								simpleTelemtryData.homeLat = simpleTelemtryData.lat;
								simpleTelemtryData.homeLon = simpleTelemtryData.lon;
								SPORT_DATA_FLAG_SET(session, SPORT_DATA_FLAG_GPS_HOME_SET);
							}
						}

					}
						break;
					case SENSOR_GPS_COURS:
						simpleTelemtryData.cog = SPORT_DATA_U32(session->rxPacket) / 100;
						break;
					case SENSOR_GPS_TIME_DATE:
					{
						uint32_t gps_time_date = SPORT_DATA_U32(session->rxPacket);
						if (gps_time_date & 0x000000ff)
						{
							simpleTelemtryData.year = gps_time_date << 24;
							simpleTelemtryData.month = gps_time_date << 16;
							simpleTelemtryData.day = gps_time_date << 8;
						}
						else
						{
							simpleTelemtryData.hour = gps_time_date << 24;
							simpleTelemtryData.minute = gps_time_date << 16;
							simpleTelemtryData.second = gps_time_date << 8;
						}
					}
						break;
					case SENSOR_ACCX:
						break;
					case SENSOR_ACCY:
						break;
					case SENSOR_ACCZ:
						break;
					}
				}
			}

			session->currentState = WAITING_START_FRAME;
		}
	}
		break;
	default:
		session->currentState = WAITING_START_FRAME;
		break;
	}

}

static void TX_Callback(uint8_t* id)
{
	SPortSession* session = (SPortSession*) id;

	if (!FIFO_available(session->send_buffer))
	{
		USART_ITConfig(session->port->port, USART_IT_TXE, DISABLE);
		USART_DirectionModeCmd(session->port->port, USART_Mode_Rx, ENABLE);
	}
	else
		USART_SendData(session->port->port, FIFO64_read(session->send_buffer));

}
