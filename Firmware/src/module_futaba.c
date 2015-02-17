/*
 * module_futaba.c
 *
 *  Created on: 03.01.2015
 *      Author: cem
 */

#include "module_futaba_priv.h"

void module_futaba_initializeConfig(void* config)
{
	struct Futaba_Config* cfg = (struct Futaba_Config*) config;

	if (cfg->version != 1)
	{
		cfg->version = 1;
		cfg->minFix = fixType_2D;

		memset(cfg->timeSlots, 0, MODULE_FUTABA_SLOTS);

		cfg->timeSlots[1] = Futaba_Values_Vario;
		cfg->timeSlots[3] = Futaba_Values_Current;
		cfg->timeSlots[6] = Futaba_Values_Cell;
		cfg->timeSlots[7] = Futaba_Values_Temperatur;
		cfg->timeSlots[8] = Futaba_Values_GPS;
		cfg->timeSlots[16] = Futaba_Values_Kompass;
		cfg->timeSlots[17] = Futaba_Values_Sats;
		cfg->timeSlots[18] = Futaba_Values_Flightmode;
	}
}

void module_futaba_start(const struct hardware_port_cfg* port, uint8_t* config)
{
	USART_InitTypeDef USART_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;

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
	USART_InitStructure.USART_BaudRate = 100000;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_2;
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(port->port, &USART_InitStructure);
	USART_InvPinCmd(port->port, USART_InvPin_Rx | USART_InvPin_Tx, ENABLE);
	USART_HalfDuplexCmd(port->port, ENABLE);
	USART_OverrunDetectionConfig(port->port, USART_OVRDetection_Disable);
	USART_Cmd(port->port, ENABLE);

	struct Futaba_Session* session = (struct Futaba_Session*) malloc(sizeof(struct Futaba_Session));

	session->port = port;
	session->next = NULL;
	session->config = (struct Futaba_Config*) config;
	session->rxPointer = 0;
	session->currentSlot = 0;
	session->lastFrame = 0;
	session->slotHasData = 0;

	memset(session->rxBuffer, 0, MODULE_FUTABA_BUFFER_SIZE);
	memset(session->slotData, 0, MODULE_FUTABA_SLOTS * MODULE_FUTABA_SLOT_DATA);
	FIFO_init(session->txBuffer);

	LL_APPEND(futaba_sessions, session);

	/* Initialize & Enable Interrupts */
	hardware_register_callback2(port, &module_futaba_rx_callback, &module_futaba_tx_callback, &module_futaba_tim_callback,
			(uint8_t*) session);

	session->task_id = CoCreateTask(module_futaba_task, session, 1, &session->task_stack[MODULE_FUTABA_TASK_STACK - 1],
			MODULE_FUTABA_TASK_STACK);

	NVIC_InitTypeDef def;
	def.NVIC_IRQChannelCmd = ENABLE;
	def.NVIC_IRQChannel = port->nvic_ch;
	def.NVIC_IRQChannelPriority = PRIORITY_TELEMETRY;
	NVIC_Init(&def);

	//Timer
	port->timer.rcc.cmd(port->timer.rcc.periph, ENABLE);

	TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBase_InitStructure.TIM_Prescaler = 2 - 1;
	TIM_TimeBase_InitStructure.TIM_Period = MODULE_FUTABA_TIMING_START;
	TIM_TimeBaseInit(port->timer.tim, &TIM_TimeBase_InitStructure);

	TIM_ClearITPendingBit(port->timer.tim, TIM_IT_Update);

	TIM_ITConfig(port->timer.tim, TIM_IT_Update, ENABLE);

	def.NVIC_IRQChannel = port->timer.nvic_ch;
	def.NVIC_IRQChannelCmd = ENABLE;
	def.NVIC_IRQChannelPriority = PRIORITY_TELEMETRY;
	NVIC_Init(&def);

}

void module_futaba_task(void* pData)
{
	struct Futaba_Session* session = (struct Futaba_Session*) pData;

	uint8_t slot = 0;

	uint8_t ledEnabled = session->port == &usart_port1;

	U64 ticks = CoGetOSTime();
	U64 next_toggle = ticks - 1;
	int16_t i16tmp = -5000;
	uint16_t u16tmp = 500;

	session->slotHasData = 0;

	for (slot = 0; slot < MODULE_FUTABA_SLOTS; slot++)
	{
		switch (session->config->timeSlots[slot])
		{
		case Futaba_Values_Vario:
			session->slotHasData |= 11 << slot;
			slot += 1;
			break;
		case Futaba_Values_Current:
			session->slotHasData |= 111 << slot;
			slot += 3;
			break;
		case Futaba_Values_GPS:
			session->slotHasData |= 0xFF << slot;
			slot += 7;
			break;
		case Futaba_Values_None:
			break;
		default:
			session->slotHasData |= 1 << slot;
			break;
		}

	}

	USART_ITConfig(session->port->port, USART_IT_RXNE, ENABLE);

	while (1)
	{
		if (ledEnabled)
		{
			if ((session->lastFrame + delay_sec(2)) < ticks)
				hardware_led_off_red();
			else if (ticks > next_toggle)
			{
				hardware_led_toggle_red();
				next_toggle = ticks + delay_ms(200);
			}

		}

		for (slot = 0; slot < MODULE_FUTABA_SLOTS; slot++)
		{
			switch (session->config->timeSlots[slot])
			{
			case Futaba_Values_Vario:
			{
				i16tmp = simpleTelemtryData.vsi * 100;
				u16tmp = simpleTelemtryData.alt + 30000;

				session->slotData[slot][0] = i16tmp >> 8;
				session->slotData[slot][1] = i16tmp;
				session->slotData[slot + 1][0] = (((u16tmp >> 8) & 0x7F)) | 0b1 << 7;
				session->slotData[slot + 1][1] = (u16tmp);

				slot += 1;
			}
				break;
			case Futaba_Values_Current:
			{
				i16tmp = simpleTelemtryData.current * 100;

				if (i16tmp > 0x3FFF)
					i16tmp = 0x3FFF;
				else if (i16tmp < -16383)
					i16tmp = -16383;

				u16tmp = i16tmp >= 0 ? i16tmp : (i16tmp * -1 ^ 0xFFFF);

				u16tmp = u16tmp & 0x3FFF;

				session->slotData[slot + 0][0] = u16tmp >> 8 | 0x40;

				if (simpleTelemtryData.current < 0)
					session->slotData[slot + 0][0] |= 0x80;

				session->slotData[slot + 0][1] = u16tmp;

				i16tmp = simpleTelemtryData.capacity_current;
				u16tmp = simpleTelemtryData.battery / 10;

				session->slotData[slot + 1][0] = u16tmp >> 8;
				session->slotData[slot + 1][1] = u16tmp;
				session->slotData[slot + 2][0] = i16tmp >> 8;
				session->slotData[slot + 2][1] = i16tmp;

				slot += 2;
			}
				break;
			case Futaba_Values_Sats:
				session->slotData[slot][0] = 100 + simpleTelemtryData.numSat;
				session->slotData[slot][1] = 0x80;
				break;
			case Futaba_Values_Temperatur:
				session->slotData[slot][0] = 100 + simpleTelemtryData.temp1;
				session->slotData[slot][1] = 0x80;
				break;
			case Futaba_Values_Kompass:
			{
				session->slotData[slot][0] = (uint16_t) simpleTelemtryData.heading;
				session->slotData[slot][1] = ((uint16_t) simpleTelemtryData.heading) >> 8;
				break;
			}
			case Futaba_Values_Cell:
			{
				u16tmp = simpleTelemetry_getLowestCell(0);
				session->slotData[slot][0] = u16tmp;
				session->slotData[slot][1] = u16tmp >> 8;
				break;
			}
			case Futaba_Values_Flightmode:
				session->slotData[slot][0] = 100 + simpleTelemtryData.mode;
				session->slotData[slot][1] = 0x80;
				break;
			case Futaba_Values_GPS:
			{
				struct Futaba_Sensor_SBS01G* gps = (struct Futaba_Sensor_SBS01G*) session->slotData[slot];

				if (simpleTelemtryData.fixType >= session->config->minFix)
				{
					gps->LongitudeSign = simpleTelemtryData.lon < 0 ? 1 : 0;
					gps->Longitude =
							((simpleTelemtryData.lon < 0 ? simpleTelemtryData.lon * -1 : simpleTelemtryData.lon) * 600000);

					gps->LatitudeSign = simpleTelemtryData.lat < 0 ? 1 : 0;
					gps->Latitude =
							((simpleTelemtryData.lat < 0 ? simpleTelemtryData.lat * -1 : simpleTelemtryData.lat) * 600000);

					gps->GPSFix = 1;
					gps->Speed = simpleTelemtryData.speed * 3.6;

					gps->UTC = simpleTelemtryData.hour * 3600 + simpleTelemtryData.minute * 60 + simpleTelemtryData.second;
				}
				else
					gps->GPSFix = 0;

				gps->GPSStrength = (simpleTelemtryData.fixType == fixType_No) ? 0 : simpleTelemtryData.fixType - 1;

				gps->Vario = rintf((150 + simpleTelemtryData.vsi) / 0.8f);
				gps->Altitude = rintf((simpleTelemtryData.alt + 1500) / 0.8f);

				gps->BaroStatus = 1;
				slot += 7;
			}
				break;
			default:
				break;
			}
		}

		CoTickDelay(delay_ms(10));
		ticks = CoGetOSTime();
	}

}

static void module_futaba_rx_callback(uint8_t* id)
{
	struct Futaba_Session* session = (struct Futaba_Session*) id;

	uint8_t read = USART_ReceiveData(session->port->port);

	session->rxBuffer[session->rxPointer] = read;

	session->rxPointer = (session->rxPointer + 1) % MODULE_FUTABA_BUFFER_SIZE;

	if (session->rxPointer >= MODULE_FUTABA_FRAME_SIZE
			&& session->rxBuffer[session->rxPointer - MODULE_FUTABA_FRAME_SIZE] == MODULE_FUTABA_DATA_START
			&& (read == Futaba_Sections_FirstMain || read == Futaba_Sections_SecondMain || read == Futaba_Sections_ThridMain
					|| read == Futaba_Sections_ForthMain))
	{
		//Maybe Parse SBUS RC Channel Data
		session->rxPointer = 0;
		if (read == Futaba_Sections_FirstMain)
			session->currentSlot = 0;
		else if (read == Futaba_Sections_SecondMain)
			session->currentSlot = 8;
		else if (read == Futaba_Sections_ThridMain)
			session->currentSlot = 16;
		else if (read == Futaba_Sections_ForthMain)
			session->currentSlot = 24;

		memset(session->rxBuffer, 0, MODULE_FUTABA_BUFFER_SIZE);

		USART_DirectionModeCmd(session->port->port, USART_Mode_Rx, DISABLE);

		//Enable timer
		session->port->timer.tim->CNT = 0;
		TIM_SetAutoreload(session->port->timer.tim, MODULE_FUTABA_TIMING_START);
		TIM_Cmd(session->port->timer.tim, ENABLE);

		session->lastFrame = CoGetOSTime();
	}

}

static void module_futaba_tim_callback(uint8_t* id)
{
	struct Futaba_Session* session = (struct Futaba_Session*) id;

//reset timer value
	TIM_SetAutoreload(session->port->timer.tim, MODULE_FUTABA_TIMING_DATA);

	if (session->slotHasData & (1 << session->currentSlot))
	{
		FIFO64_write(session->txBuffer, Futaba_Slots[session->currentSlot]);
		FIFO64_write(session->txBuffer, session->slotData[session->currentSlot][0]);
		FIFO64_write(session->txBuffer, session->slotData[session->currentSlot][1]);
		USART_ITConfig(session->port->port, USART_IT_TXE, ENABLE);
	}

	session->currentSlot++;

	if ((session->currentSlot % MODULE_FUTABA_SLOTS_FRAME) == 0)
	{
		TIM_Cmd(session->port->timer.tim, DISABLE);
		USART_DirectionModeCmd(session->port->port, USART_Mode_Rx, ENABLE);
	}
}

static void module_futaba_tx_callback(uint8_t* id)
{
	struct Futaba_Session* session = (struct Futaba_Session*) id;

	if (!FIFO_available(session->txBuffer))
		USART_ITConfig(session->port->port, USART_IT_TXE, DISABLE);
	else
		USART_SendData(session->port->port, FIFO64_read(session->txBuffer));
}

