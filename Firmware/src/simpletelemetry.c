/*
 * simpletelemetry.c
 *
 *  Created on: 30.08.2014
 *      Author: cem
 */

#include "simpletelemtry.h"

void simpleTelemetry_initialize()
{
	simpleTelemtryData.lat = 0;
	simpleTelemtryData.lon = 0;
	simpleTelemtryData.alt = 0;
	simpleTelemtryData.homeLat = 0;
	simpleTelemtryData.homeLon = 0;
	simpleTelemtryData.homeAltBaro = 20;
	simpleTelemtryData.gpsAlt = 0;
	simpleTelemtryData.speed = 0;
	simpleTelemtryData.fixType = fixType_No;
	simpleTelemtryData.numSat = 0;
	simpleTelemtryData.heading = 0;
	simpleTelemtryData.headingNc = 0;
	simpleTelemtryData.cog = 0;
	simpleTelemtryData.vsi = 0;
	simpleTelemtryData.gpsVsi = 0;
	simpleTelemtryData.hdop = 0;
	simpleTelemtryData.vdop = 0;
	simpleTelemtryData.pitch = 0;
	simpleTelemtryData.roll = 0;
	simpleTelemtryData.year = 0;
	simpleTelemtryData.month = 0;
	simpleTelemtryData.day = 0;
	simpleTelemtryData.hour = 0;
	simpleTelemtryData.minute = 0;
	simpleTelemtryData.second = 0;
	simpleTelemtryData.battery = 0;
	simpleTelemtryData.mode = flightMode_failsafe;
	simpleTelemtryData.lastHeartbeat = 0;
}

int16_t simpleTelemetry_getRCIn(uint8_t chan)
{
	return simpleTelemtryData.rcIn[chan] / 2 + 1500;
}

uint8_t simpleTelemetry_isStickConfig()
{
	if (simpleTelemetry_getRCIn(2) < 1900)
		return 0;

	if (simpleTelemetry_getRCIn(5) < 1900)
		return 0;

	if (simpleTelemetry_getRCIn(1) > 1100 && simpleTelemetry_getRCIn(1) < 1900)
		return 0;

	if (simpleTelemetry_getRCIn(3) > 1100 && simpleTelemetry_getRCIn(3) < 1900)
		return 0;

	return 1;
}


uint8_t simpleTelemetry_stickConfigPosition()
{
	if (simpleTelemetry_getRCIn(2) > 1550)
		return 0;

	if(simpleTelemetry_getRCIn(2) > 1150) // Oben
	{
		if(simpleTelemetry_getRCIn(1) < 1100) //links
			return 0;
		else if(simpleTelemetry_getRCIn(1) > 1900) //rechts
			return 0;

	}
	else // unten
	{
		if(simpleTelemetry_getRCIn(1) < 1100) //links
			return 1;
		else if(simpleTelemetry_getRCIn(1) > 1450 && simpleTelemetry_getRCIn(1) < 1550) //mitte
			return 0;
		else if(simpleTelemetry_getRCIn(1) > 1900) //rechts
			return 3;
	}

	return 0;
}
