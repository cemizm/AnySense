/*
 * module_spektrum.c
 *
 *  Created on: 06.08.2015
 *      Author: cem
 */

#include "module_spektrum_priv.h"

void module_spektrum_initializeConfig(void* config)
{
	struct Spektrum_Config* cfg = (struct Spektrum_Config*) config;

	if (cfg->version != 1)
	{
		cfg->version = 1;
		cfg->cells = 3;
		cfg->lowVoltage = 15;
		cfg->maxCapacity = 3500;
		cfg->active_sensors = spektrum_sensor_current | spektrum_sensor_gps | spektrum_sensor_powerbox | spektrum_sensor_vario;
	}
}

void module_spektrum_start(const struct hardware_port_cfg* port, uint8_t* config)
{

	if (port->i2c.device == NULL)
		return;

	struct Spektrum_Session* session = (struct Spektrum_Session*) malloc(sizeof(struct Spektrum_Session));
	session->next = NULL;
	LL_APPEND(spektrum_sessions, session);

	session->port = port;
	session->config = (struct Spektrum_Config*) config;

	memset(session->enabledSensors, 0, MODULE_SPEKTRUM_SENSOR_ENABLED);
	uint8_t index = 0;
	if (session->config->active_sensors & spektrum_sensor_current)
		session->enabledSensors[index++] = MODULE_SPEKTRUM_SENSOR_CURRENT;

	if (session->config->active_sensors & spektrum_sensor_powerbox)
		session->enabledSensors[index++] = MODULE_SPEKTRUM_SENSOR_POWERBOX;

	if (session->config->active_sensors & spektrum_sensor_gps)
	{
		session->enabledSensors[index++] = MODULE_SPEKTRUM_SENSOR_GPS1;
		session->enabledSensors[index++] = MODULE_SPEKTRUM_SENSOR_GPS2;
	}

	//if (session->config->active_sensors & spektrum_sensor_vario)
	//	session->enabledSensors[index++] = MODULE_SPEKTRUM_SENSOR_VARIO;

	port->rx.clock.cmd(port->rx.clock.periph, ENABLE);
	port->tx.clock.cmd(port->tx.clock.periph, ENABLE);

	port->dma.clock.cmd(port->dma.clock.periph, ENABLE);

	port->i2c.clock.cmd(port->i2c.clock.periph, ENABLE);

	/* GPIO Initialization */
	GPIO_InitTypeDef initStr = port->rx.init;
	initStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(port->rx.gpio, &initStr);

	initStr = port->tx.init;
	initStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(port->tx.gpio, &initStr);

	/* Alternate function configuration */
	GPIO_PinAFConfig(port->rx.gpio, port->rx.pin_source, port->i2c.af);
	GPIO_PinAFConfig(port->tx.gpio, port->tx.pin_source, port->i2c.af);

	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(port->dma.channel);
	DMA_StructInit(&DMA_InitStructure);
	DMA_InitStructure.DMA_BufferSize = MODULE_SPEKTRUM_FRAME_SIZE;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) session->data.txBuffer;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&port->i2c.device->TXDR);
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(port->dma.channel, &DMA_InitStructure);

	DMA_ITConfig(port->dma.channel, DMA_IT_TC, ENABLE);

	NVIC_InitTypeDef def;
	def.NVIC_IRQChannelCmd = ENABLE;
	def.NVIC_IRQChannel = port->dma.nvic_ch;
	def.NVIC_IRQChannelPriority = PRIORITY_TELEMETRY;
	NVIC_Init(&def);

	I2C_DeInit(port->i2c.device);

	I2C_InitTypeDef initDef;
	I2C_StructInit(&initDef);
	initDef.I2C_Ack = I2C_Ack_Disable;
	initDef.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	initDef.I2C_DigitalFilter = 0x00;
	initDef.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	initDef.I2C_Mode = I2C_Mode_I2C;
	initDef.I2C_OwnAddress1 = MODULE_SPEKTRUM_SENSOR_CURRENT << 1;

	I2C_NumberOfBytesConfig(port->i2c.device, 0);
	I2C_SlaveByteControlCmd(port->i2c.device, DISABLE);
	I2C_StretchClockCmd(port->i2c.device, ENABLE);
	I2C_Init(port->i2c.device, &initDef);
	I2C_OwnAddress2Config(port->i2c.device, 0x00, I2C_OA2_Mask07);
	I2C_DualAddressCmd(port->i2c.device, ENABLE);
	I2C_AcknowledgeConfig(port->i2c.device, DISABLE);

	def.NVIC_IRQChannel = port->i2c.nvic_ch;
	NVIC_Init(&def);

	session->ready = CoCreateFlag(Co_TRUE, Co_FALSE);
	session->task_id = CoCreateTask(module_spektrum_task, session, 0, &session->task_stack[MODULE_SPEKTRUM_TASK_STACK - 1],
			MODULE_SPEKTRUM_TASK_STACK);

	hardware_register_i2c_callback(session->port, &module_spektrum_irq_i2c, (uint8_t*) session);
	hardware_register_dma_callback(session->port, &module_spektrum_irq_dma, (uint8_t*) session);

	I2C_ITConfig(port->i2c.device, I2C_IT_ADDR, ENABLE);

	I2C_DMACmd(port->i2c.device, I2C_DMAReq_Tx, ENABLE);
	I2C_Cmd(port->i2c.device, ENABLE);
}

void module_spektrum_task(void* pData)
{
	struct Spektrum_Session* session = (struct Spektrum_Session*) pData;

	uint8_t ledEnabled = session->port == &usart_port1;

	U64 ticks = CoGetOSTime();
	U64 next_toggle = ticks - 1;

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

		if (CoWaitForSingleFlag(session->ready, 0) == E_OK)
		{
			memset(session->data.txBuffer, 0, MODULE_SPEKTRUM_FRAME_SIZE);

			session->data.txBuffer[0] = session->sensor;

			if (session->sensor == MODULE_SPEKTRUM_SENSOR_CURRENT)
			{
				//simpleTelemtryData.current = 184;
				session->data.current_t.current = ltob16((uint16_t )(simpleTelemtryData.current / 0.19679144f));
			}
			else if (session->sensor == MODULE_SPEKTRUM_SENSOR_POWERBOX)
			{
				session->data.powerbox.voltage1 = ltob16(simpleTelemtryData.battery / 10);
				session->data.powerbox.voltage2 = ltob16(simpleTelemetry_getLowestCell(session->config->cells) / 10);
				session->data.powerbox.capacity1 = ltob16(simpleTelemtryData.capacity_current);

				session->data.powerbox.voltage1_alarm =
						simpleTelemetry_getCalculatedPercentage(session->config->cells) < session->config->lowVoltage ? 1 : 0;

				session->data.powerbox.capacity1_alarm =
						simpleTelemtryData.capacity_current > session->config->maxCapacity ? 1 : 0;

			}
			else if (session->sensor == MODULE_SPEKTRUM_SENSOR_GPS1)
			{
				session->data.gps1.alt = toBCD16(simpleTelemtryData.alt * 10);
				module_spektrum_fill_gps(&(session->data.gps1.lat), simpleTelemtryData.lat);
				module_spektrum_fill_gps(&(session->data.gps1.lon), simpleTelemtryData.lon);
				session->data.gps1.isNorth = simpleTelemtryData.lat > 0 ? 1 : 0;
				session->data.gps1.isEast = simpleTelemtryData.lon > 0 ? 1 : 0;
				session->data.gps1.lonExceed = simpleTelemtryData.lon > 100 ? 1 : 0;
				session->data.gps1.cog = toBCD16(simpleTelemtryData.heading * 10);

			}
			else if (session->sensor == MODULE_SPEKTRUM_SENSOR_GPS2)
			{
				session->data.gps2.speed = toBCD16(simpleTelemtryData.speed * 19.438);
				session->data.gps2.sats = toBCD8(simpleTelemtryData.numSat);
				session->data.gps2.alt = toBCD8(simpleTelemtryData.alt / 1000);
				session->data.gps2.hours = toBCD8(simpleTelemtryData.hour);
				session->data.gps2.minutes = toBCD8(simpleTelemtryData.minute);
				session->data.gps2.seconds = toBCD8(simpleTelemtryData.second);
			}

			DMA_SetCurrDataCounter(session->port->dma.channel, MODULE_SPEKTRUM_FRAME_SIZE);
			DMA_Cmd(session->port->dma.channel, ENABLE);
		}

		ticks = CoGetOSTime();
	}

}

static void module_spektrum_irq_i2c(uint8_t* pHandle)
{
	struct Spektrum_Session* session = (struct Spektrum_Session*) pHandle;

	if (I2C_GetITStatus(session->port->i2c.device, I2C_IT_ADDR))
	{
		session->sensor = (I2C_GetAddressMatched(session->port->i2c.device) >> 1);

		if (I2C_GetTransferDirection(session->port->i2c.device) == I2C_Direction_Receiver)
		{
			for (uint8_t i = 0; i < MODULE_SPEKTRUM_SENSOR_ENABLED; i++)
			{
				if (session->enabledSensors[i] == session->sensor)
				{
					isr_SetFlag(session->ready);
					I2C_ClearITPendingBit(session->port->i2c.device, I2C_ICR_ADDRCF);
					return;
				}
			}

			I2C_SoftwareResetCmd(session->port->i2c.device);
		}
	}
}

static void module_spektrum_irq_dma(uint8_t* pHandle)
{
	struct Spektrum_Session* session = (struct Spektrum_Session*) pHandle;

	if (DMA_GetITStatus(session->port->dma.it_tc))
	{
		DMA_ClearITPendingBit(session->port->dma.it_gl);
		DMA_Cmd(session->port->dma.channel, DISABLE);
	}
}

static uint8_t toBCD8(uint8_t dec)
{
	uint8_t result = 0;
	int shift = 0;

	while (dec)
	{
		result += (dec % 10) << shift;
		dec = dec / 10;
		shift += 4;
	}
	return result;
}

static uint16_t toBCD16(uint16_t dec)
{
	uint16_t result = 0;
	int shift = 0;

	while (dec)
	{
		result += (dec % 10) << shift;
		dec = dec / 10;
		shift += 4;
	}
	return result;
}

static void module_spektrum_fill_gps(struct Spektrum_GPS_Degree* deg, double dec)
{
	if (dec < 0)
		dec *= -1;

	double degree = 0;
	double minute = modf(dec, &degree) * 60;
	double second = modf(minute, &minute) * 10000;
	modf(second, &second);

	deg->deg = toBCD8((uint8_t) degree);
	deg->min = toBCD8((uint8_t) minute);
	deg->minfrac = toBCD16((uint16_t) second);
}
