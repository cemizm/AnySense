/*
 * module_mavlink.c
 *
 *  Created on: 29.08.2014
 *      Author: cem
 */

#include "module_mavlink_priv.h"

uint16_t mavlink_pack_nextData(mavlink_message_t* msg, uint8_t* currentValue)
{

	uint16_t len = 0;

	switch (*currentValue)
	{
	case 0:
		len = mavlink_pack_heartbeat(msg);
		break;
	case 1:
		len = mavlink_pack_sys_status(msg);
		break;
	case 2:
		len = mavlink_pack_gps(msg);
		break;
	case 3:
		len = mavlink_pack_vfr_hud(msg);
		break;
	case 4:
		len = mavlink_pack_attitude(msg);
		break;
	case 5:
		len = mavlink_pack_rc_out(msg);
		break;
	}

	*currentValue = *currentValue + 1;

	if (*currentValue > 5)
		*currentValue = 0;

	return len;
}

uint16_t mavlink_pack_heartbeat(mavlink_message_t* msg)
{
	return mavlink_msg_heartbeat_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, MAVLINK_TYPE, MAVLINK_AP_TYPE, MAV_MODE_PREFLIGHT,
			simpleTelemtryData.mode, simpleTelemtryData.mode == flightMode_failsafe ? MAV_STATE_CRITICAL : MAV_STATE_ACTIVE);
}

uint16_t mavlink_pack_sys_status(mavlink_message_t* msg)
{
	return mavlink_msg_sys_status_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, MAVLINK_SENSORS, MAVLINK_SENSORS, MAVLINK_SENSORS,
			0, simpleTelemtryData.battery, 0, 0, 0, 0, simpleTelemtryData.packets_lost, simpleTelemtryData.packets_drop,
			simpleTelemtryData.packets_corrupted, 0);
}

uint16_t mavlink_pack_gps(mavlink_message_t* msg)
{
	//TODO: fix missing values
	return mavlink_msg_gps_raw_int_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, simpleTelemtryData.fixType,
			simpleTelemtryData.lat * 10000000, simpleTelemtryData.lon * 10000000, simpleTelemtryData.alt * 1000,
			simpleTelemtryData.hdop * 100, simpleTelemtryData.vdop * 100, simpleTelemtryData.speed * 100,
			simpleTelemtryData.cog * 100, simpleTelemtryData.numSat);
}
uint16_t mavlink_pack_vfr_hud(mavlink_message_t* msg)
{
	//TODO: fix missing values
	return mavlink_msg_vfr_hud_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, simpleTelemtryData.speed,
			simpleTelemtryData.heading, (simpleTelemtryData.throttle + 1000) / 20, simpleTelemtryData.alt, simpleTelemtryData.vsi);
}

uint16_t mavlink_pack_attitude(mavlink_message_t* msg)
{
	//TODO: fix missing values
	return mavlink_msg_attitude_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, simpleTelemtryData.roll,
			simpleTelemtryData.pitch, simpleTelemtryData.heading * M_PI / 180, 0, 0, 0);
}

uint16_t mavlink_pack_rc_out(mavlink_message_t* msg)
{
	return mavlink_msg_rc_channels_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, 10, simpleTelemetry_getRCIn(0),
			simpleTelemetry_getRCIn(1), simpleTelemetry_getRCIn(2), simpleTelemetry_getRCIn(3), simpleTelemetry_getRCIn(4),
			simpleTelemetry_getRCIn(5), simpleTelemetry_getRCIn(6), simpleTelemetry_getRCIn(7), simpleTelemetry_getRCIn(8),
			simpleTelemetry_getRCIn(9), UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX,
			UINT16_MAX, 255);
}

void mavlink_initializeConfig(void* config)
{

}

void mavlink_start(const struct hardware_port_cfg* port, uint8_t* config)
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
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	struct mavlink_session* session = (struct mavlink_session*) malloc(sizeof(struct mavlink_session));
	session->next = NULL;
	LL_APPEND(mavlink_sessions, session);

	session->port = port;
	session->config = config;
	FIFO_init(session->tx_buffer);

	for(int i=0; i<MODULE_MAVLINK_RX_QUEUE_SIZE;i++)
		session->rx_buffer[i].inUse = 0;

	USART_Init(port->port, &USART_InitStructure);
	USART_OverrunDetectionConfig(port->port, USART_OVRDetection_Disable);
	USART_Cmd(port->port, ENABLE);

	NVIC_InitTypeDef def;
	def.NVIC_IRQChannelCmd = ENABLE;
	def.NVIC_IRQChannel = port->nvic_ch;
	def.NVIC_IRQChannelPriority = 0;
	NVIC_Init(&def);

	session->event_id = CoCreateQueue(session->queue, MODULE_MAVLINK_RX_QUEUE_SIZE, EVENT_SORT_TYPE_PRIO);

	session->task_id = CoCreateTask(module_mavlink_task, session, 0, &session->task_stack[MODULE_MAVLINK_TASK_STACK - 1],
			MODULE_MAVLINK_TASK_STACK);

	hardware_register_callback(session->port, &RX_Callback, &TX_Callback, (uint8_t*) session);

	USART_ITConfig(session->port->port, USART_IT_RXNE, ENABLE);
}

void module_mavlink_task(void* pData)
{
	struct mavlink_session* session = (struct mavlink_session*) pData;

	struct mavlink_rx_buffer* buffer = NULL;

	StatusType status;

	mavlink_message_t msg_tmp;
	uint8_t mavlink_tmp_buf[MAVLINK_MAX_PACKET_LEN];
	uint16_t msg_len = 0;
	uint16_t tx_len = 0;

	uint8_t currentValue = 0;

	while (1)
	{
		buffer = CoPendQueueMail(session->event_id, 100, &status);

		msg_len = 0;

		if (status == E_OK)
		{
			if (buffer->message.msgid == MAVLINK_MSG_ID_REQUEST_DATA_STREAM)
			{
				//TODO: SAVE Requested types
			}

			buffer->inUse = 0;
		}
		else
		{
			msg_len = mavlink_pack_nextData(&msg_tmp, &currentValue);
		}

		if (msg_len > 0)
		{
			tx_len = mavlink_msg_to_send_buffer(mavlink_tmp_buf, &msg_tmp);

			for (int i = 0; i < tx_len; i++)
				FIFO512_write(session->tx_buffer, mavlink_tmp_buf[i]);

			USART_ITConfig(session->port->port, USART_IT_TXE, ENABLE);
		}
	}

}

static void RX_Callback(uint8_t* id)
{
	struct mavlink_session* session = (struct mavlink_session*) id;

	uint8_t read = USART_ReceiveData(session->port->port);

	struct mavlink_rx_buffer* buffer = NULL;
	for (int i = 0; i < MODULE_MAVLINK_RX_QUEUE_SIZE; i++)
	{
		if (!session->rx_buffer[i].inUse)
		{
			buffer = &session->rx_buffer[i];
			break;
		}
	}

	if (buffer == NULL)
		return;

	if (mavlink_parse_char(MAVLINK_COMM_3, read, &buffer->message, &session->tmp_status))
	{
		buffer->inUse = 1;
		StatusType t = isr_PostQueueMail(session->event_id, buffer);

		if (t == E_QUEUE_FULL)
		{
			buffer->inUse = 0;
		}
	}


}

static void TX_Callback(uint8_t* id)
{
	struct mavlink_session* session = (struct mavlink_session*) id;

	if (!FIFO_available(session->tx_buffer))
		USART_ITConfig(session->port->port, USART_IT_TXE, DISABLE);
	else
		USART_SendData(session->port->port, FIFO512_read(session->tx_buffer));
}
