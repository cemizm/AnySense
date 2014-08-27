//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdint.h>
#include "diag/Trace.h"
#include "stm32f0xx.h"
#include "stm32f0xx_conf.h"

//libs
#include "config.h"
#include "timer.h"
#include "hardware.h"

//naza
#include "naza.h"

//parser e.g. frsky
#include "SPort.h"

// ----- main() ---------------------------------------------------------------

static void start_parser(struct portParserStruct* parser,
		const struct hardware_port_cfg* port);

int main() {


	DEBUG_INIT()

	Timer_Initialize();

	config_initialize();
	naza_initialize();
	config_startManager();

	start_parser(&configuration.port1, &usart_port1);
	start_parser(&configuration.port2, &usart_port2);
	// Infinite loop
	while (1) {

	}
}

static void start_parser(struct portParserStruct* parser,
		const struct hardware_port_cfg* port) {
	switch (parser->type) {
	case parser_frsky:
		sport_start(port, parser->parserConfig);
		break;
	case parser_mavlink:
		//mavlink
		break;
	default:
		//unnmapped
		break;
	}
}

