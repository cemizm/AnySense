/*
 * module_hott_priv.h
 *
 *  Created on: 12.09.2014
 *      Author: cem
 */

#ifndef MODULE_HOTT_PRIV_H_
#define MODULE_HOTT_PRIV_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "arm_math.h"

#include "module_hott.h"
#include "simpletelemtry.h"
#include "utlist.h"
#include "CoOS.h"

#define MODULE_HOTT_MSG_BUFFER_SIZE 		173
#define MODULE_HOTT_TX_INIT_DELAY 			delay_ms(5)
#define MODULE_HOTT_TX_DELAY 				delay_ms(2)
#define MODULE_HOTT_TASK_STACK				256
#define MODULE_HOTT_TEXT_ROWS				8
#define MODULE_HOTT_TEXT_COLUMNS			21
#define MODULE_HOTT_TEXT_PAGES				5

#define MODULE_HOTT_MSG_START				0x7C
#define MODULE_HOTT_TEXT_START				0x7B
#define MODULE_HOTT_MSG_STOP				0x7D
#define MODULE_HOTT_VOLTAGE_ALARMS			3
#define MODULE_HOTT_DISTANCE_ALARMS			3

#define MATH_MAP(A,B,X,C,D)   				((uint8_t) (((float)(X-A)/(B-A)) * (D-C) + C))

#define HOTT_INIT_MSG(msg, sensor)	{ 		\
	msg.start = MODULE_HOTT_MSG_START;		\
	msg.request_sensor_id = sensor;			\
	msg.sensor_id = (uint8_t)(sensor << 4);	\
	msg.stop = MODULE_HOTT_MSG_STOP;		\
}
#define HOTT_INIT_TEXT(msg)	{ 				\
	msg.start = MODULE_HOTT_TEXT_START;		\
	memset(msg.text, 0x20, MODULE_HOTT_TEXT_ROWS * MODULE_HOTT_TEXT_COLUMNS); \
	msg.stop = MODULE_HOTT_MSG_STOP;		\
}

/** ###### HoTT module sensor Ids ###### */
enum hott_sensor_id
{
	hott_sensor_id_gam = 0x8D,  		// General Air Module ID
	hott_senor_id_eam = 0x8E,		// Electric Air Module ID
	hott_sensor_id_gps = 0x8A,		// GPS Module ID
	hott_sensor_id_vario = 0x89,		// Vario Sensor Module ID
};

enum hott_request_type
{
	hott_request_type_binary = 0x80, hott_request_type_text = 0x7F,
};

enum hott_key
{
	hott_key_left = 0x07, hott_key_set = 0x09, hott_key_up = 0x0B, hott_key_down = 0x0D, hott_key_right = 0x0E,
};

//Hott alarm, depending on sensor
enum hott_alarm_tone
{
	hott_alarm_tone_none = 0x00,		//No alarm
	hott_alarm_tone_unk_01 = 0x01,
	hott_alarm_tone_diff_neg_1 = 0x02,		//Negative Difference
	hott_alarm_tone_diff_neg_2 = 0x03,		//Negative Difference
	hott_alarm_tone_unk_04 = 0x04,
	hott_alarm_tone_unk_05 = 0x05,
	hott_alarm_tone_s1_temp_min = 0x06,		//Min. Sensor 1 temp.
	hott_alarm_tone_s2_temp_min = 0x07,		//Min. Sensor 2 temp.
	hott_alarm_tone_s1_temp_max = 0x08,		//Max. Sensor 1 temp.
	hott_alarm_tone_s2_temp_max = 0x09,		//Max. Sensor 2 temp.
	hott_alarm_tone_s1_volt_max = 0x0A,		//Max. Sensor 1 voltage
	hott_alarm_tone_s2_volt_max = 0x0B,		//Max. Sensor 2 voltage
	hott_alarm_tone_unk_0C = 0x0C,
	hott_alarm_tone_diff_pos_1 = 0x0D,		//Positive Difference 2 B
	hott_alarm_tone_diff_pos_2 = 0x0E,		//Positive Difference 1 C
	hott_alarm_tone_alt_min = 0x0F,		//Min. Altitude O
	hott_alarm_tone_volt_min = 0x10,		//Min. Power Voltage P
	hott_alarm_tone_cell_min = 0x11,		//Min. Cell voltage Q
	hott_alarm_tone_s1_volt_min = 0x12,		//Min. Sens. 1 voltage R
	hott_alarm_tone_s2_volt_min = 0x13,		//Min. Sens. 2 voltage R
	hott_alarm_tone_rpm_min = 0x14,		//Min. RPM
	hott_alarm_tone_unk_15 = 0x15,
	hott_alarm_tone_cap_max = 0x16,		//Max. used capacity
	hott_alarm_tone_cur_max = 0x17,		//Max. Current
	hott_alarm_tone_pwr_max = 0x18,		//Max. Power Voltage X
	hott_alarm_tone_rpm_max = 0x19,		//Maximum RPM Y
	hott_alarm_tone_alt_max = 0x1A  	//Max. Altitude Z
};

//Hott alarm on display
enum hott_alarm_msg
{
	hott_msg_alarm_cells = 0x0001,	//all cell voltage
	hott_msg_alarm_volt1 = 0x0002,	//Battery 1
	hott_msg_alarm_volt2 = 0x0004,	//Battery 2
	hott_msg_alarm_temp1 = 0x0008,	//Temperature 1
	hott_msg_alarm_temp2 = 0x0010,	//Temperature 2
	hott_msg_alarm_fuel = 0x0020,	//Fuel
	hott_msg_alarm_curr = 0x0040,	//mAh
	hott_msg_alarm_alt1 = 0x0080,	//Altitude
	hott_msg_alarm_main_curr = 0x0100,	//main power current
	hott_msg_alarm_main_volt = 0x0200,	//main power voltage
	hott_msg_alarm_alt2 = 0x0400,	//Altitude
	hott_msg_alarm_ms = 0x0800,	//m/s
	hott_msg_alarm_m3s = 0x1000,	//m/3s
	hott_msg_alarm_unknown1 = 0x2000,	//unknown
	hott_msg_alarm_unknown2 = 0x4000,	//unknown
	hott_msg_alarm_on_active = 0x8000,	//"ON" sign/text msg active
};

enum hott_gps_fix
{
	hott_gps_fix_nofix = 0x2d,				//no Fix
	hott_gps_fix_2D = 0x32,					//2d Fix
	hott_gps_fix_3D = 0x33,					//3d Fix
	hott_gps_fix_DGPS = 0x44,				//DGPS Fix
};

// GENERAL AIR MODULE
struct hott_msg_gam
{
	uint8_t start;								//#01 start uint8_t constant value 0x7c
	enum hott_sensor_id request_sensor_id;		//#02 GAM sensor id. constant value 0x8d=GENRAL AIR MODULE
	enum hott_alarm_tone warning_beeps;			//#03 warning tone
	uint8_t sensor_id;							//#04 constant value 0xd0
	enum hott_alarm_msg alarm;					//#05 alarm bitmask
	uint8_t cell[6];							//#07 Voltage Cells (in 2 mV increments, 210 == 4.20 V)
	uint16_t battery1;							//#13 LSB battery 1 voltage LSB value. 0.1V steps. 50 = 5.5V only pos. voltages
	uint16_t battery2;							//#15 LSB battery 2 voltage LSB value. 0.1V steps. 50 = 5.5V only pos. voltages
	uint8_t temp1;								//#17 Temperature 1. Offset of 20. a value of 20 = 0°C
	uint8_t temp2;								//#18 Temperature 2. Offset of 20. a value of 20 = 0°C
	uint8_t fuel_percent;						//#19 Fuel capacity in %. Values 0--100
	uint16_t fuel_ml;							//#20 LSB Fuel in ml scale. Full = 65535!
	uint16_t rpm;                      		  	//#22 RPM in 10 RPM steps. 300 = 3000rpm
	uint16_t altitude;                  		//#24 altitude in meters. offset of 500, 500 = 0m
	uint16_t climbrate_L;						//#26 climb rate in 0.01m/s. Value of 30000 = 0.00 m/s
	uint8_t climbrate3s;						//#28 climb rate in m/3sec. Value of 120 = 0m/3sec
	uint16_t current;							//#29 current in 0.1A steps 100 == 10,0A
	uint16_t main_voltage;	           		    //#31 LSB Main power voltage using 0.1V steps 100 == 10,0V
	uint16_t batt_cap;                 		  	//#33 LSB used battery capacity in 10mAh steps
	uint16_t speed;                    		  	//#35 LSB (air?) speed in km/h(?) we are using ground speed here per default
	uint8_t min_cell_volt;              	    //#37 minimum cell voltage in 2mV steps. 124 = 2,48V
	uint8_t min_cell_volt_num;              	//#38 number of the cell with the lowest voltage
	uint16_t rpm2;                         		//#39 LSB 2nd RPM in 10 RPM steps. 100 == 1000rpm
	uint8_t general_error_number;				//#41 General Error Number (Voice Error == 12)
	uint8_t pressure;							//#42 High pressure up to 16bar. 0,1bar scale. 20 == 2.0bar
	uint8_t version;						//#43 version number (uint8_ts 35 .43 new but not yet in the record in the display!)
	uint8_t stop;								//#44 stop uint8_t 0x7D
}__attribute__((packed, aligned(1)));

//GPS MODULE
struct hott_msg_gps
{
	uint8_t start;							//#01 start uint8_t constant value 0x7c
	enum hott_sensor_id request_sensor_id;	//#02 GPS Sesnor ID
	enum hott_alarm_tone warning_beeps;		//#03 warning tone
	uint8_t sensor_id;						//#04 GPS Text message ID
	enum hott_alarm_msg alarm;				//#05 alarm bitmask
	uint8_t flightDirection; 				//#07: 119 = Flightdir./dir. 1 = 2°; 0°(North), 90°(East), 180°(South), 270°(West)
	uint16_t GPSSpeed; 						//#08: 8 = /GPS speed low byte 8km/h
	uint8_t latNS; 							//#10: 00 = N = 48°39’988 #10 north = 0, south = 1
	uint16_t latMin; 						//#11: 231 0xE7 = 0x12E7 = 4839
	uint16_t latSec; 						//#13: 171 220 = 0xDC = 0x03DC = 0988
	uint8_t lonEW; 							//#15: 000  = E= 9° 25’9360 east = 0, west = 1
	uint16_t lonMin; 						//#16: 150 157 = 0x9D = 0x039D = 0925
	uint16_t lonSec; 						//#18: 056 144 = 0x90 0x2490 = 9360
	uint16_t distance; 						//#20: 027 123 = /distance low byte 6 = 6 m
	uint16_t altitude; 						//#22: 243 244 = /Altitude low byte 500 = 0m
	uint16_t climbrate1s;                   //#24: climb rate in 0.01m/s. Value of 30000 = 0.00 m/s
	uint8_t climbrate3s;					//#26: climb rate in m/3sec. Value of 120 = 0m/3sec
	uint8_t GPSNumSat;						//#27: GPS.Satelites (number of satelites) (1 byte)
	uint8_t GPSFixChar; 					//#28: GPS.FixChar. (GPS fix character. display, if DGPS, 2D oder 3D) (1 byte)
	uint8_t HomeDirection; 					//#29: HomeDirection (direction from starting point to Model position) (1 byte)
	uint8_t angleXdirection; 				//#30: angle x-direction (1 byte)
	uint8_t angleYdirection; 				//#31: angle y-direction (1 byte)
	uint8_t angleZdirection; 				//#32: angle z-direction (1 byte)
	uint8_t gps_time_h;  					//#33 UTC time hours
	uint8_t gps_time_m;  					//#34 UTC time minutes
	uint8_t gps_time_s;  					//#35 UTC time seconds
	uint8_t gps_time_sss;  					//#36 UTC time milliseconds
	uint16_t msl_altitude;  				//#37 mean sea level altitude
	uint8_t vibration; 						//#39: vibration (1 bytes)
	uint8_t Ascii4; 						//#40: 00 ASCII Free Character [4]
	uint8_t Ascii5; 						//#41: 00 ASCII Free Character [5]
	uint8_t GPS_fix; 						//#42: 00 ASCII Free Character [6], we use it for GPS FIX
	uint8_t version; 						//#43: 00 version number
	uint8_t stop;	 						//#44: 0x7D Ende byte
}__attribute__((packed, aligned(1)));

//general text message 7 lines 20 characters + \0
struct hott_msg_text
{
	uint8_t start;							//#01 Starting constant value == 0x7b
	uint8_t esc;                            //#02 Escape (higher-ranking menu in text mode or Text mode leave)
											//0x00 to stay normal
											//0x01 to exit
											//I will send 2 times, so the ESCAPE works really well, so two data frames with 0x01 in byte 2
	enum hott_alarm_tone warning_beeps;
	uint8_t text[MODULE_HOTT_TEXT_ROWS][MODULE_HOTT_TEXT_COLUMNS];	//ASCII text to display Bit 7 = 1 -> Inverse character
	uint8_t stop;													//#172 constant value 0x7D
}__attribute__((packed, aligned(1)));

union hott_msg
{
	uint8_t buffer[MODULE_HOTT_MSG_BUFFER_SIZE];
	struct hott_msg_gam gam;
	struct hott_msg_gps gps;

	struct hott_msg_text text;
};

struct hott_voltage_alarm
{
	uint8_t level;					//warning level in percent (0=disable)
	enum hott_alarm_tone tone;		//warning tone if value <= level
	uint8_t interval;				//interval in seconds repeating tone
	uint8_t repeat;					//repeating tone (0=infinite)
	uint8_t invert;					//should text be inverterd (0=no invert)
}__attribute__((packed, aligned(1)));

struct hott_distance_alarm
{
	uint16_t level;					//warning level in meters
	enum hott_alarm_tone tone;		//warning tone if value <= level
	uint8_t interval;				//interval in seconds repeating tone
	uint8_t repeat;					//repeating tone (0=infinite)
	uint8_t invert;					//should text be inverterd (0=no invert)
}__attribute__((packed, aligned(1)));

enum simulate_sensor
{
	simulate_sensor_gps = 1, simulate_sensor_gam = 2,
};

struct hott_config
{
	uint8_t version;
	uint8_t num_cells;								//number of cells
	enum simulate_sensor active_sensors;			//bitmask for sensors
	struct hott_voltage_alarm voltage_alarm[MODULE_HOTT_VOLTAGE_ALARMS];
	struct hott_distance_alarm distance_alarm[MODULE_HOTT_DISTANCE_ALARMS];
}__attribute__((packed, aligned(1)));

enum hott_session_state
{
	hott_session_state_idle, hott_session_state_got_request_type, hott_session_state_got_sensor_id,
};

struct hott_session
{
	const struct hardware_port_cfg* port;
	struct hott_config* config;

	enum hott_session_state state;

	enum hott_request_type request_type;
	enum hott_sensor_id sensor;
	enum hott_key key;
	U64 msg_received;

	OS_FlagID flag;
	OS_TID task_id;
	OS_STK task_stack[MODULE_HOTT_TASK_STACK];

	struct hott_session* next;
};

static struct hott_session* hott_sessions;

struct hott_voltage_alarm_exec
{
	struct hott_voltage_alarm* alarm;
	uint8_t count;
};

struct hott_distance_alarm_exec
{
	struct hott_distance_alarm* alarm;
	uint8_t count;
};

void module_hott_task(void* pData);

__inline__ uint8_t getVoltagePercent(uint16_t voltage, uint8_t cells);
__inline__ uint16_t getFraction(float value, uint8_t precision);
__inline__ double getDistance(double lon1, double lat1, double lon2, double lat2);
__inline__ double calculateAngle(double long1, double lat1, double long2, double lat2);

int hott_cmp_alarm_distance(const void * a, const void * b);
int hott_cmp_alarm_voltage(const void * a, const void * b);

static void RX_Callback(uint8_t* id);

#endif /* MODULE_HOTT_PRIV_H_ */
