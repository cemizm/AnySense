/*
 * simpletelemtry.h
 *
 *  Created on: 26.08.2014
 *      Author: cem
 */

#ifndef SIMPLETELEMTRY_H_
#define SIMPLETELEMTRY_H_

#include <stdint.h>

enum fixType
{
	fixType_No = 0, fixType_2D = 2, fixType_3D = 3, fixType_DGPS = 4
} fixType;

enum flightMode
{
	flightMode_manual = 0, flightMode_gps = 1, flightMode_failsafe = 2, flightMode_atti = 3
};


struct simpleTelemetry
{
	double lat; //latitude in degree decimal
	double lon; //longitude in degree decimal
	float alt; //altitude in m (from barometric sensor)
	float gpsAlt; //altitude in m (from GPS)
	float speed; //speed in m/s
	enum fixType fixType; //fix type (see fixType enum)
	uint8_t numSat; //number of satellites
	float heading; //heading in degrees (titlt compensated)
	float headingNc; //heading in degrees (not titlt compensated)
	float cog; //course over ground
	float vsi; //vertical speed (barometric) in m/s (a.k.a. climb speed)
	float gpsVsi; //vertical speed (from GPS) in m/s (a.k.a. climb speed)
	float hdop; //horizontal dilution of precision
	float vdop; //vertical dilution of precision
	float pitch; //pitch in rad
	float roll; //roll in rad
	uint8_t year; //year from GPS (minus 2000)
	uint8_t month; //month from GPS
	uint8_t day; //day from GPS
	uint8_t hour; //hour from GPS (Note that for time between 16:00 and 23:59 the hour returned from GPS module is actually 00:00 - 7:59)
	uint8_t minute; //minute from GPS
	uint8_t second; //second from GPS
	uint16_t battery; //battery voltage in mV
	int16_t rcIn[10]; //RC stick input (-1000~1000), use rcInChan_t enum to index the table
	int16_t throttle;
	enum flightMode mode; //flight mode (see mode_t enum)
	unsigned long long lastHeartbeat;

	//errors
	uint16_t packets_drop;
	uint16_t packets_lost;
	uint16_t packets_corrupted;
};

struct simpleTelemetry simpleTelemtryData;

void simpleTelemetry_initialize();
int16_t simpleTelemetry_getRCIn(uint8_t chan);

#endif /* SIMPLETELEMTRY_H_ */