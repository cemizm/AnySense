/*
 * Sport->c
 *
 *  Created on: 22.08.2014
 *      Author: cem
 */

#include "SPort_priv.h"

void sport_start(const struct hardware_port_cfg* port, uint8_t* config)
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
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(port->port, &USART_InitStructure);
	USART_InvPinCmd(port->port, USART_InvPin_Rx | USART_InvPin_Tx, ENABLE);
	USART_HalfDuplexCmd(port->port, ENABLE);
	USART_OverrunDetectionConfig(port->port, USART_OVRDetection_Disable);
	USART_Cmd(port->port, ENABLE);

	SPortSession* session = (SPortSession*) malloc(sizeof(SPortSession));

	session->currentState = WAITING_START_FRAME;
	session->port = port;
	session->next = NULL;
	session->config = (struct SPort_Config*) config;
	session->currentValue = 0;
	session->tmp.Header = SPORT_DATAFRAME;

	LL_APPEND(sessions, session);

	/* Initialize & Enable Interrupts */
	hardware_register_callback(port, &RX_Callback, &TX_Callback,
			(uint8_t*) session);

	NVIC_InitTypeDef def;
	def.NVIC_IRQChannelCmd = ENABLE;
	def.NVIC_IRQChannel = port->nvic_ch;
	def.NVIC_IRQChannelPriority = 3;
	NVIC_Init(&def);

	USART_ITConfig(port->port, USART_IT_RXNE, ENABLE);

}

static void RX_Callback(uint8_t* id) {

	SPortSession* session = (SPortSession*) id;

	uint8_t read = USART_ReceiveData(session->port->port);

	switch (session->currentState) {
	case WAITING_START_FRAME:
		if (read == SPORT_STARTSTOP)
			session->currentState = WAITING_SESNOR_ID;
		break;
	case WAITING_SESNOR_ID:
		if (read == session->config->sesnorId) {
			DEBUG_TOGGLE_BLUE();
			switch (session->currentValue) {
			case 0:
				session->tmp.Id = SENSOR_VFAS;
				session->tmp.Value = 1108;
				SendPacket(session, &session->tmp);
				break;
			}

			session->currentValue++;
			if (session->currentValue > 0)
				session->currentValue = 0;
		}
		session->currentState = WAITING_START_FRAME;
		break;
	default:
		session->currentState = WAITING_START_FRAME;
		break;
	}

}

static void TX_Callback(uint8_t* id) {
	SPortSession* session = (SPortSession*) id;

	if (!FIFO_available(session->send_buffer))
		USART_ITConfig(session->port->port, USART_IT_TXE, DISABLE);
	else
		USART_SendData(session->port->port, FIFO64_read(session->send_buffer));

}

static void SendPacket(SPortSession* session, struct SPort_Packet* packet) {
	uint8_t* data = (uint8_t*) packet;
	uint8_t byte;
	short crc = 0;

	for (uint8_t i = 0; i < 7; i++) {
		byte = data[i];
		FIFO64_write(session->send_buffer, byte);
		crc += byte; //0-1FF
		crc += crc >> 8; //0-100
		crc &= 0x00FF;
		crc += crc >> 8; //0-0FF
		crc &= 0x00FF;
	}
	FIFO64_write(session->send_buffer, 0xFF - crc);

	USART_ITConfig(session->port->port, USART_IT_TXE, ENABLE);
}
