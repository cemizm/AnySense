/*
 * simpletelemtry.h
 *
 *  Created on: 26.08.2014
 *      Author: cem
 */

#ifndef SIMPLETELEMTRY_H_
#define SIMPLETELEMTRY_H_

#include <stdint.h>
#include "CoOS.h"
#include "math.h"

#define SIMPLE_TELEMETRY_RC		10
#define SIMPLE_TELEMETRY_CELLS	12

#define MATH_MAP(A,B,X,C,D)   				((uint8_t) (((float)(X-A)/(B-A)) * (D-C) + C))

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
	double homeLat;      // home lattitude (radians)
	double homeLon;      // home longitude (radians)
	float homeAltBaro; // home altitude from barometric sensor plus 20m (meters)
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
	int16_t rcIn[SIMPLE_TELEMETRY_RC]; //RC stick input (-1000~1000), use rcInChan_t enum to index the table
	int16_t throttle;
	enum flightMode mode; //flight mode (see mode_t enum)
	uint8_t armed;
	U64 lastHeartbeat;
	float current; //current consumption in 0.1 A
	float temp1;
	float temp2;
	uint16_t cells[SIMPLE_TELEMETRY_CELLS]; //battery voltage in mV
	uint8_t cellCount;
	uint16_t capacity_current; // current capacity (3115mAh)
	uint8_t percentage_charge; // percentage of charge (62%)
	uint32_t flightime; //flightime in seconds

	//errors
	uint16_t packets_drop;
	uint16_t packets_lost;
	uint16_t packets_corrupted;

};

struct simpleTelemetry simpleTelemtryData;

void simpleTelemetry_initialize();
int16_t simpleTelemetry_getRCIn(uint8_t chan);
uint8_t simpleTelemetry_isStickConfig();
uint8_t simpleTelemetry_stickConfigPosition();
uint8_t simpleTelemetry_isAlive();

__inline__ uint8_t simpleTelemetry_isHomePointSet()
{
	return simpleTelemtryData.homeLon != 0 && simpleTelemtryData.homeLat != 0;
}

__inline__ uint8_t simpleTelemetry_validGPS()
{
	return simpleTelemtryData.fixType >= fixType_2D;
}

__inline__ uint16_t simpleTelemetry_getLowestCell(uint8_t cellCount)
{
	if (simpleTelemtryData.cellCount == 0)
	{
		if (cellCount == 0)
			return 0;

		return simpleTelemtryData.battery / cellCount;
	}

	uint16_t lowest = simpleTelemtryData.cells[0];

	for (uint8_t i = 1; i < simpleTelemtryData.cellCount; i++)
	{
		if (simpleTelemtryData.cells[i] < lowest)
			lowest = simpleTelemtryData.cells[i];
	}

	return lowest;
}

__inline__ uint8_t simpleTelemetry_getPercentage(uint16_t cellVoltage)
{
	if (simpleTelemtryData.percentage_charge > 0)
		return simpleTelemtryData.percentage_charge;

	if (cellVoltage > 4150) // 4,15V 100%
		return 100;
	else if (cellVoltage > 4100) // 4,10V 90%
		return MATH_MAP(4100, 4150, cellVoltage, 90, 100);
	else if (cellVoltage > 3970) //3,97V 80% - 90%
		return MATH_MAP(3970, 4100, cellVoltage, 80, 90);
	else if (cellVoltage > 3920) //3,92V 70% - 80%,
		return MATH_MAP(3920, 3970, cellVoltage, 70, 80);
	else if (cellVoltage > 3870) //3,87 60% - 70%
		return MATH_MAP(3870, 3920, cellVoltage, 60, 70);
	else if (cellVoltage > 3830) //3,83 50% - 60%
		return MATH_MAP(3830, 3870, cellVoltage, 50, 60);
	else if (cellVoltage > 3790) //3,79V 40%-50%
		return MATH_MAP(3790, 3830, cellVoltage, 40, 50);
	else if (cellVoltage > 3750) //3,75V 30%-40%
		return MATH_MAP(3750, 3790, cellVoltage, 30, 40);
	else if (cellVoltage > 3700) //3,70V 20%-30%
		return MATH_MAP(3700, 3750, cellVoltage, 20, 30);
	else if (cellVoltage > 3600) //3,60V 10%-20%
		return MATH_MAP(3600, 3700, cellVoltage, 10, 20);
	else if (cellVoltage > 3300) //3,30V 5% - 10%
		return MATH_MAP(3300, 3600, cellVoltage, 5, 10);
	else if (cellVoltage > 3000) //3,00V 0% - 5%
		return MATH_MAP(3000, 3300, cellVoltage, 0, 5);

	return 0;
}

__inline__ uint8_t simpleTelemetry_getCalculatedPercentage(uint8_t cellCount)
{
	return simpleTelemetry_getPercentage(simpleTelemetry_getLowestCell(cellCount));
}

__inline__ double simpleTelemetry_getDistance()
{
	if (simpleTelemetry_validGPS() && simpleTelemetry_isHomePointSet())
	{

		double delta = (simpleTelemtryData.lon - simpleTelemtryData.homeLon) * (M_PI / 180);
		double sdlong = sin(delta);
		double cdlong = cos(delta);
		double lat1 = simpleTelemtryData.lat * (M_PI / 180);
		double lat2 = simpleTelemtryData.homeLat * (M_PI / 180);
		double slat1 = sin(lat1);
		double clat1 = cos(lat1);
		double slat2 = sin(lat2);
		double clat2 = cos(lat2);
		delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
		delta = pow(delta, 2);
		delta += pow(clat2 * sdlong, 2);
		delta = sqrt(delta);
		double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
		delta = atan2(delta, denom);

		return delta * 6372795;
	}

	return 0;
}

__inline__ double simpleTelemetry_calculateAngle()
{
	if (simpleTelemetry_validGPS() && simpleTelemetry_isHomePointSet())
	{
		double off_y = simpleTelemtryData.homeLon - simpleTelemtryData.lon;
		double off_x = cos(M_PI / 180 * simpleTelemtryData.lon) * (simpleTelemtryData.homeLat - simpleTelemtryData.lat);
		double bearing = atan2(off_y, off_x) / M_PI * 180;

		if (bearing < 0)
			bearing += 360;

		return bearing;
	}
	return 0;
}

__inline__ float simpleTelemetry_getRelativeHeight()
{
	return simpleTelemtryData.alt - (simpleTelemtryData.homeAltBaro - 20);
}

#endif /* SIMPLETELEMTRY_H_ */
