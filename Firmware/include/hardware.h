/*
 * hardware.h
 *
 *  Created on: 22.08.2014
 *      Author: cem
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

#include "stddef.h"

#include "stm32f0xx.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_i2c.h"
#include "stm32f0xx_dma.h"

#define delay_ms(ms) ms
#define delay_sec(sec) delay_ms(sec * 1000)
#define tick_ms(tick) tick
#define tick_sec(tick) tick_ms(tick / 1000)

#define ltob16(num) ((num>>8) | (num<<8))

#define PRIORITY_CAN		2
#define PRIORITY_MAVLINK	3
#define PRIORITY_TELEMETRY	1

#define CASSERT(predicate, file) _impl_CASSERT_LINE(predicate,__LINE__,file)

#define _impl_PASTE(a,b) a##b
#define _impl_CASSERT_LINE(predicate, line, file) \
    typedef char _impl_PASTE(assertion_failed_##file##_,line)[2*!!(predicate)-1];

#define PROTOCOL_SESSION_SIZE				4096

#ifdef STM32F072B

#define LED_PORT		GPIOC
#define LED_PORT_CLOCK	RCC_AHBPeriph_GPIOC
#define LED_PIN_RED		GPIO_Pin_6
#define LED_PIN_GREEN	GPIO_Pin_9

#define I2C_STRETCH_PORT					GPIOB
#define I2C_STRETCH_PORT_CLOCK				RCC_AHBPeriph_GPIOB
#define I2C_STRETCH_PIN						GPIO_Pin_6

/*  NAZA_CAN RX Pin Definitions */
#define NAZA_CAN_GPIO_RX_Pin				GPIO_Pin_8
#define NAZA_CAN_GPIO_RX_Port				GPIOB
#define NAZA_CAN_GPIO_RX_Clock				RCC_AHBPeriph_GPIOB
#define NAZA_CAN_GPIO_RX_ClockCmd			RCC_AHBPeriphClockCmd
#define NAZA_CAN_GPIO_RX_AF					GPIO_AF_4
#define NAZA_CAN_GPIO_RX_AF_Pin				GPIO_PinSource8

/*  NAZA_CAN TX Pin Definitions */
#define NAZA_CAN_GPIO_TX_Pin				GPIO_Pin_9
#define NAZA_CAN_GPIO_TX_Port				GPIOB
#define NAZA_CAN_GPIO_TX_Clock				RCC_AHBPeriph_GPIOB
#define NAZA_CAN_GPIO_TX_ClockCmd			RCC_AHBPeriphClockCmd
#define NAZA_CAN_GPIO_TX_AF					GPIO_AF_4
#define NAZA_CAN_GPIO_TX_AF_Pin				GPIO_PinSource9

#else

#define LED_PORT		GPIOA
#define LED_PORT_CLOCK	RCC_AHBPeriph_GPIOA
#define LED_PIN_RED		GPIO_Pin_0
#define LED_PIN_GREEN	GPIO_Pin_1

#define I2C_STRETCH_PORT					GPIOB
#define I2C_STRETCH_PORT_CLOCK				RCC_AHBPeriph_GPIOB
#define I2C_STRETCH_PIN						GPIO_Pin_6

/*  NAZA_CAN RX Pin Definitions */
#define NAZA_CAN_GPIO_RX_Pin				GPIO_Pin_11
#define NAZA_CAN_GPIO_RX_Port				GPIOA
#define NAZA_CAN_GPIO_RX_Clock				RCC_AHBPeriph_GPIOA
#define NAZA_CAN_GPIO_RX_ClockCmd			RCC_AHBPeriphClockCmd
#define NAZA_CAN_GPIO_RX_AF					GPIO_AF_4
#define NAZA_CAN_GPIO_RX_AF_Pin				GPIO_PinSource11

/*  NAZA_CAN TX Pin Definitions */
#define NAZA_CAN_GPIO_TX_Pin				GPIO_Pin_12
#define NAZA_CAN_GPIO_TX_Port				GPIOA
#define NAZA_CAN_GPIO_TX_Clock				RCC_AHBPeriph_GPIOA
#define NAZA_CAN_GPIO_TX_ClockCmd			RCC_AHBPeriphClockCmd
#define NAZA_CAN_GPIO_TX_AF					GPIO_AF_4
#define NAZA_CAN_GPIO_TX_AF_Pin				GPIO_PinSource12

#endif

typedef void (*IRQ_Callback)(uint8_t*);

typedef void (*ClockCmd)(uint32_t, FunctionalState);

struct hardware_irq_cfg_struct
{
	IRQ_Callback callback;
	uint32_t flags;
	NVIC_InitTypeDef init;
};

struct hardware_rcc_cfg_struct
{
	ClockCmd cmd;
	uint32_t periph;
};

struct hardware_gpio_cfg_struct
{
	GPIO_TypeDef *gpio;
	GPIO_InitTypeDef init;
	uint16_t pin_source;
	struct hardware_rcc_cfg_struct clock;
};

struct hardware_tim_cfg_struct
{
	TIM_TypeDef* tim;
	struct hardware_rcc_cfg_struct rcc;
	uint8_t nvic_ch;
};

struct hardware_i2c_cfg_struct
{
	I2C_TypeDef* device;
	uint8_t af;
	uint8_t nvic_ch;
	struct hardware_rcc_cfg_struct clock;
};

struct hardware_dma_cfg_struct
{
	DMA_Channel_TypeDef* channel;
	uint8_t nvic_ch;
	uint32_t it_tc;
	uint32_t it_gl;
	struct hardware_rcc_cfg_struct clock;
};

struct hardware_port_cfg
{
	USART_TypeDef* port;
	uint8_t af;
	uint8_t nvic_ch;
	struct hardware_gpio_cfg_struct rx; // Usart RX, i2c SDA
	struct hardware_gpio_cfg_struct tx; // USART TX, i2c SCL
	struct hardware_rcc_cfg_struct clock;
	struct hardware_tim_cfg_struct timer;
	struct hardware_i2c_cfg_struct i2c;
	struct hardware_dma_cfg_struct dma;
};

//availible ports

extern const struct hardware_port_cfg usart_port1;
extern const struct hardware_port_cfg usart_port2;

void hardware_register_callback2(const struct hardware_port_cfg* port, IRQ_Callback rx_callback, IRQ_Callback tx_callback,
		IRQ_Callback timer_callback, uint8_t* id);

void hardware_register_callback(const struct hardware_port_cfg* port, IRQ_Callback rx_callback, IRQ_Callback tx_callback,
		uint8_t* id);
void hardware_unregister_callback(const struct hardware_port_cfg* port);

void hardware_register_i2c_callback(const struct hardware_port_cfg* port, IRQ_Callback callback, uint8_t* id);
void hardware_unregister_i2c_callback(const struct hardware_port_cfg* port);
void hardware_register_dma_callback(const struct hardware_port_cfg* port, IRQ_Callback callback, uint8_t* id);
void hardware_unregister_dma_callback(const struct hardware_port_cfg* port);

inline void hardware_led_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(LED_PORT_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_1;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = LED_PIN_RED | LED_PIN_GREEN;
	GPIO_Init(LED_PORT, &GPIO_InitStructure);
}

inline void hardware_led_toggle_red()
{
	GPIO_WriteBit(LED_PORT, LED_PIN_RED, GPIO_ReadOutputDataBit(LED_PORT, LED_PIN_RED) ? Bit_RESET : Bit_SET);
}

inline void hardware_led_off_red()
{
	GPIO_WriteBit(LED_PORT, LED_PIN_RED, Bit_RESET);
}

inline void hardware_led_on_red()
{
	GPIO_WriteBit(LED_PORT, LED_PIN_RED, Bit_SET);
}

inline void hardware_led_toggle_green()
{
	GPIO_WriteBit(LED_PORT, LED_PIN_GREEN, GPIO_ReadOutputDataBit(LED_PORT, LED_PIN_GREEN) ? Bit_RESET : Bit_SET);
}

inline void hardware_i2c_stretch()
{

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(I2C_STRETCH_PORT_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_1;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = I2C_STRETCH_PIN;
	GPIO_Init(I2C_STRETCH_PORT, &GPIO_InitStructure);

	GPIO_WriteBit(I2C_STRETCH_PORT, I2C_STRETCH_PIN, Bit_RESET);
}
inline void hardware_i2c_free()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(I2C_STRETCH_PORT_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_1;
	GPIO_InitStructure.GPIO_Pin = I2C_STRETCH_PIN;
	GPIO_Init(I2C_STRETCH_PORT, &GPIO_InitStructure);
}

#endif /* HARDWARE_H_ */
