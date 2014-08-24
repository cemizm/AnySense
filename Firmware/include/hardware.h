/*
 * hardware.h
 *
 *  Created on: 22.08.2014
 *      Author: cem
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

#include "stm32f0xx.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"

#ifdef STM32F072B

#define DEBUG_INIT() {										\
	GPIO_InitTypeDef GPIO_InitStructure; 					\
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE); 	\
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			\
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;		\
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			\
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		\
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 ;	\
	GPIO_Init(GPIOC, &GPIO_InitStructure);					\
}

#define DEBUG_TOGGLE(pin)				{ GPIO_WriteBit(GPIOC, pin, GPIO_ReadOutputDataBit(GPIOC, pin) ? Bit_RESET : Bit_SET); }
#define DEBUG_TOGGLE_RED()				DEBUG_TOGGLE(GPIO_Pin_6)
#define DEBUG_TOGGLE_BLUE()				DEBUG_TOGGLE(GPIO_Pin_7)
#define DEBUG_TOGGLE_ORANGE()			DEBUG_TOGGLE(GPIO_Pin_8)
#define DEBUG_TOGGLE_GREEN()			DEBUG_TOGGLE(GPIO_Pin_9)

#else

#define DEBUG_INIT()

#define DEBUG_TOGGLE(pin)
#define DEBUG_TOGGLE_RED()
#define DEBUG_TOGGLE_BLUE()
#define DEBUG_TOGGLE_ORANGE()
#define DEBUG_TOGGLE_GREEN()

#endif

typedef void (*IRQ_Callback)(uint8_t*);

typedef void (*ClockCmd)(uint32_t, FunctionalState);

struct hardware_irq_cfg_struct {
    IRQ_Callback callback;
    uint32_t flags;
    NVIC_InitTypeDef init;
};

struct hardware_rcc_cfg_struct {
	ClockCmd 	cmd;
	uint32_t	periph;
};

struct hardware_gpio_cfg_struct {
    GPIO_TypeDef     *gpio;
    GPIO_InitTypeDef init;
    uint16_t pin_source;
    struct hardware_rcc_cfg_struct		clock;
};

struct hardware_port_cfg {
	USART_TypeDef*						port;
	uint8_t 							af;
	uint8_t								nvic_ch;
    struct hardware_gpio_cfg_struct 	rx;
    struct hardware_gpio_cfg_struct 	tx;
    struct hardware_rcc_cfg_struct		clock;
};

//availible ports

extern const struct hardware_port_cfg usart_port1;
extern const struct hardware_port_cfg usart_port2;

void hardware_register_callback(const struct hardware_port_cfg* port,
		IRQ_Callback rx_callback, IRQ_Callback tx_callback, uint8_t* id);
void hardware_unregister_callback(const struct hardware_port_cfg* port);


#endif /* HARDWARE_H_ */
