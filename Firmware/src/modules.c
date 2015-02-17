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

	config_save();

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
	def.NVIC_IRQChannelPriority = PRIORITY_TELEMETRY;
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
		hott_initializeConfig(parser->parserConfig);
		break;
	case parser_jeti:
		break;
	case parser_mavlink:
		mavlink_initializeConfig(parser->parserConfig);
		break;
	case parser_futaba:
		module_futaba_initializeConfig(parser->parserConfig);
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

	USART_ITConfig(usart_port2.port, USART_IT_RXNE, ENABLE);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void configManager_task(void* pdata)
{

	U64 exitIn = CoGetOSTime() + delay_ms(5600);
	U64 ticks = 0;
	U64 nextFCHeartbeat = CoGetOSTime() + delay_sec(2);
	uint8_t established = 0;
	uint8_t exit = 0;

	U64 stickConfigStart = 0;
	U64 stickConfigSetStart = 0;
	U64 stickConfigDelay = 0;
	U64 stickConfigLedOn = 0;

	uint8_t isStickConfig = 0;
	uint8_t stickConfigPos = 0;
	uint8_t stickConfigPosNew = 0;
	uint8_t stickConfigFinish = 0;
	uint8_t stickConfigBlink = 1;

	StatusType status;

	struct mavlink_rx_buffer* buffer = NULL;

	mavlink_message_t msg_tmp;
	uint8_t mavlink_tmp_buf[MAVLINK_MAX_PACKET_LEN];
	uint16_t msg_len = 0;
	uint16_t tx_len = 0;

	uint8_t currentValue = 0;

	while (!exit)
	{

		buffer = CoPendQueueMail(command_box_id, delay_ms(100), &status);

		msg_len = 0;

		ticks = CoGetOSTime();

		if (status == E_OK)
		{

			if (buffer->message.msgid == MAVLINK_MSG_ID_REQUEST_DATA_STREAM)
			{
				//TODO: SAVE Requested types
			}
			else if (buffer->message.msgid == MAVLINK_MSG_ID_CONFIGURATION_CONTROL)
			{
				established = 1;

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
					msg_len = mavlink_msg_configuration_version2_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, &msg_tmp,
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

				config_save();

				msg_len = mavlink_msg_configuration_control_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, &msg_tmp,
						result ? CONFIG_COMMAND_ACK : CONFIG_COMMAND_NACK, portNum, parserType);
			}

			buffer->inUse = 0;

		}
		else if (established)
		{
			if (ticks > nextFCHeartbeat && (simpleTelemtryData.lastHeartbeat + delay_sec(1))  > ticks)
			{
				msg_len = mavlink_msg_configuration_naza_heartbeat_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, &msg_tmp,
						ticks - simpleTelemtryData.lastHeartbeat);

				nextFCHeartbeat = ticks + delay_sec(2);
			}
			else
				msg_len = mavlink_pack_nextData(&msg_tmp, &currentValue);
		}
		else if (isStickConfig)
		{
			if (stickConfigDelay > ticks)
			{
				hardware_led_toggle_red();
			}
			else
			{
				if (!stickConfigFinish)
				{
					stickConfigPosNew = simpleTelemetry_stickConfigPosition();

					if (stickConfigPosNew != stickConfigPos)
					{
						stickConfigStart = ticks + delay_sec(30);

						stickConfigPos = stickConfigPosNew;
						stickConfigBlink = stickConfigPos;
						stickConfigLedOn = ticks;
					}

					if (stickConfigLedOn > ticks)
						hardware_led_on_red();
					else
					{
						hardware_led_off_red();

						if (ticks > (stickConfigLedOn + (stickConfigBlink < stickConfigPos ? 100 : 700)))
						{
							stickConfigLedOn = ticks + delay_ms(300);
							if (stickConfigBlink == stickConfigPos)
								stickConfigBlink = 1;
							else
								stickConfigBlink++;
						}
					}

					if (simpleTelemetry_isStickConfig())
					{
						if (stickConfigSetStart == 0)
							stickConfigSetStart = ticks + delay_sec(1);
					}
					else
						stickConfigSetStart = 0;

					if (stickConfigSetStart > 0 && ticks > stickConfigSetStart)
					{
						switch (stickConfigPos)
						{
						case 1:
							configuration.port1.type = parser_frsky;
							break;
						case 2:
							configuration.port1.type = parser_hott;
							break;
						case 3:
							configuration.port1.type = parser_futaba;
						}

						memset(configuration.port1.parserConfig, 0, CONFIG_PARSER_LENGTH);

						modules_initialize_config(&configuration.port1);
						config_save();

						stickConfigStart = ticks + delay_sec(3);
						stickConfigFinish = 1;
					}
				}
				else
				{
					//led off last half
					if ((stickConfigStart - ticks) < delay_ms(1500))
						hardware_led_off_red();
					else
						//Blink first half
						hardware_led_toggle_red();
				}
			}

			if (ticks > stickConfigStart)
				exit = 1;
		}
		else
		{
			if (simpleTelemetry_isStickConfig())
			{
				if (stickConfigStart == 0)
					stickConfigStart = ticks + delay_sec(1);

				if (ticks > stickConfigStart)
				{
					stickConfigStart = ticks + delay_sec(30); //auto exit stick config mode
					stickConfigDelay = ticks + delay_sec(3);
					isStickConfig = 1;
				}
			}
			else
				stickConfigStart = 0;

			if (ticks > exitIn)
				exit = 1;
		}

		if (msg_len > 0)
		{
			tx_len = mavlink_msg_to_send_buffer(mavlink_tmp_buf, &msg_tmp);

			for (int i = 0; i < tx_len; i++)
				FIFO512_write(tx_buffer, mavlink_tmp_buf[i]);

			USART_ITConfig(usart_port2.port, USART_IT_TXE, ENABLE);
		}

	}
	CoDelQueue(command_box_id, OPT_DEL_ANYWAY);

	USART_ITConfig(usart_port2.port, USART_IT_RXNE, DISABLE);
	USART_ITConfig(usart_port2.port, USART_IT_TXE, DISABLE);

	hardware_unregister_callback(&usart_port2);

	USART_Cmd(usart_port2.port, DISABLE);
	USART_DeInit(usart_port2.port);
	NVIC_DisableIRQ(usart_port2.nvic_ch);

	CoSchedLock();

	configManager_start(&configuration.port1, &usart_port1);
	configManager_start(&configuration.port2, &usart_port2);

	CoSchedUnlock();

	hardware_led_off_red();

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
		hott_start(port, parser->parserConfig);
		break;
	case parser_jeti:
		break;
	case parser_mavlink:
		mavlink_start(port, parser->parserConfig);
		break;
	case parser_futaba:
		module_futaba_start(port, parser->parserConfig);
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
