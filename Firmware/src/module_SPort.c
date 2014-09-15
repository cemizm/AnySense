/*
 * Sport->c
 *
 *  Created on: 22.08.2014
 *      Author: cem
 */

#include "module_SPort_priv.h"

void sport_initializeConfig(void* conf)
{
	uint16_t version = *((uint8_t*) conf + 1);
	//struct SPort_Config* scfg = (struct SPort_Config*)conf;

	if (version != SPORT_CONFIG_CURRENT_VERSION)
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
	session->currentValue = 0;
	session->tmp.Header = SPORT_DATAFRAME;

	FIFO_init(session->send_buffer);

	LL_APPEND(sessions, session);

	/* Initialize & Enable Interrupts */
	hardware_register_callback(port, &RX_Callback, &TX_Callback, (uint8_t*) session);

	NVIC_InitTypeDef def;
	def.NVIC_IRQChannelCmd = ENABLE;
	def.NVIC_IRQChannel = port->nvic_ch;
	def.NVIC_IRQChannelPriority = 1;
	NVIC_Init(&def);

	USART_ITConfig(port->port, USART_IT_RXNE, ENABLE);
}

void sport_getValue(enum telemetryValue val, int32_t * result, uint8_t* len)
{
	*len = 0;

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
		result[0] = (simpleTelemtryData.homeAltBaro - simpleTelemtryData.alt) * 100;
		*len = 1;
		break;
	case tv_gpsAlt:
		result[0] = simpleTelemtryData.gpsAlt * 100;
		*len = 1;
		break;
	case tv_speed:
		result[0] = simpleTelemtryData.speed * 100;
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
		result[0] = simpleTelemtryData.year << 24 || simpleTelemtryData.month << 16 || simpleTelemtryData.day << 8 || 0xff;
		result[1] = simpleTelemtryData.hour << 24 || simpleTelemtryData.minute << 16 || simpleTelemtryData.second << 8;
		*len = 2;
		break;
	case tv_none:
	default:
		*len = 0;
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
		break;
	case WAITING_SESNOR_ID:

		if (read == session->config->sensorId)
		{
			if (session->config->map[session->currentValue] != tv_none)
			{
				session->tmp.Id = mapping[session->currentValue];

				if (session->tmp_length == 0)
					sport_getValue(session->config->map[session->currentValue], session->tmp_bytes, &session->tmp_length);

				if (session->tmp_length > 0)
				{
					session->tmp.Value = session->tmp_bytes[session->tmp_length - 1];

					short crc = 0;
					uint8_t* data = (uint8_t*) &session->tmp;
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
					USART_ITConfig(session->port->port, USART_IT_TXE, ENABLE);

					session->tmp_length--;
				}
			}

			if (session->tmp_length == 0)
			{
				uint8_t startValue = session->currentValue + 1;

				while (startValue != session->currentValue && session->config->map[startValue] == tv_none)
				{
					startValue++;
					if (startValue > SPORT_VALUES_MAX)
						startValue = 0;
				}

				session->currentValue = startValue;
			}
		}
		session->currentState = WAITING_START_FRAME;
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
		USART_ITConfig(session->port->port, USART_IT_TXE, DISABLE);
	else
		USART_SendData(session->port->port, FIFO64_read(session->send_buffer));

}
