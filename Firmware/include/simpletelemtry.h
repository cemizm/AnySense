/*
 * simpletelemtry.h
 *
 *  Created on: 26.08.2014
 *      Author: cem
 */

#ifndef SIMPLETELEMTRY_H_
#define SIMPLETELEMTRY_H_

#include <stdint.h>

enum fixType {
	fixType_No = 0, fixType_2D = 2, fixType_3D = 3, fixType_DGPS = 4
} fixType;
enum flightMode {
	flightMode_manual = 0, flightMode_gps = 1, flightMode_failsafe = 2, flightMode_atti = 3
};

struct simpleTelemetry {
	double lat; //latitude in degree decimal
	double lon; //longitude in degree decimal
	double alt; //altitude in m (from barometric sensor)
	double gpsAlt; //altitude in m (from GPS)
	double speed; //speed in m/s
	enum fixType fixType; //fix type (see fixType enum)
    uint8_t numSat; //number of satellites
    double heading; //heading in degrees (titlt compensated)
    double headingNc; //heading in degrees (not titlt compensated)
    double cog; //course over ground
    double vsi; //vertical speed (barometric) in m/s (a.k.a. climb speed)
    double gpsVsi; //vertical speed (from GPS) in m/s (a.k.a. climb speed)
    double hdop; //horizontal dilution of precision
    double vdop; //vertical dilution of precision
    int8_t pitch; //pitch in degrees
    int8_t roll; //roll in degrees
    uint8_t year; //year from GPS (minus 2000)
    uint8_t month;//month from GPS
    uint8_t day; //day from GPS
    uint8_t hour; //hour from GPS (Note that for time between 16:00 and 23:59 the hour returned from GPS module is actually 00:00 - 7:59)
    uint8_t minute; //minute from GPS
    uint8_t second; //second from GPS
    uint16_t battery; //battery voltage in mV
    int16_t rcIn[10]; //RC stick input (-1000~1000), use rcInChan_t enum to index the table
    enum flightMode mode; //flight mode (see mode_t enum)
};

struct simpleTelemetry simpleTelemtryData;

void simpleTelemetry_initialize();

#endif /* SIMPLETELEMTRY_H_ */
