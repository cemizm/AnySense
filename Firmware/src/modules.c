/*
 * modules.c
 *
 *  Created on: 29.08.2014
 *      Author: cem
 */

#include "modules_privat.h"

void modules_initialize()
{
	config_initialize();

	modules_initialize_config(&configuration.port1);
	modules_initialize_config(&configuration.port2);

	USART_InitTypeDef USART_InitStructure;

	usart_port2.rx.clock.cmd(usart_port2.rx.clock.periph, ENABLE);
	usart_port2.tx.clock.cmd(usart_port2.tx.clock.periph, ENABLE);

	usart_port2.clock.cmd(usart_port2.clock.periph, ENABLE);

	/* GPIO Initialization */
	GPIO_Init(usart_port2.rx.gpio, (GPIO_InitTypeDef *) &usart_port2.rx.init);
	GPIO_Init(usart_port2.tx.gpio, (GPIO_InitTypeDef *) &usart_port2.tx.init);

	/* Alternate function configuration */
	GPIO_PinAFConfig(usart_port2.rx.gpio, usart_port2.rx.pin_source, usart_port2.af);
	GPIO_PinAFConfig(usart_port2.tx.gpio, usart_port2.tx.pin_source, usart_port2.af);

	/* USART Initialization */
	USART_InitStructure.USART_BaudRate = 57600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(usart_port2.port, &USART_InitStructure);
	USART_OverrunDetectionConfig(usart_port2.port, USART_OVRDetection_Disable);
	USART_Cmd(usart_port2.port, ENABLE);

	NVIC_InitTypeDef def;
	def.NVIC_IRQChannelCmd = ENABLE;
	def.NVIC_IRQChannel = usart_port2.nvic_ch;
	def.NVIC_IRQChannelPriority = 1;
	NVIC_Init(&def);

	FIFO_init(tx_buffer);
}

void modules_initialize_config(struct portParserStruct* parser)
{

	switch (parser->type)
	{
	case parser_frsky:
		sport_initializeConfig(parser->parserConfig);
		break;
	case parser_hott:
		break;
	case parser_jeti:
		break;
	case parser_mavlink:
		break;
	default:
		break;
	}
}

void modules_start()
{
	command_box_id = CoCreateQueue(command_queue, RX_QUEUE_SIZE, EVENT_SORT_TYPE_PRIO);

	assert_param(command_box_id != E_CREATE_FAIL);

	configManager_task_id = CoCreateTask(configManager_task, 0, 0, &configManager_task_stack[CONFIGMANAGER_TASK_STACK - 1],
			CONFIGMANAGER_TASK_STACK);

	assert_param(configManager_task_id != E_CREATE_FAIL);

	hardware_register_callback(&usart_port2, &RX_Callback, &TX_Callback, 0);

	FIFO64_write(tx_buffer, 0xCE);
	FIFO64_write(tx_buffer, 0xFF);
	FIFO64_write(tx_buffer, 0xCE);
	FIFO64_write(tx_buffer, 0xFF);

	USART_ITConfig(usart_port2.port, USART_IT_TXE, ENABLE);

	USART_ITConfig(usart_port2.port, USART_IT_RXNE, ENABLE);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void configManager_task(void* pdata)
{
	DEBUG_TOGGLE_ORANGE();

	U64 exitIn = CoGetOSTime() + 2100;
	U64 ticks = 0;
	U64 lastNazaHeartbeatSend = 0;
	uint8_t established = 0;
	uint8_t exit = 0;

	StatusType status;

	struct mavlink_rx_buffer* buffer = NULL;

	mavlink_message_t msg_tmp;
	uint8_t mavlink_tmp_buf[MAVLINK_MAX_PACKET_LEN];
	uint16_t msg_len = 0;
	uint16_t tx_len = 0;

	uint8_t currentValue = 0;

	while (!exit)
	{

		buffer = CoPendQueueMail(command_box_id, established ? 100 : 500, &status);

		msg_len = 0;

		ticks = CoGetOSTime();

		if (status == E_OK)
		{
			established = 1;

			if (buffer->message.msgid == MAVLINK_MSG_ID_REQUEST_DATA_STREAM)
			{
				//TODO: SAVE Requested types
			}
			else if (buffer->message.msgid == MAVLINK_MSG_ID_CONFIGURATION_CONTROL)
			{

				CONFIG_COMMAND cmd = mavlink_msg_configuration_control_get_command(&buffer->message);
				uint16_t param1 = mavlink_msg_configuration_control_get_param1(&buffer->message);
				//uint16_t param2 = mavlink_msg_configuration_control_get_param2(&buffer->message);
				switch (cmd)
				{
				case CONFIG_COMMAND_BOOTLOADER:
					*((unsigned long *) 0x20003FE0) = 0xDEADBEEF;
					NVIC_SystemReset();
					break;
				case CONFIG_COMMAND_EXIT:
					exit = 1;
					break;
				case CONFIG_COMMAND_GET_VERSION:
					msg_len = mavlink_msg_configuration_version_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, &msg_tmp,
					FIRMWARE_VERSION, configuration.port1.type, configuration.port2.type);
					break;
				case CONFIG_COMMAND_GET_CONFIGURATION:
				{

					struct portParserStruct* port = NULL;

					if (param1 == CONFIG_PORT_1)
						port = &configuration.port1;
					else if (param1 == CONFIG_PORT_2)
						port = &configuration.port2;

					if (port != NULL)
					{
						msg_len = mavlink_msg_configuration_port_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, &msg_tmp, param1,
								port->type, port->parserConfig);
					}
				}
					break;
				default:
					break;
				}
			}
			else if (buffer->message.msgid == MAVLINK_MSG_ID_CONFIGURATION_PORT)
			{
				uint8_t portNum = mavlink_msg_configuration_port_get_port(&buffer->message);
				uint8_t parserType = mavlink_msg_configuration_port_get_protocol(&buffer->message);
				uint8_t result = 0;
				if (portNum == CONFIG_PORT_1)
				{
					configuration.port1.type = parserType;
					result = mavlink_msg_configuration_port_get_data(&buffer->message, configuration.port1.parserConfig);
				}
				else if (portNum == CONFIG_PORT_2)
				{
					configuration.port2.type = parserType;
					result = mavlink_msg_configuration_port_get_data(&buffer->message, configuration.port2.parserConfig);
				}

				msg_len = mavlink_msg_configuration_control_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, &msg_tmp,
						result ? CONFIG_COMMAND_ACK : CONFIG_COMMAND_NACK, portNum, parserType);
			}

			buffer->inUse = 0;

			if (msg_len > 0)
			{
				tx_len = mavlink_msg_to_send_buffer(mavlink_tmp_buf, &msg_tmp);

				for (int i = 0; i < tx_len; i++)
					FIFO512_write(tx_buffer, mavlink_tmp_buf[i]);

				USART_ITConfig(usart_port2.port, USART_IT_TXE, ENABLE);
			}

		}
		else if (established)
		{
			if (simpleTelemtryData.lastHeartbeat > lastNazaHeartbeatSend)
			{
				msg_len = mavlink_msg_configuration_naza_heartbeat_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, &msg_tmp,
						ticks - simpleTelemtryData.lastHeartbeat);

				lastNazaHeartbeatSend = simpleTelemtryData.lastHeartbeat;
			}
			else
				msg_len = mavlink_pack_nextData(&msg_tmp, &currentValue);

			if (msg_len > 0)
			{
				tx_len = mavlink_msg_to_send_buffer(mavlink_tmp_buf, &msg_tmp);

				for (int i = 0; i < tx_len; i++)
					FIFO512_write(tx_buffer, mavlink_tmp_buf[i]);

				USART_ITConfig(usart_port2.port, USART_IT_TXE, ENABLE);
			}
		}
		else
		{
			if (ticks > exitIn)
				exit = 1;
		}

	}

	USART_ITConfig(usart_port2.port, USART_IT_RXNE, DISABLE);
	USART_ITConfig(usart_port2.port, USART_IT_TXE, DISABLE);

	hardware_unregister_callback(&usart_port2);

	USART_Cmd(usart_port2.port, DISABLE);
	USART_DeInit(usart_port2.port);

	CoSchedLock();

	config_save();

	configManager_start(&configuration.port1, &usart_port1);
	configManager_start(&configuration.port2, &usart_port2);

	CoSchedUnlock();

	DEBUG_TOGGLE_ORANGE();

	CoExitTask();

}

void configManager_start(struct portParserStruct* parser, const struct hardware_port_cfg* port)
{

	switch (parser->type)
	{
	case parser_frsky:
		sport_start(port, parser->parserConfig);
		break;
	case parser_hott:
		break;
	case parser_jeti:
		break;
	case parser_mavlink:
		break;
	default:
		break;
	}
}

static void RX_Callback(uint8_t* id)
{

	uint8_t read = USART_ReceiveData(usart_port2.port);

	struct mavlink_rx_buffer* buffer = NULL;
	for (int i = 0; i < RX_QUEUE_SIZE; i++)
	{
		if (!rx_buffer[i].inUse)
		{
			buffer = &rx_buffer[i];
			break;
		}
	}

	if (buffer == NULL)
		return;

	if (mavlink_parse_char(MAVLINK_COMM_3, read, &buffer->message, &tmp_mavlink_status))
	{
		buffer->inUse = 1;
		StatusType t = isr_PostQueueMail(command_box_id, buffer);

		if (t == E_QUEUE_FULL)
		{
			buffer->inUse = 0;
		}
	}

}

static void TX_Callback(uint8_t* id)
{
	if (!FIFO_available(tx_buffer))
		USART_ITConfig(usart_port2.port, USART_IT_TXE, DISABLE);
	else
		USART_SendData(usart_port2.port, FIFO512_read(tx_buffer));

}

#pragma GCC diagnostic pop
