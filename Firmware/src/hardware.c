/*
 * hardware.c
 *
 *  Created on: 23.08.2014
 *      Author: cem
 */

#include "hardware_priv.h"
#include "CoOS.h"

#include <stdlib.h>

void hardware_register_callback(const struct hardware_port_cfg* port, IRQ_Callback rx_callback, IRQ_Callback tx_callback,
		uint8_t* id)
{
	hardware_register_callback2(port, rx_callback, tx_callback, NULL, id);
}

void hardware_register_callback2(const struct hardware_port_cfg* port, IRQ_Callback rx_callback, IRQ_Callback tx_callback,
		IRQ_Callback timer_callback, uint8_t* id)
{

	if (port == &usart_port1)
	{
		cbInfoPort1.rx_callback = rx_callback;
		cbInfoPort1.tx_callback = tx_callback;
		cbInfoPort1.timer_callback = timer_callback;
		cbInfoPort1.id = id;
	}
	else if (port == &usart_port2)
	{
		cbInfoPort2.rx_callback = rx_callback;
		cbInfoPort2.tx_callback = tx_callback;
		cbInfoPort2.timer_callback = timer_callback;
		cbInfoPort2.id = id;
	}
}

void hardware_register_i2c_callback(const struct hardware_port_cfg* port, IRQ_Callback callback, uint8_t* id)
{
	if (port == &usart_port1)
	{
		cbInfoPort1.i2c_callback = callback;
		cbInfoPort1.id = id;
	}
}

void hardware_unregister_i2c_callback(const struct hardware_port_cfg* port)
{
	if (port == &usart_port1)
	{
		cbInfoPort1.i2c_callback = NULL;
	}
}

void hardware_register_dma_callback(const struct hardware_port_cfg* port, IRQ_Callback callback, uint8_t* id)
{
	if (port == &usart_port1)
	{
		cbInfoPort1.dma_callback = callback;
		cbInfoPort1.id = id;
	}
}

void hardware_unregister_dma_callback(const struct hardware_port_cfg* port)
{
	if (port == &usart_port1)
	{
		cbInfoPort1.dma_callback = NULL;
	}
}

void hardware_register_timeout_callback(const struct hardware_port_cfg* port, IRQ_Callback callback, uint8_t* id)
{
	if (port == &usart_port1)
	{
		cbInfoPort1.rto_callback = callback;
		cbInfoPort1.id = id;
	}
	else if (port == &usart_port2)
	{
		cbInfoPort2.rto_callback = callback;
		cbInfoPort2.id = id;
	}
}

void hardware_unregister_timeout_callback(const struct hardware_port_cfg* port)
{
	if (port == &usart_port1)
	{
		cbInfoPort2.rto_callback = NULL;
	}
	else if (port == &usart_port2)
	{
		cbInfoPort2.rto_callback = NULL;
	}
}

void hardware_unregister_callback(const struct hardware_port_cfg* port)
{
	if (port == &usart_port1)
	{
		cbInfoPort1.rx_callback = NULL;
		cbInfoPort1.tx_callback = NULL;
		cbInfoPort1.timer_callback = NULL;
		cbInfoPort1.id = NULL;
	}
	else if (port == &usart_port2)
	{
		cbInfoPort2.rx_callback = NULL;
		cbInfoPort2.tx_callback = NULL;
		cbInfoPort2.timer_callback = NULL;
		cbInfoPort2.id = NULL;
	}
}

void Generic_USART_IRQHandler(struct IRQ_CallbackInfoStruct* cb)
{
	if (USART_GetITStatus(cb->port->port, USART_IT_RXNE) != RESET)
	{
		if (cb->rx_callback == NULL)
			USART_RequestCmd(cb->port->port, USART_Request_RXFRQ, ENABLE);
		else
			cb->rx_callback(cb->id);
	}
	if (USART_GetITStatus(cb->port->port, USART_IT_TXE) != RESET)
	{
		if (cb->tx_callback == NULL)
			USART_RequestCmd(cb->port->port, USART_Request_TXFRQ, ENABLE);
		else
			cb->tx_callback(cb->id);
	}
	if (USART_GetITStatus(cb->port->port, USART_IT_RTO) != RESET)
	{
		cb->rto_callback(cb->id);
	}
}
void Generic_TIM_IRQHandler(struct IRQ_CallbackInfoStruct* cb)
{
	if (TIM_GetITStatus(cb->port->timer.tim, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(cb->port->timer.tim, TIM_IT_Update);

		if (cb->timer_callback != NULL)
			cb->timer_callback(cb->id);
	}

}

void Generic_I2C_IRQHandler(struct IRQ_CallbackInfoStruct* cb)
{
	cb->i2c_callback(cb->id);
}

void Generic_DMA_IRQHandler(struct IRQ_CallbackInfoStruct* cb)
{
	cb->dma_callback(cb->id);
}

#ifdef STM32F072B

void USART1_IRQHandler(void)
{
	CoEnterISR();
	Generic_USART_IRQHandler(&cbInfoPort1);
	CoExitISR();
}

void USART3_4_IRQHandler(void)
{
	CoEnterISR();
	Generic_USART_IRQHandler(&cbInfoPort2);
	CoExitISR();
}

#else

void USART1_IRQHandler(void)
{
	CoEnterISR();
	Generic_USART_IRQHandler(&cbInfoPort1);
	CoExitISR();
}

void USART2_IRQHandler(void)
{
	CoEnterISR();
	Generic_USART_IRQHandler(&cbInfoPort2);
	CoExitISR();
}

#endif

void TIM16_IRQHandler(void)
{
	CoEnterISR();
	Generic_TIM_IRQHandler(&cbInfoPort1);
	CoExitISR();
}

void TIM17_IRQHandler(void)
{
	CoEnterISR();
	Generic_TIM_IRQHandler(&cbInfoPort2);
	CoExitISR();
}

void I2C1_IRQHandler(void)
{
	CoEnterISR();
	Generic_I2C_IRQHandler(&cbInfoPort1);
	CoExitISR();
}

void DMA1_Channel2_3_IRQHandler(void)
{
	CoEnterISR();
	Generic_DMA_IRQHandler(&cbInfoPort1);
	CoExitISR();

}
