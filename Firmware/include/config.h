/*
 * config.h
 *
 *  Created on: 22.08.2014
 *      Author: cem
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>

#define CONFIG_PARSER_LENGTH	128
#define CONFIG_VERSION			1
#define FIRMWARE_VERSION		0x00010000

typedef enum parserTypeEnum
{
	parser_none = 0, parser_frsky = 1, parser_mavlink = 2, parser_hott = 3, parser_jeti = 4, parser_futaba = 5,
} parserType;

struct portParserStruct
{
	parserType type;
	uint8_t parserConfig[CONFIG_PARSER_LENGTH];
};

struct configStruct
{
	uint16_t configVersion;
	struct portParserStruct port1;
	struct portParserStruct port2;
};

struct configStruct configuration;

void config_initialize();

int8_t config_load();
int8_t config_save();

#endif /* CONFIG_H_ */
