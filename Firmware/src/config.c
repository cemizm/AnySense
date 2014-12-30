/*
 * config.c
 *
 *  Created on: 22.08.2014
 *      Author: cem
 */

#include "config_priv.h"

struct configStruct configuration = {
		.configVersion = CONFIG_VERSION,
		.port1 = {
				.type = parser_hott,
				.parserConfig = { 0 },
		},
		.port2 = {
				.type = parser_frsky,
				.parserConfig = { 0 },
		},
};

void config_initialize()
{
	int32_t status = config_load();
	if (status < 0)
	{
		//corrupted data on flash, initialize and save
		config_save();
	}
}

int8_t config_load()
{
	uint32_t len = sizeof(struct configStruct);
	uint8_t* data = (uint8_t*) &configuration;

// We need to write 32 bit words, so the length should have been extended
// to an even multiple of 4 bytes, and should include 4 bytes for the 32 bit CRC.
	uint32_t nwords = (len / 4) + 1 + (len % 4 ? 1 : 0);
	uint32_t size = nwords * 4;

// Ensure that the length is not longer than the max size.
	if (size > CONFIG_LENGTH)
	{
		return -1;
	}

	uint8_t* base = (uint8_t*) CONFIG_BASEADRESS;

// Read the CRC.
	uint32_t crc_flash = *((uint32_t *) (base + size - 4));

// Calculate a 32 bit CRC of the data.
	uint32_t crc = crc_update(0xffffffff, base, len);
	if (crc != crc_flash)
	{
		return -2;
	}

	uint16_t version = *((uint16_t *) base);

	if (version != CONFIG_VERSION)
	{
		return -3;
	}

// Read the data from flash.
	memcpy(data, ((uint32_t*) base), len);

	return 0;
}

int8_t config_save()
{
	uint32_t len = sizeof(struct configStruct);
	uint8_t* data = (uint8_t*) &configuration;

// We need to write 32 bit words, so extend the length to be an even multiple of 4 bytes,
// and include 4 bytes for the 32 bit CRC.
	uint32_t nwords = (len / 4) + 1 + (len % 4 ? 1 : 0);
	uint32_t size = nwords * 4;

// Ensure that the length is not longer than the max size.
	if (size > CONFIG_LENGTH)
	{
		return -1;
	}

// Calculate a 32 bit CRC of the data.
	uint32_t crc = crc_update(0xffffffff, data, len);

// Unlock the Flash Program Erase controller
	FLASH_Unlock();

// See if we have to write the data.
	if ((memcmp(data, (uint8_t *) CONFIG_BASEADRESS, len) == 0)
			&& (memcmp((uint8_t *) &crc, (uint8_t *) CONFIG_BASEADRESS + size - 4, 4) == 0))
	{
		FLASH_Lock();
		return 0;
	}

// Erase page
	FLASH_Status fs = FLASH_ErasePage(CONFIG_BASEADRESS);
	if (fs != FLASH_COMPLETE)
	{ // error
		FLASH_Lock();
		return -2;
	}

// write 4 bytes at a time into program flash area (emulated EEPROM area)
	uint8_t *p1 = data;
	uint32_t *p3 = (uint32_t *) CONFIG_BASEADRESS;
	for (uint32_t i = 0; i < size; p3++)
	{
		uint32_t value = 0;

		if (i == (size - 4))
		{
			// write the CRC.
			value = crc;
			i += 4;
		}
		else
		{
			if (i < len)
			{
				value |= (uint32_t) *p1++ << 0;
			}
			else
			{
				value |= 0x000000ff;
			}
			i++;
			if (i < len)
			{
				value |= (uint32_t) *p1++ << 8;
			}
			else
			{
				value |= 0x0000ff00;
			}
			i++;
			if (i < len)
			{
				value |= (uint32_t) *p1++ << 16;
			}
			else
			{
				value |= 0x00ff0000;
			}
			i++;
			if (i < len)
			{
				value |= (uint32_t) *p1++ << 24;
			}
			else
			{
				value |= 0xff000000;
			}
			i++;
		}

		// write a 32-bit value
		fs = FLASH_ProgramWord((uint32_t) p3, value);
		if (fs != FLASH_COMPLETE)
		{
			FLASH_Lock();
			return -3;
		}
	}

// Lock the Flash Program Erase controller
	FLASH_Lock();

	return 0;

}
