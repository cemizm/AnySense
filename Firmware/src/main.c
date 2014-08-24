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

//parser e.g. frsky
#include "SPort.h"

static void test1000();

// ----- main() ---------------------------------------------------------------

static void configManager_Finished();
static void start_parser(struct portParserStruct* parser,
		const struct hardware_port_cfg* port);

int main() {

	Timer_Initialize();

	config_initialize();
	config_startManager(&configManager_Finished);

	// Infinite loop
	while (1) {

	}
}

static void configManager_Finished() {

	DEBUG_INIT();

	Timer_Register(&test1000, 1000);

	start_parser(&configuration.port1, &usart_port1);
	start_parser(&configuration.port2, &usart_port2);
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

static void test1000() {

	DEBUG_TOGGLE_RED();

}

