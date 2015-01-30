/*
 * simpletelemetry.c
 *
 *  Created on: 30.08.2014
 *      Author: cem
 */

#include "simpletelemtry.h"
#include "hardware.h"
#include "string.h"

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
	simpleTelemtryData.current = 0;
	simpleTelemtryData.temp1 = 0;
	simpleTelemtryData.temp2 = 0;
	simpleTelemtryData.cellCount = 0;
	simpleTelemtryData.capacity_current = 0;
	simpleTelemtryData.percentage_charge = 0;


	memset(simpleTelemtryData.cells, 0, SIMPLE_TELEMETRY_CELLS);
	memset(simpleTelemtryData.rcIn, 0, SIMPLE_TELEMETRY_RC);

#ifdef DEBUG

	simpleTelemtryData.cellCount = 5;
	simpleTelemtryData.cells[0] = 3800;
	simpleTelemtryData.cells[1] = 4200;
	simpleTelemtryData.cells[2] = 4200;
	simpleTelemtryData.cells[3] = 3400;
	simpleTelemtryData.cells[4] = 3700;
#endif

}

int16_t simpleTelemetry_getRCIn(uint8_t chan)
{
	return simpleTelemtryData.rcIn[chan] / 2 + 1500;
}

uint8_t simpleTelemetry_isAlive()
{
	return (simpleTelemtryData.lastHeartbeat + delay_sec(3)) > CoGetOSTime();
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
	int16_t fmSwitch = simpleTelemetry_getRCIn(4);

	if(fmSwitch < 1475)
		return 1;
	else if(fmSwitch < 1875)
		return 2;
	else
		return 3;
}

uint16_t simpleTelemetry_getLowestCell()
{
	uint16_t cell = simpleTelemtryData.cells[0];

	for(uint8_t i=1;cell<simpleTelemtryData.cellCount; i++)
	{
		if(simpleTelemtryData.cells[i] < cell)
			cell = simpleTelemtryData.cells[i];
	}

	return cell;
}
