/*
 * module_multiplex.c
 *
 *  Created on: 19.08.2015
 *      Author: cem
 */

#include "module_multiplex_priv.h"

void module_multiplex_initializeConfig(void* config)
{
	struct Multiplex_Config* cfg = (struct Multiplex_Config*) config;

	if (cfg->version != 1)
	{
		cfg->version = 1;
		cfg->mapping[0] = Multiplex_Telemetry_Value_None;
		cfg->mapping[1] = Multiplex_Telemetry_Value_None;
		cfg->mapping[2] = Multiplex_Telemetry_Value_Distance;
		cfg->mapping[3] = Multiplex_Telemetry_Value_Altitude;
		cfg->mapping[4] = Multiplex_Telemetry_Value_Speed;
		cfg->mapping[5] = Multiplex_Telemetry_Value_VSpeed;
		cfg->mapping[6] = Multiplex_Telemetry_Value_Compass;
		cfg->mapping[7] = Multiplex_Telemetry_Value_COG;
		cfg->mapping[8] = Multiplex_Telemetry_Value_HomeDirection;
		cfg->mapping[9] = Multiplex_Telemetry_Value_Flightmode;
		cfg->mapping[10] = Multiplex_Telemetry_Value_Satellite;
		cfg->mapping[11] = Multiplex_Telemetry_Value_GPS_Fix;
		cfg->mapping[12] = Multiplex_Telemetry_Value_Voltage;
		cfg->mapping[13] = Multiplex_Telemetry_Value_LipoVoltage;
		cfg->mapping[14] = Multiplex_Telemetry_Value_Current;
		cfg->mapping[15] = Multiplex_Telemetry_Value_Capacity;
	}
}

void module_multiplex_start(const struct hardware_port_cfg* port, uint8_t* config)
{

	struct Multiplex_Session* session = (struct Multiplex_Session*) malloc(sizeof(struct Multiplex_Session));
	session->next = NULL;
	LL_APPEND(multiplex_sessions, session);

	session->port = port;
	session->config = (struct Multiplex_Config*) config;

	port->rx.clock.cmd(port->rx.clock.periph, ENABLE);
	port->tx.clock.cmd(port->tx.clock.periph, ENABLE);

	port->dma.clock.cmd(port->dma.clock.periph, ENABLE);

	port->clock.cmd(port->clock.periph, ENABLE);

	/* GPIO Initialization */
	GPIO_Init(port->rx.gpio, (GPIO_InitTypeDef *) &port->rx.init);
	GPIO_Init(port->tx.gpio, (GPIO_InitTypeDef *) &port->tx.init);

	/* Alternate function configuration */
	GPIO_PinAFConfig(port->rx.gpio, port->rx.pin_source, port->af);
	GPIO_PinAFConfig(port->tx.gpio, port->tx.pin_source, port->af);

	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(port->dma.channel);
	DMA_StructInit(&DMA_InitStructure);
	DMA_InitStructure.DMA_BufferSize = MODULE_MULTIPLEX_RESPONSE_SIZE;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &session->response;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&port->port->TDR);
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(port->dma.channel, &DMA_InitStructure);

	DMA_ITConfig(port->dma.channel, DMA_IT_TC, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = port->dma.nvic_ch;
	NVIC_InitStructure.NVIC_IRQChannelPriority = PRIORITY_TELEMETRY;
	NVIC_Init(&NVIC_InitStructure);

	/* USART Initialization */
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 38400;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(port->port, &USART_InitStructure);
	USART_HalfDuplexCmd(port->port, ENABLE);
	USART_OverrunDetectionConfig(port->port, USART_OVRDetection_Disable);
	USART_SetReceiverTimeOut(port->port, MODULE_MULTIPLEX_RECEIVE_TIMEOUT);
	USART_ReceiverTimeOutCmd(port->port, ENABLE);
	USART_Cmd(port->port, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = port->nvic_ch;
	NVIC_Init(&NVIC_InitStructure);

	session->flag = CoCreateFlag(Co_TRUE, Co_FALSE);
	session->task_id = CoCreateTask(module_multiplex_task, session, 0, &session->task_stack[MODULE_MULTIPLEX_TASK_STACK - 1],
			MODULE_MULTIPLEX_TASK_STACK);

	/* Initialize & Enable Interrupts */
	hardware_register_callback(port, &module_multiplex_irq_rx, NULL, (uint8_t*) session);
	hardware_register_timeout_callback(port, &module_multiplex_irq_timeout, (uint8_t*) session);
	hardware_register_dma_callback(session->port, &module_multiplex_irq_dma, (uint8_t*) session);

	USART_DMACmd(port->port, USART_DMAReq_Tx, ENABLE);
	USART_Cmd(port->port, ENABLE);

	USART_ITConfig(port->port, USART_IT_RXNE, ENABLE);
	USART_ITConfig(port->port, USART_IT_RTO, ENABLE);
}

void module_multiplex_task(void* pData)
{
	struct Multiplex_Session* session = (struct Multiplex_Session*) pData;

	uint8_t ledEnabled = session->port == &usart_port1;

	U64 ticks = CoGetOSTime();
	U64 next_toggle = ticks - 1;

	double temp = 0;

	while (1)
	{
		if (ledEnabled)
		{
			if (ticks > next_toggle)
			{
				hardware_led_toggle_red();
				next_toggle = ticks + delay_ms(200);
			}
		}

		if (CoWaitForSingleFlag(session->flag, 0) == E_OK)
		{
			memset(&session->response, 0, MODULE_MULTIPLEX_RESPONSE_SIZE);
			session->response.Address = session->address;

			switch (session->config->mapping[session->response.Address])
			{
			case Multiplex_Telemetry_Value_Altitude:
				session->response.Unit = Multiplex_Units_Meter;
				session->response.Value = simpleTelemetry_getRelativeHeight();
				break;
			case Multiplex_Telemetry_Value_Speed:
				session->response.Unit = Multiplex_Units_Speed;
				session->response.Value = simpleTelemtryData.speed * 36;
				break;
			case Multiplex_Telemetry_Value_Capacity:
				session->response.Unit = Multiplex_Units_Capacity;
				session->response.Value = simpleTelemtryData.capacity_current;
				break;
			case Multiplex_Telemetry_Value_Distance:
			{
				temp = simpleTelemetry_getDistance();
				if (temp > 2000)
				{
					session->response.Unit = Multiplex_Units_Distance;
					session->response.Value = temp / 100;
				}
				else
				{
					session->response.Unit = Multiplex_Units_Meter;
					session->response.Value = temp;

				}
			}
				break;
			case Multiplex_Telemetry_Value_COG:
				session->response.Unit = Multiplex_Units_Dir;
				session->response.Value = simpleTelemtryData.cog * 10;
				break;
			case Multiplex_Telemetry_Value_Compass:
				session->response.Unit = Multiplex_Units_Dir;
				session->response.Value = simpleTelemtryData.heading * 10;
				break;
			case Multiplex_Telemetry_Value_Current:
				session->response.Unit = Multiplex_Units_Current;
				session->response.Value = simpleTelemtryData.current * 10;
				break;
			case Multiplex_Telemetry_Value_Flightmode:
				session->response.Unit = Multiplex_Units_Liquid;
				session->response.Value = simpleTelemtryData.mode;
				break;
			case Multiplex_Telemetry_Value_GPS_Fix:
				session->response.Unit = Multiplex_Units_Liquid;
				session->response.Value = simpleTelemtryData.fixType;
				break;
			case Multiplex_Telemetry_Value_HomeDirection:
				session->response.Unit = Multiplex_Units_Dir;
				session->response.Value = simpleTelemetry_calculateAngle() * 10;
				break;
			case Multiplex_Telemetry_Value_LipoTemp:
				session->response.Unit = Multiplex_Units_Temp;
				session->response.Value = simpleTelemtryData.temp1 * 10;
				break;
			case Multiplex_Telemetry_Value_LipoVoltage:
				session->response.Unit = Multiplex_Units_Voltage;
				session->response.Value = simpleTelemetry_getLowestCell(0) / 100;
				break;
			case Multiplex_Telemetry_Value_Satellite:
				session->response.Unit = Multiplex_Units_Liquid;
				session->response.Value = simpleTelemtryData.numSat;
				break;
			case Multiplex_Telemetry_Value_VSpeed:
				session->response.Unit = Multiplex_Units_VSpeed;
				session->response.Value = simpleTelemtryData.vsi * 10;
				break;
			case Multiplex_Telemetry_Value_Voltage:
				session->response.Unit = Multiplex_Units_Voltage;
				session->response.Value = simpleTelemtryData.battery / 100;
				break;
			case Multiplex_Telemetry_Value_None:
				break;
			}

			session->read = 0;
			session->address = MODULE_MULTIPLEX_ADRESSES;

			DMA_SetCurrDataCounter(session->port->dma.channel, MODULE_MULTIPLEX_RESPONSE_SIZE);
			DMA_Cmd(session->port->dma.channel, ENABLE);
		}

		ticks = CoGetOSTime();
	}
}

static void module_multiplex_irq_rx(uint8_t* pHandle)
{
	struct Multiplex_Session* session = (struct Multiplex_Session*) pHandle;
	session->address = USART_ReceiveData(session->port->port);
	session->read++;
}

static void module_multiplex_irq_timeout(uint8_t* pHandle)
{
	struct Multiplex_Session* session = (struct Multiplex_Session*) pHandle;

	if (session->read == 1 && session->address < MODULE_MULTIPLEX_ADRESSES
			&& session->config->mapping[session->address] != Multiplex_Telemetry_Value_None)
	{
		isr_SetFlag(session->flag);
	}
	else
	{
		session->read = 0;
		session->address = MODULE_MULTIPLEX_ADRESSES;
	}

	USART_ClearITPendingBit(session->port->port, USART_IT_RTO);
}

static void module_multiplex_irq_dma(uint8_t* pHandle)
{
	struct Multiplex_Session* session = (struct Multiplex_Session*) pHandle;

	if (DMA_GetITStatus(session->port->dma.it_tc))
	{
		DMA_ClearITPendingBit(session->port->dma.it_gl);
		DMA_Cmd(session->port->dma.channel, DISABLE);
	}
}
