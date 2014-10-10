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

const struct hardware_port_cfg usart_port1 = {
		.port = USART1,
		.af = GPIO_AF_1,
		.nvic_ch = USART1_IRQn,
		.tx = {
				.gpio = GPIOA,
				.pin_source = GPIO_PinSource9,
				.init = {
						.GPIO_Mode = GPIO_Mode_AF,
						.GPIO_Speed = GPIO_Speed_Level_3,
						.GPIO_OType = GPIO_OType_PP,
						.GPIO_PuPd = GPIO_PuPd_DOWN,
						.GPIO_Pin = GPIO_Pin_9,
				},
				.clock = {
						.cmd = RCC_AHBPeriphClockCmd,
						.periph = RCC_AHBPeriph_GPIOA,
				},
		}, .
		rx = {
				.gpio = GPIOA,
				.pin_source = GPIO_PinSource10,
				.init = {
						.GPIO_Mode = GPIO_Mode_AF,
						.GPIO_Speed = GPIO_Speed_Level_3,
						.GPIO_OType = GPIO_OType_PP,
						.GPIO_PuPd = GPIO_PuPd_DOWN,
						.GPIO_Pin = GPIO_Pin_10,
				},
				.clock = {
						.cmd = RCC_AHBPeriphClockCmd,
						.periph = RCC_AHBPeriph_GPIOA,
				},
		},
		.clock = {
				.cmd = RCC_APB2PeriphClockCmd,
				.periph = RCC_APB2Periph_USART1,
		},
};

const struct hardware_port_cfg usart_port2 = {
		.port = USART3,
		.af = GPIO_AF_1,
		.nvic_ch = USART3_4_IRQn,
		.tx = {
				.gpio = GPIOC,
				.pin_source = GPIO_PinSource4,
				.init = {
						.GPIO_Mode = GPIO_Mode_AF,
						.GPIO_Speed = GPIO_Speed_Level_3,
						.GPIO_OType = GPIO_OType_PP,
						.GPIO_PuPd = GPIO_PuPd_DOWN,
						.GPIO_Pin = GPIO_Pin_4,
				},
				.clock = {
						.cmd = RCC_AHBPeriphClockCmd,
						.periph = RCC_AHBPeriph_GPIOC,
				},
		},
		.rx = {
				.gpio = GPIOC,
				.pin_source = GPIO_PinSource5,
				.init = {
						.GPIO_Mode = GPIO_Mode_AF,
						.GPIO_Speed = GPIO_Speed_Level_3,
						.GPIO_OType = GPIO_OType_PP,
						.GPIO_PuPd = GPIO_PuPd_DOWN,
						.GPIO_Pin = GPIO_Pin_5,
				},
				.clock = {
						.cmd = RCC_AHBPeriphClockCmd,
						.periph = RCC_AHBPeriph_GPIOC,
				},
		},
		.clock = {
				.cmd = RCC_APB1PeriphClockCmd,
				.periph = RCC_APB1Periph_USART3,
		},
};

#else

const struct hardware_port_cfg usart_port1 = {
		.port = USART1,
		.af = GPIO_AF_0,
		.nvic_ch = USART1_IRQn,
		.tx = {
				.gpio = GPIOB,
				.pin_source = GPIO_PinSource6,
				.init = {
						.GPIO_Mode = GPIO_Mode_AF,
						.GPIO_Speed = GPIO_Speed_Level_3,
						.GPIO_OType = GPIO_OType_PP,
						.GPIO_PuPd = GPIO_PuPd_DOWN,
						.GPIO_Pin = GPIO_Pin_6,
				},
				.clock = {
						.cmd = RCC_AHBPeriphClockCmd,
						.periph = RCC_AHBPeriph_GPIOB,
				},
		}, .
		rx = {
				.gpio = GPIOB,
				.pin_source = GPIO_PinSource7,
				.init = {
						.GPIO_Mode = GPIO_Mode_AF,
						.GPIO_Speed = GPIO_Speed_Level_3,
						.GPIO_OType = GPIO_OType_PP,
						.GPIO_PuPd = GPIO_PuPd_DOWN,
						.GPIO_Pin = GPIO_Pin_7,
				},
				.clock = {
						.cmd = RCC_AHBPeriphClockCmd,
						.periph = RCC_AHBPeriph_GPIOB,
				},
		},
		.clock = {
				.cmd = RCC_APB2PeriphClockCmd,
				.periph = RCC_APB2Periph_USART1,
		},
};

const struct hardware_port_cfg usart_port2 = {
		.port = USART2,
		.af = GPIO_AF_1,
		.nvic_ch = USART2_IRQn,
		.tx = {
				.gpio = GPIOA,
				.pin_source = GPIO_PinSource14,
				.init = {
						.GPIO_Mode = GPIO_Mode_AF,
						.GPIO_Speed = GPIO_Speed_Level_3,
						.GPIO_OType = GPIO_OType_PP,
						.GPIO_PuPd = GPIO_PuPd_DOWN,
						.GPIO_Pin = GPIO_Pin_14,
				},
				.clock = {
						.cmd = RCC_AHBPeriphClockCmd,
						.periph = RCC_AHBPeriph_GPIOA,
				},
		},
		.rx = {
				.gpio = GPIOA,
				.pin_source = GPIO_PinSource15,
				.init = {
						.GPIO_Mode = GPIO_Mode_AF,
						.GPIO_Speed = GPIO_Speed_Level_3,
						.GPIO_OType = GPIO_OType_PP,
						.GPIO_PuPd = GPIO_PuPd_DOWN,
						.GPIO_Pin = GPIO_Pin_15,
				},
				.clock = {
						.cmd = RCC_AHBPeriphClockCmd,
						.periph = RCC_AHBPeriph_GPIOA,
				},
		},
		.clock = {
				.cmd = RCC_APB1PeriphClockCmd,
				.periph = RCC_APB1Periph_USART2,
		},
};

#endif



struct IRQ_CallbackInfoStruct {
	const struct hardware_port_cfg* port;
	IRQ_Callback rx_callback;
	IRQ_Callback tx_callback;
	uint8_t* id;
};

static struct IRQ_CallbackInfoStruct cbInfoPort1 = { .port = &usart_port1 };
static struct IRQ_CallbackInfoStruct cbInfoPort2 = { .port = &usart_port2 };


#endif /* HARDWARE_PRIV_H_ */
