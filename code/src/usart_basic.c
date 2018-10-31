/**
 * \file
 *
 * \brief USART basic driver.
 *
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *
 */
#include <compiler.h>
#include <clock_config.h>
#include <usart_basic.h>
#include <atomic.h>

int8_t USART_0_init()
{

	// Module is in UART mode

	/* Enable USART0 */
	PRR &= ~(1 << PRUSART0);

#define BAUD 9600

#include <utils/setbaud.h>

	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

	// UCSR0A = 0 << MPCM0; /* Multi-processor Communication Mode: disabled */

	UCSR0B = 1 << RXCIE0    /* RX Complete Interrupt Enable: enabled */
	         | 0 << UDRIE0  /* USART Data Register Empty Interupt Enable: disabled */
	         | 1 << RXEN0   /* Receiver Enable: enabled */
	         | 1 << TXEN0   /* Transmitter Enable: enabled */
	         | 0 << UCSZ02; /*  */

	// UCSR0C = 0 << USBS0 /* USART Stop Bit Select: disabled */;

	return 0;
}

void USART_0_enable()
{
	UCSR0B |= ((1 << TXEN0) | (1 << RXEN0));
}

void USART_0_enable_rx()
{
	UCSR0B |= (1 << RXEN0);
}

void USART_0_enable_tx()
{
	UCSR0B |= (1 << TXEN0);
}

void USART_0_disable()
{
	UCSR0B &= ~((1 << TXEN0) | (1 << RXEN0));
}

bool USART_0_is_tx_ready()
{
	return (UCSR0A & (1 << UDRE0));
}

bool USART_0_is_rx_ready()
{
	return (UCSR0A & (1 << RXC0));
}

bool USART_0_is_tx_busy()
{
	return (!(UCSR0A & (1 << TXC0)));
}

uint8_t USART_0_read()
{
	while (!(UCSR0A & (1 << RXC0)))
		;
	return UDR0;
}

void USART_0_write(const uint8_t data)
{
	while (!(UCSR0A & (1 << UDRE0)))
		;
	UDR0 = data;
}
