/*
 * hardware_priv.h
 *
 *  Created on: 24.08.2014
 *      Author: cem
 */

#ifndef HARDWARE_PRIV_H_
#define HARDWARE_PRIV_H_

#include "hardware.h"

#ifdef STM32F072B

const struct hardware_port_cfg usart_port1 =
{
	.port = USART1,
	.af = GPIO_AF_0,
	.nvic_ch = USART1_IRQn,
	.tx =
	{
		.gpio = GPIOB,
		.pin_source = GPIO_PinSource6,
		.init =
		{
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_Speed = GPIO_Speed_Level_3,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_DOWN,
			.GPIO_Pin = GPIO_Pin_6,
		},
		.clock =
		{
			.cmd = RCC_AHBPeriphClockCmd,
			.periph = RCC_AHBPeriph_GPIOB,
		},
	},
	.rx =
	{
		.gpio = GPIOB,
		.pin_source = GPIO_PinSource7,
		.init =
		{
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_Speed = GPIO_Speed_Level_3,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_DOWN,
			.GPIO_Pin = GPIO_Pin_7,
		},
		.clock =
		{
			.cmd = RCC_AHBPeriphClockCmd,
			.periph = RCC_AHBPeriph_GPIOB,
		},
	},
	.clock =
	{
		.cmd = RCC_APB2PeriphClockCmd,
		.periph = RCC_APB2Periph_USART1,
	},
	.timer =
	{
		.tim = TIM16,
		.nvic_ch = TIM16_IRQn,
		.rcc =
		{
			.cmd = RCC_APB2PeriphClockCmd,
			.periph = RCC_APB2Periph_TIM16,
		},
	},
	.i2c =
	{
		.device = I2C1,
		.nvic_ch = I2C1_IRQn,
		.clock =
		{
			.cmd = RCC_APB1PeriphClockCmd,
			.periph = RCC_APB1Periph_I2C1,
		},
		.af = GPIO_AF_1,
	},
	.dma =
	{
		.channel = DMA1_Channel2,
		.nvic_ch = DMA1_Channel2_3_IRQn,
		.it_tc = DMA1_IT_TC2,
		.it_gl = DMA1_IT_GL2,
		.clock =
		{
			.cmd = RCC_AHBPeriphClockCmd,
			.periph = RCC_AHBPeriph_DMA1,
		},
	},
};

const struct hardware_port_cfg usart_port2 =
{
	.port = USART3,
	.af = GPIO_AF_1,
	.nvic_ch = USART3_4_IRQn,
	.tx =
	{
		.gpio = GPIOC,
		.pin_source = GPIO_PinSource4,
		.init =
		{
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_Speed = GPIO_Speed_Level_3,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_DOWN,
			.GPIO_Pin = GPIO_Pin_4,
		},
		.clock =
		{
			.cmd = RCC_AHBPeriphClockCmd,
			.periph = RCC_AHBPeriph_GPIOC,
		},
	},
	.rx =
	{
		.gpio = GPIOC,
		.pin_source = GPIO_PinSource5,
		.init =
		{
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_Speed = GPIO_Speed_Level_3,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_DOWN,
			.GPIO_Pin = GPIO_Pin_5,
		},
		.clock =
		{
			.cmd = RCC_AHBPeriphClockCmd,
			.periph = RCC_AHBPeriph_GPIOC,
		},
	},
	.clock =
	{
		.cmd = RCC_APB1PeriphClockCmd,
		.periph = RCC_APB1Periph_USART3,
	},
	.timer =
	{
		.tim = TIM17,
		.nvic_ch = TIM17_IRQn,
		.rcc =
		{
			.cmd = RCC_APB2PeriphClockCmd,
			.periph = RCC_APB2Periph_TIM17,
		},
	},
	.i2c = NULL,
};

#else

const struct hardware_port_cfg usart_port1 =
{
	.port = USART1,
	.af = GPIO_AF_0,
	.nvic_ch = USART1_IRQn,
	.tx =
	{
		.gpio = GPIOB,
		.pin_source = GPIO_PinSource6,
		.init =
		{
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_Speed = GPIO_Speed_Level_3,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_DOWN,
			.GPIO_Pin = GPIO_Pin_6,
		},
		.clock =
		{
			.cmd = RCC_AHBPeriphClockCmd,
			.periph = RCC_AHBPeriph_GPIOB,
		},
	}, .
	rx =
	{
		.gpio = GPIOB,
		.pin_source = GPIO_PinSource7,
		.init =
		{
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_Speed = GPIO_Speed_Level_3,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_DOWN,
			.GPIO_Pin = GPIO_Pin_7,
		},
		.clock =
		{
			.cmd = RCC_AHBPeriphClockCmd,
			.periph = RCC_AHBPeriph_GPIOB,
		},
	},
	.clock =
	{
		.cmd = RCC_APB2PeriphClockCmd,
		.periph = RCC_APB2Periph_USART1,
	},
	.timer =
	{
		.tim = TIM16,
		.nvic_ch = TIM16_IRQn,
		.rcc =
		{
			.cmd = RCC_APB2PeriphClockCmd,
			.periph = RCC_APB2Periph_TIM16,
		},
	},
	.i2c =
	{
		.device = I2C1,
		.nvic_ch = I2C1_IRQn,
		.clock =
		{
			.cmd = RCC_APB1PeriphClockCmd,
			.periph = RCC_APB1Periph_I2C1,
		},
		.af = GPIO_AF_1,
	},
	.dma =
	{
		.channel = DMA1_Channel2,
		.nvic_ch = DMA1_Channel2_3_IRQn,
		.it_tc = DMA1_IT_TC2,
		.it_gl = DMA1_IT_GL2,
		.clock =
		{
			.cmd = RCC_AHBPeriphClockCmd,
			.periph = RCC_AHBPeriph_DMA1,
		},
	},
};

const struct hardware_port_cfg usart_port2 =
{
	.port = USART2,
	.af = GPIO_AF_1,
	.nvic_ch = USART2_IRQn,
	.tx =
	{
		.gpio = GPIOA,
		.pin_source = GPIO_PinSource14,
		.init =
		{
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_Speed = GPIO_Speed_Level_3,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_DOWN,
			.GPIO_Pin = GPIO_Pin_14,
		},
		.clock =
		{
			.cmd = RCC_AHBPeriphClockCmd,
			.periph = RCC_AHBPeriph_GPIOA,
		},
	},
	.rx =
	{
		.gpio = GPIOA,
		.pin_source = GPIO_PinSource15,
		.init =
		{
			.GPIO_Mode = GPIO_Mode_AF,
			.GPIO_Speed = GPIO_Speed_Level_3,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd = GPIO_PuPd_DOWN,
			.GPIO_Pin = GPIO_Pin_15,
		},
		.clock =
		{
			.cmd = RCC_AHBPeriphClockCmd,
			.periph = RCC_AHBPeriph_GPIOA,
		},
	},
	.clock =
	{
		.cmd = RCC_APB1PeriphClockCmd,
		.periph = RCC_APB1Periph_USART2,
	},
	.timer =
	{
		.tim = TIM17,
		.nvic_ch = TIM17_IRQn,
		.rcc =
		{
			.cmd = RCC_APB2PeriphClockCmd,
			.periph = RCC_APB2Periph_TIM17,
		},
	},
	.i2c =
	{
		.device = NULL,
	},
	.dma =
	{
		.channel = NULL,
	},
};

#endif

struct IRQ_CallbackInfoStruct
{
	const struct hardware_port_cfg* port;
	IRQ_Callback rx_callback;
	IRQ_Callback tx_callback;
	IRQ_Callback rto_callback;
	IRQ_Callback timer_callback;
	IRQ_Callback dma_callback;
	IRQ_Callback i2c_callback;
	uint8_t* id;
};

static struct IRQ_CallbackInfoStruct cbInfoPort1 =
{	.port = &usart_port1};
static struct IRQ_CallbackInfoStruct cbInfoPort2 =
{	.port = &usart_port2};

#endif /* HARDWARE_PRIV_H_ */
