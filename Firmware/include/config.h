/*
 * config.h
 *
 *  Created on: 22.08.2014
 *      Author: cem
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>

#define CONFIG_PARSER_LENGTH	512

typedef enum parserTypeEnum {
	parser_none = 0, parser_frsky = 1, parser_mavlink = 2, parser_hott = 3, parser_jeti = 4,
}parserType;

struct portParserStruct {
	parserType type;
	uint8_t parserConfig[CONFIG_PARSER_LENGTH];
};

struct configStruct {
	struct portParserStruct port1;
	struct portParserStruct port2;
};

struct configStruct configuration;

void config_initialize();
void config_startManager();


#endif /* CONFIG_H_ */
