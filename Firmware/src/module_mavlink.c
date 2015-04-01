/*
 * module_mavlink.c
 *
 *  Created on: 29.08.2014
 *      Author: cem
 */

#include "module_mavlink_priv.h"

uint16_t mavlink_pack_nextData(mavlink_message_t* msg, uint8_t* currentValue, uint8_t type, uint8_t cells)
{

	uint16_t len = 0;

	switch (*currentValue)
	{
	case 0:
		len = mavlink_pack_heartbeat(msg, type);
		break;
	case 1:
		len = mavlink_pack_sys_status(msg, cells);
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
	case 6:
		len = mavlink_pack_battery(msg);
	}

	*currentValue = *currentValue + 1;

	if (*currentValue > 6)
		*currentValue = 0;

	return len;
}

__inline__ uint8_t mavlink_mode()
{
	//0 = stab = gps
	//1 = acro = manual
	//5 = alth = atti
	//6 = retl = failsafe

	if (simpleTelemtryData.mode == flightMode_gps)
		return 0;
	else if (simpleTelemtryData.mode == flightMode_failsafe)
		return 6;
	else if (simpleTelemtryData.mode == flightMode_atti)
		return 2;
	else
		return 1;
}

uint16_t mavlink_pack_heartbeat(mavlink_message_t* msg, uint8_t type)
{
	if (type == 0)
		return mavlink_msg_heartbeat_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, MAVLINK_TYPE, MAVLINK_AP_TYPE,
				MAV_MODE_PREFLIGHT, simpleTelemtryData.mode,
				simpleTelemtryData.mode == flightMode_failsafe ? MAV_STATE_CRITICAL : MAV_STATE_ACTIVE);
	else
	{

		return mavlink_msg_heartbeat_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, MAVLINK_TYPE, MAVLINK_AP_TYPE,
				simpleTelemtryData.armed ? 128 : 0, mavlink_mode(),
				simpleTelemtryData.mode == flightMode_failsafe ? MAV_STATE_CRITICAL : MAV_STATE_ACTIVE);

	}
}

uint16_t mavlink_pack_sys_status(mavlink_message_t* msg, uint8_t cellCount)
{
	return mavlink_msg_sys_status_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, MAVLINK_SENSORS, MAVLINK_SENSORS, MAVLINK_SENSORS,
			0, simpleTelemtryData.battery, simpleTelemtryData.current * 100, simpleTelemetry_getCalculatedPercentage(cellCount),
			0, 0, simpleTelemtryData.packets_lost, simpleTelemtryData.packets_drop, simpleTelemtryData.packets_corrupted, 0);
}

uint16_t mavlink_pack_gps(mavlink_message_t* msg)
{
	return mavlink_msg_gps_raw_int_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, simpleTelemtryData.fixType,
			simpleTelemtryData.lat * 10000000, simpleTelemtryData.lon * 10000000,
			simpleTelemetry_getRelativeHeight() * 1000, simpleTelemtryData.hdop * 100,
			simpleTelemtryData.vdop * 100, simpleTelemtryData.speed * 100, simpleTelemtryData.cog * 100,
			simpleTelemtryData.numSat);
}

uint16_t mavlink_pack_vfr_hud(mavlink_message_t* msg)
{
	return mavlink_msg_vfr_hud_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, simpleTelemtryData.speed,
			simpleTelemtryData.heading, (simpleTelemtryData.throttle + 1000) / 20, simpleTelemetry_getRelativeHeight(), simpleTelemtryData.vsi);
}

uint16_t mavlink_pack_attitude(mavlink_message_t* msg)
{
	return mavlink_msg_attitude_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, simpleTelemtryData.roll,
			simpleTelemtryData.pitch, simpleTelemtryData.heading * M_PI / 180, 0, 0, 0);
}

/*
 *
 RC	Mavlink	Naza
 1	2		1
 2	3		2
 3	6		5
 4	4		3
 5	8		7
 6	9		8
 7	5		4
 8	10		9
 9	1		0
 10	7		6
 */

uint16_t mavlink_pack_rc_out(mavlink_message_t* msg)
{
	return mavlink_msg_rc_channels_raw_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, 10, simpleTelemetry_getRCIn(1),
			simpleTelemetry_getRCIn(2), simpleTelemetry_getRCIn(5), simpleTelemetry_getRCIn(3), simpleTelemetry_getRCIn(7),
			simpleTelemetry_getRCIn(8), simpleTelemetry_getRCIn(4), simpleTelemetry_getRCIn(9), 255);
}

uint16_t mavlink_pack_battery(mavlink_message_t* msg)
{
	return mavlink_msg_battery_status_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 1, MAV_BATTERY_FUNCTION_ALL,
			MAV_BATTERY_TYPE_LIPO, simpleTelemtryData.temp1, simpleTelemtryData.cells, -1, -1, -1, -1);

}

void mavlink_initializeConfig(void* config)
{
	struct mavlink_config* cfg = (struct mavlink_config*) config;

	if (cfg->version != 1)
	{
		cfg->version = 1;
		cfg->num_cells = 3;
	}
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
	session->config = (struct mavlink_config*) config;
	FIFO_init(session->tx_buffer);

	for (int i = 0; i < MODULE_MAVLINK_RX_QUEUE_SIZE; i++)
		session->rx_buffer[i].inUse = 0;

	USART_Init(port->port, &USART_InitStructure);
	USART_OverrunDetectionConfig(port->port, USART_OVRDetection_Disable);
	USART_Cmd(port->port, ENABLE);

	NVIC_InitTypeDef def;
	def.NVIC_IRQChannelCmd = ENABLE;
	def.NVIC_IRQChannel = port->nvic_ch;
	def.NVIC_IRQChannelPriority = PRIORITY_MAVLINK;
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

	U64 currTime = 0;
	U64 loop200 = 0;
	U64 loop500 = 0;
	U64 loop1000 = 0;
	U64 bootUpTime = 0;

	while (1)
	{
		buffer = CoPendQueueMail(session->event_id, delay_ms(100), &status);
		currTime = CoGetOSTime();

		if (status == E_OK)
		{
			msg_len = 0;

			if (buffer->message.msgid == MAVLINK_MSG_ID_REQUEST_DATA_STREAM)
			{
				//Bug in MinimOSD FW leads to Hangup on Warning checking
				//delay data transmission to simulate warnings
				/*bootUpTime = currTime + delay_ms(5000);
				FIFO_init(session->tx_buffer);*/
			}

			buffer->inUse = 0;

			if (msg_len > 0)
			{
				tx_len = mavlink_msg_to_send_buffer(mavlink_tmp_buf, &msg_tmp);

				for (int i = 0; i < tx_len; i++)
					FIFO512_write(session->tx_buffer, mavlink_tmp_buf[i]);

				USART_ITConfig(session->port->port, USART_IT_TXE, ENABLE);
			}
		}
		/*else if (bootUpTime > currTime)
		{
			if (currTime > loop1000)
			{
				loop1000 = currTime + delay_ms(1000); // 1Hz = every 1000 milliseconds

				mavlink_pack_heartbeat(&msg_tmp, 1);
				mavlink_sendMessage(session, &msg_tmp, mavlink_tmp_buf);
			}
		}*/
		else
		{

			if (currTime > loop1000)
			{
				loop1000 = currTime + delay_ms(1000); // 1Hz = every 1000 milliseconds

				mavlink_pack_heartbeat(&msg_tmp, 1);
				mavlink_sendMessage(session, &msg_tmp, mavlink_tmp_buf);

				mavlink_pack_sys_status(&msg_tmp, session->config->num_cells);
				mavlink_sendMessage(session, &msg_tmp, mavlink_tmp_buf);

			}

			if (currTime > loop500)
			{
				loop500 = currTime + delay_ms(500); // 2Hz = every 500 milliseconds

				mavlink_pack_gps(&msg_tmp);
				mavlink_sendMessage(session, &msg_tmp, mavlink_tmp_buf);

				mavlink_pack_vfr_hud(&msg_tmp);
				mavlink_sendMessage(session, &msg_tmp, mavlink_tmp_buf);
			}

			if (currTime > loop200)
			{
				loop200 = currTime + delay_ms(200); // 5Hz = every 200 milliseconds

				mavlink_pack_attitude(&msg_tmp);
				mavlink_sendMessage(session, &msg_tmp, mavlink_tmp_buf);

				mavlink_pack_rc_out(&msg_tmp);
				mavlink_sendMessage(session, &msg_tmp, mavlink_tmp_buf);

			}

		}

	}

}

__inline__ static void mavlink_sendMessage(struct mavlink_session* session, mavlink_message_t* msg, uint8_t* mavlink_tmp_buf)
{
	uint16_t tx_len = mavlink_msg_to_send_buffer(mavlink_tmp_buf, msg);

	for (int i = 0; i < tx_len; i++)
		FIFO512_write(session->tx_buffer, mavlink_tmp_buf[i]);

	USART_ITConfig(session->port->port, USART_IT_TXE, ENABLE);
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
