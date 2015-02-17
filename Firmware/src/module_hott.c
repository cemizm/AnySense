/*
 * module_hott.c
 *
 *  Created on: 12.09.2014
 *      Author: cem
 */

#include "module_hott_priv.h"

void hott_initializeConfig(void* config)
{
	struct hott_config* cfg = (struct hott_config*) config;

	if (cfg->version != 1)
	{
		cfg->version = 1;
		cfg->num_cells = 3;
		cfg->active_sensors = simulate_sensor_gam | simulate_sensor_gps | simulate_sensor_vario | simulate_sensor_eam;
		cfg->gps_altitude_source = value_source_alt_sensor;
		cfg->gps_flightdirection_source = value_source_gps_sensor;

		cfg->voltage_alarm[0].level = 20;
		cfg->voltage_alarm[0].interval = 8;
		cfg->voltage_alarm[0].repeat = 0;
		cfg->voltage_alarm[0].invert = 1;
		cfg->voltage_alarm[0].tone = hott_alarm_tone_volt_min;

		cfg->voltage_alarm[1].level = 10;
		cfg->voltage_alarm[1].interval = 5;
		cfg->voltage_alarm[1].repeat = 0;
		cfg->voltage_alarm[1].invert = 1;
		cfg->voltage_alarm[1].tone = hott_alarm_tone_s1_volt_min;

		cfg->voltage_alarm[2].level = 5;
		cfg->voltage_alarm[2].interval = 2;
		cfg->voltage_alarm[2].repeat = 0;
		cfg->voltage_alarm[2].invert = 1;
		cfg->voltage_alarm[2].tone = hott_alarm_tone_s2_volt_min;

		cfg->distance_alarm[0].level = 3000;
		cfg->distance_alarm[0].interval = 10;
		cfg->distance_alarm[0].repeat = 0;
		cfg->distance_alarm[0].invert = 1;
		cfg->distance_alarm[0].tone = hott_alarm_tone_unk_04;

		cfg->distance_alarm[1].level = 0;
		cfg->distance_alarm[1].interval = 0;
		cfg->distance_alarm[1].repeat = 0;
		cfg->distance_alarm[1].invert = 0;
		cfg->distance_alarm[1].tone = hott_alarm_tone_none;

		cfg->distance_alarm[2].level = 0;
		cfg->distance_alarm[2].interval = 0;
		cfg->distance_alarm[2].repeat = 0;
		cfg->distance_alarm[2].invert = 0;
		cfg->distance_alarm[2].tone = hott_alarm_tone_none;
	}
}

void hott_start(const struct hardware_port_cfg* port, uint8_t* config)
{
	USART_InitTypeDef USART_InitStructure;

	port->rx.clock.cmd(port->rx.clock.periph, ENABLE);
	port->tx.clock.cmd(port->tx.clock.periph, ENABLE);

	port->clock.cmd(port->clock.periph, ENABLE);

	/* GPIO Initialization */
	GPIO_InitTypeDef initStr = port->rx.init;
	initStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(port->rx.gpio, &initStr);

	initStr = port->tx.init;
	initStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(port->tx.gpio, &initStr);

	/* Alternate function configuration */
	GPIO_PinAFConfig(port->rx.gpio, port->rx.pin_source, port->af);
	GPIO_PinAFConfig(port->tx.gpio, port->tx.pin_source, port->af);

	/* USART Initialization */
	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	struct hott_session* session = (struct hott_session*) malloc(sizeof(struct hott_session));
	session->next = NULL;
	LL_APPEND(hott_sessions, session);

	session->port = port;
	session->config = (struct hott_config*) config;

	USART_Init(port->port, &USART_InitStructure);
	USART_HalfDuplexCmd(port->port, ENABLE);
	USART_OverrunDetectionConfig(port->port, USART_OVRDetection_Disable);
	USART_Cmd(port->port, ENABLE);

	NVIC_InitTypeDef def;
	def.NVIC_IRQChannelCmd = ENABLE;
	def.NVIC_IRQChannel = port->nvic_ch;
	def.NVIC_IRQChannelPriority = PRIORITY_TELEMETRY;
	NVIC_Init(&def);

	session->flag = CoCreateFlag(Co_TRUE, Co_FALSE);

	session->task_id = CoCreateTask(module_hott_task, session, 0, &session->task_stack[MODULE_HOTT_TASK_STACK - 1],
			MODULE_HOTT_TASK_STACK);

	hardware_register_callback(session->port, &RX_Callback, NULL, (uint8_t*) session);

	USART_ITConfig(session->port->port, USART_IT_RXNE, ENABLE);
}

void module_hott_task(void* pData)
{
	struct hott_session* session = (struct hott_session*) pData;

	U64 delay = 0;
	U64 ticks = 0;
	uint8_t pos = 0;

	uint8_t ledEnabled = session->port == &usart_port1;

	union hott_msg msg;	//Union of messages
	uint8_t size; 		//Size of current packet
	uint8_t crc;		//crc calculated during transmission

	U64 alarm_nextTone = 0;
	U64 distance_nextTone = 0;

	struct hott_voltage_alarm_exec alarms[MODULE_HOTT_VOLTAGE_ALARMS];
	for (uint8_t i = 0; i < MODULE_HOTT_VOLTAGE_ALARMS; i++)
	{
		alarms[i].alarm = session->config->voltage_alarm + i;
		alarms[i].count = 0;
	}
	qsort(alarms, MODULE_HOTT_VOLTAGE_ALARMS, sizeof(struct hott_voltage_alarm_exec), &hott_cmp_alarm_voltage);

	struct hott_distance_alarm_exec distance_alarms[MODULE_HOTT_DISTANCE_ALARMS];
	for (uint8_t i = 0; i < MODULE_HOTT_DISTANCE_ALARMS; i++)
	{
		distance_alarms[i].alarm = session->config->distance_alarm + i;
		distance_alarms[i].count = 0;
	}
	qsort(distance_alarms, MODULE_HOTT_DISTANCE_ALARMS, sizeof(struct hott_distance_alarm_exec), &hott_cmp_alarm_distance);

	uint8_t exec_alarm = 0;
	double distance = 0;
	uint8_t voltagePercent = 0;
	uint16_t lowest_cell = 0;
	double tmp_conv = 0;
	int8_t page = 0;
	uint8_t line = 0;
	uint8_t active = 0;

	float minHeight = 500;
	float maxHeight = 500;

	char fixTextNo[] = "No";
	char fixText2D[] = "2D";
	char fixText3D[] = "3D";
	char fixTextDGPS[] = "DGPS";
	char* fixText = NULL;

	char modeTextMan[] = "Manual";
	char modeTextAtti[] = "Atti";
	char modeTextGPS[] = "GPS";
	char modeTextFS[] = "Failsafe";
	char* modeText = NULL;

	char fixChars[] = { '-', '-', '2', '3', 'D' };

	while (1)
	{
		CoWaitForSingleFlag(session->flag, 0);

		size = 0;
		crc = 0;

		memset(msg.buffer, 0, MODULE_HOTT_MSG_BUFFER_SIZE);
		ticks = CoGetOSTime();

		lowest_cell = simpleTelemetry_getLowestCell(session->config->num_cells);
		voltagePercent = simpleTelemetry_getPercentage(lowest_cell);

		if (simpleTelemtryData.homeLon != 0 && simpleTelemtryData.homeLat != 0)
		{
			distance = getDistance(simpleTelemtryData.lon, simpleTelemtryData.lat, simpleTelemtryData.homeLon,
					simpleTelemtryData.homeLat);
		}
		else
			distance = 0;

		if (simpleTelemtryData.mode == flightMode_manual)
			modeText = modeTextMan;
		else if (simpleTelemtryData.mode == flightMode_atti)
			modeText = modeTextAtti;
		else if (simpleTelemtryData.mode == flightMode_gps)
			modeText = modeTextGPS;
		else
			modeText = modeTextFS;

		switch (session->request_type)
		{
		case hott_request_type_binary:
			if (session->sensor == hott_sensor_id_gam && (session->config->active_sensors & simulate_sensor_gam))
			{
				HOTT_INIT_MSG(msg.gam, hott_sensor_id_gam);

				msg.gam.altitude = 500 + simpleTelemtryData.alt - (simpleTelemtryData.homeAltBaro - 20);
				msg.gam.battery1 = simpleTelemtryData.battery / 100;
				msg.gam.main_voltage = simpleTelemtryData.battery / 100;
				msg.gam.climbrate_L = 30000 + (simpleTelemtryData.vsi * 100);
				msg.gam.climbrate3s = 120;
				msg.gam.speed = simpleTelemtryData.speed * 3.6;
				msg.gam.current = simpleTelemtryData.current * 10;
				msg.gam.temp1 = 20 + simpleTelemtryData.temp1;
				msg.gam.temp2 = 20 + simpleTelemtryData.temp2;

				if (simpleTelemtryData.cellCount > 0)
				{
					for (uint8_t i = 0; i < simpleTelemtryData.cellCount && i < MODULE_HOTT_GAM_CELLS; i++)
						msg.gam.cell[i] = simpleTelemtryData.cells[i] / 20;
				}

				msg.gam.min_cell_volt = lowest_cell / 20;
				msg.gam.fuel_percent = voltagePercent;
				msg.gam.batt_cap = simpleTelemtryData.capacity_current / 10;

				size = sizeof(struct hott_msg_gam);
			} //session->sensor == hott_sensor_id_gam
			else if (session->sensor == hott_sensor_id_vario && (session->config->active_sensors & simulate_sensor_vario))
			{
				HOTT_INIT_MSG(msg.vario, hott_sensor_id_vario);

				msg.vario.Vclimbm10s = 30000;
				msg.vario.Vclimbm3s = 30000;

				if (session->config->gps_altitude_source == value_source_alt_sensor)
				{
					msg.vario.height = 500 + simpleTelemtryData.alt - (simpleTelemtryData.homeAltBaro - 20);
					msg.vario.climbm2s = 120 + (simpleTelemtryData.vsi * 10);
				}
				else
				{
					msg.vario.height = 500 + simpleTelemtryData.gpsAlt;
					msg.vario.climbm2s = 120 + (simpleTelemtryData.gpsVsi * 10);
				}

				if (simpleTelemtryData.lon != 0 && simpleTelemtryData.lat != 0 && simpleTelemtryData.homeLon != 0
						&& simpleTelemtryData.homeLat != 0)
				{

					if (msg.vario.height > maxHeight)
						maxHeight = msg.vario.height;

					if (msg.vario.height < minHeight)
						minHeight = msg.vario.height;
				}

				msg.vario.height_max = maxHeight;
				msg.vario.height_min = minHeight;

				snprintf((char *) &msg.vario.text_msg, MODULE_HOTT_TEXT_COLUMNS + 1, "Flightmode: %s", modeText);

				if (session->config->gps_flightdirection_source == value_source_alt_sensor)
					msg.vario.flight_direction = simpleTelemtryData.heading / 2;
				else
					msg.vario.flight_direction = simpleTelemtryData.cog / 2;

				size = sizeof(struct hott_msg_vario);
			} //session->sensor == hott_sensor_id_vario
			else if (session->sensor == hott_sensor_id_eam && (session->config->active_sensors & simulate_sensor_eam))
			{
				HOTT_INIT_MSG(msg.eam, hott_sensor_id_eam);

				if (session->config->gps_altitude_source == value_source_alt_sensor)
				{
					msg.eam.height = 500 + simpleTelemtryData.alt - (simpleTelemtryData.homeAltBaro - 20);
					msg.eam.climbm2s = 30000 + (simpleTelemtryData.vsi * 100);
				}
				else
				{
					msg.eam.height = 500 + simpleTelemtryData.gpsAlt;
					msg.eam.climbm2s = 30000 + (simpleTelemtryData.gpsVsi * 100);
				}

				msg.eam.climbm3s = 120;

				msg.eam.speed = simpleTelemtryData.speed * 3.6;
				msg.eam.driveVoltage = simpleTelemtryData.battery / 100;

				msg.eam.current = simpleTelemtryData.current * 10;
				msg.eam.temp1 = 20 + simpleTelemtryData.temp1;
				msg.eam.temp2 = 20 + simpleTelemtryData.temp2;
				msg.eam.capacity = simpleTelemtryData.capacity_current;

				for (uint8_t i = 0; i < simpleTelemtryData.cellCount && i < MODULE_HOTT_EAM_CELLS; i++)
					msg.eam.cells[i] = simpleTelemtryData.cells[i] / 20;

				size = sizeof(struct hott_msg_eam);
			} //session->sensor == hott_sensor_id_eam
			else if (session->sensor == hott_sensor_id_gps && (session->config->active_sensors & simulate_sensor_gps))
			{
				HOTT_INIT_MSG(msg.gps, hott_sensor_id_gps);
				msg.gps.GPSNumSat = simpleTelemtryData.numSat;

				msg.gps.GPS_fix = fixChars[simpleTelemtryData.fixType];

				if (session->config->gps_altitude_source == value_source_alt_sensor)
				{
					msg.gps.altitude = 500 + simpleTelemtryData.alt - (simpleTelemtryData.homeAltBaro - 20);
					msg.gps.climbrate1s = 30000 + (simpleTelemtryData.vsi * 100);
				}
				else
				{
					msg.gps.altitude = 500 + simpleTelemtryData.gpsAlt;
					msg.gps.climbrate1s = 30000 + (simpleTelemtryData.gpsVsi * 100);
				}

				msg.gps.climbrate3s = 120;

				msg.gps.GPSSpeed = simpleTelemtryData.speed * 3.6;

				if (session->config->gps_flightdirection_source == value_source_alt_sensor)
					msg.gps.flightDirection = simpleTelemtryData.heading / 2;
				else
					msg.gps.flightDirection = simpleTelemtryData.cog / 2;

				msg.gps.angleXdirection = simpleTelemtryData.pitch;
				msg.gps.angleYdirection = simpleTelemtryData.roll;
				msg.gps.angleZdirection = simpleTelemtryData.heading;

				tmp_conv = simpleTelemtryData.lon;
				if (tmp_conv < 0)
				{
					msg.gps.lonEW = 1;
					tmp_conv *= -1;
				}

				msg.gps.lonMin = (((uint16_t) tmp_conv) * 100);
				tmp_conv = (tmp_conv - ((uint16_t) tmp_conv)) * 60;
				msg.gps.lonMin += (uint16_t) tmp_conv;
				tmp_conv = (tmp_conv - ((uint16_t) tmp_conv)) * 10000;
				msg.gps.lonSec = (uint16_t) tmp_conv;

				tmp_conv = simpleTelemtryData.lat;
				if (tmp_conv < 0)
				{
					msg.gps.latNS = 1;
					tmp_conv *= -1;
				}

				msg.gps.latMin = (((uint16_t) tmp_conv) * 100);
				tmp_conv = (tmp_conv - ((uint16_t) tmp_conv)) * 60;
				msg.gps.latMin += (uint16_t) tmp_conv;
				tmp_conv = (tmp_conv - ((uint16_t) tmp_conv)) * 10000;
				msg.gps.latSec = (uint16_t) tmp_conv;

				msg.gps.distance = (uint16_t) distance;

				if (simpleTelemtryData.lon != 0 && simpleTelemtryData.lat != 0 && simpleTelemtryData.homeLon != 0
						&& simpleTelemtryData.homeLat != 0)
				{
					msg.gps.HomeDirection = calculateAngle(simpleTelemtryData.lon, simpleTelemtryData.lat,
							simpleTelemtryData.homeLon, simpleTelemtryData.homeLat) / 2;
				}
				else
				{
					msg.gps.HomeDirection = 0;
					msg.gps.alarm = hott_msg_alarm_main_curr;
				}

				msg.gps.gps_time_h = simpleTelemtryData.hour;
				msg.gps.gps_time_m = simpleTelemtryData.minute;
				msg.gps.gps_time_s = simpleTelemtryData.second;

				size = sizeof(struct hott_msg_gps);
			} //session->sensor == hott_sensor_id_gps
			break; //hott_request_type_binary
		case hott_request_type_text:
		{
			HOTT_INIT_TEXT(msg.text);

			if (session->key == hott_key_left)
			{
				if (page > 0)
					page--;
				else
					msg.text.esc = 0x01;
			}
			else if (session->key == hott_key_right)
				page++;

			if (session->sensor == hott_sensor_id_gam && (session->config->active_sensors & simulate_sensor_gam))
			{
				if (page > 1)
					page = 1;

				switch (page)
				{
				case 0:
					//															        012345678901234567890
					snprintf((char *) &msg.text.text[0], MODULE_HOTT_TEXT_COLUMNS + 1, " AnySense GAM      <>");
					snprintf((char *) &msg.text.text[1], MODULE_HOTT_TEXT_COLUMNS + 1, " Cells     :%d",
							session->config->num_cells);
					snprintf((char *) &msg.text.text[2], MODULE_HOTT_TEXT_COLUMNS + 1, " Battery   :%d.%02dV",
							(int16_t) simpleTelemtryData.battery / 1000, getFraction(simpleTelemtryData.battery / 1000.0, 2));
					snprintf((char *) &msg.text.text[3], MODULE_HOTT_TEXT_COLUMNS + 1, " Calc. Cell:%2d.%02dV",
							(int16_t) (simpleTelemtryData.battery / session->config->num_cells) / 1000,
							getFraction((simpleTelemtryData.battery / session->config->num_cells) / 1000.0, 2));
					snprintf((char *) &msg.text.text[5], MODULE_HOTT_TEXT_COLUMNS + 1, " Altitude  :%d.%02dm",
							(int16_t) simpleTelemtryData.alt, getFraction(simpleTelemtryData.alt, 2));
					snprintf((char *) &msg.text.text[6], MODULE_HOTT_TEXT_COLUMNS + 1, " Home Alt. :%d.%02dm",
							(int16_t) simpleTelemtryData.homeAltBaro - 20, getFraction(simpleTelemtryData.homeAltBaro - 20, 2));
					snprintf((char *) &msg.text.text[7], MODULE_HOTT_TEXT_COLUMNS + 1, " VSpeed    :%d.%02dm/s",
							(int16_t) simpleTelemtryData.vsi, getFraction(simpleTelemtryData.vsi, 2));
					break;
				case 1:
					if (!active)
					{
						if (session->key == hott_key_up && line > 0)
							line--;
						else if (session->key == hott_key_down && line < 25)
							line++;
					}

					if (session->key == hott_key_set)
						active = !active;

					//															    012345678901234567890
					snprintf((char *) &msg.text.text[0], MODULE_HOTT_TEXT_COLUMNS, " AnySense Tones    < ");
					snprintf((char *) &msg.text.text[1], MODULE_HOTT_TEXT_COLUMNS + 1, "    A    H    O    V");
					snprintf((char *) &msg.text.text[2], MODULE_HOTT_TEXT_COLUMNS + 1, "    B    I    P    W");
					snprintf((char *) &msg.text.text[3], MODULE_HOTT_TEXT_COLUMNS + 1, "    C    J    Q    X");
					snprintf((char *) &msg.text.text[4], MODULE_HOTT_TEXT_COLUMNS + 1, "    D    K    R    Y");
					snprintf((char *) &msg.text.text[5], MODULE_HOTT_TEXT_COLUMNS + 1, "    E    L    S    Z");
					snprintf((char *) &msg.text.text[6], MODULE_HOTT_TEXT_COLUMNS + 1, "    F    M    T     ");
					snprintf((char *) &msg.text.text[7], MODULE_HOTT_TEXT_COLUMNS + 1, "    G    N    U     ");

					msg.text.text[line % 7 + 1][(line / 7) * 5 + 3] = '>';

					if (active)
					{
						//0x80
						msg.text.warning_beeps = line + 1;
						msg.text.text[line % 7 + 1][(line / 7) * 5 + 3] |= 0x80;
						msg.text.text[line % 7 + 1][(line / 7) * 5 + 4] |= 0x80;
					}
					break;
				}

			} //session->sensor == hott_sensor_id_gam
			else if (session->sensor == hott_sensor_id_gps && (session->config->active_sensors & simulate_sensor_gps))
			{
				if (page > 3)
					page = 3;

				switch (page)
				{
				case 0:
				{
					if (simpleTelemtryData.fixType == fixType_No)
						fixText = fixTextNo;
					else if (simpleTelemtryData.fixType == fixType_2D)
						fixText = fixText2D;
					else if (simpleTelemtryData.fixType == fixType_3D)
						fixText = fixText3D;
					else if (simpleTelemtryData.fixType == fixType_DGPS)
						fixText = fixTextDGPS;

					//															        012345678901234567890
					snprintf((char *) &msg.text.text[0], MODULE_HOTT_TEXT_COLUMNS + 1, " AnySense GPS      <>");
					snprintf((char *) &msg.text.text[1], MODULE_HOTT_TEXT_COLUMNS + 1, " Num Sats :%d",
							simpleTelemtryData.numSat);
					snprintf((char *) &msg.text.text[2], MODULE_HOTT_TEXT_COLUMNS + 1, " FixType  :%s", fixText);
					snprintf((char *) &msg.text.text[3], MODULE_HOTT_TEXT_COLUMNS + 1, " Altitude :%d.%02dm",
							(int16_t) simpleTelemtryData.gpsAlt, getFraction(simpleTelemtryData.gpsAlt, 2));
					snprintf((char *) &msg.text.text[4], MODULE_HOTT_TEXT_COLUMNS + 1, " COG      :%d.%02d",
							(int16_t) simpleTelemtryData.cog, getFraction(simpleTelemtryData.cog, 2));
					snprintf((char *) &msg.text.text[5], MODULE_HOTT_TEXT_COLUMNS + 1, " Heading  :%d.%02d",
							(int16_t) simpleTelemtryData.heading, getFraction(simpleTelemtryData.heading, 2));
					snprintf((char *) &msg.text.text[6], MODULE_HOTT_TEXT_COLUMNS + 1, " Speed    :%d.%02dm/s",
							(int16_t) simpleTelemtryData.speed, getFraction(simpleTelemtryData.speed, 2));
					snprintf((char *) &msg.text.text[7], MODULE_HOTT_TEXT_COLUMNS + 1, " VSpeed   :%d.%02dm/s",
							(int16_t) simpleTelemtryData.gpsVsi, getFraction(simpleTelemtryData.gpsVsi, 2));
				}
					break;
				case 1:

					//															        012345678901234567890
					snprintf((char *) &msg.text.text[0], MODULE_HOTT_TEXT_COLUMNS + 1, " AnySense GPS      <>");
					snprintf((char *) &msg.text.text[1], MODULE_HOTT_TEXT_COLUMNS + 1, " Position:");
					snprintf((char *) &msg.text.text[2], MODULE_HOTT_TEXT_COLUMNS + 1, "  %3d.%06d",
							(uint16_t) simpleTelemtryData.lat, getFraction(simpleTelemtryData.lat, 6));
					snprintf((char *) &msg.text.text[3], MODULE_HOTT_TEXT_COLUMNS + 1, "  %3d.%06d",
							(uint16_t) simpleTelemtryData.lon, getFraction(simpleTelemtryData.lon, 6));
					snprintf((char *) &msg.text.text[4], MODULE_HOTT_TEXT_COLUMNS + 1, " Home:");
					snprintf((char *) &msg.text.text[5], MODULE_HOTT_TEXT_COLUMNS + 1, "  %3d.%06d",
							(uint16_t) simpleTelemtryData.homeLat, getFraction(simpleTelemtryData.homeLat, 6));
					snprintf((char *) &msg.text.text[6], MODULE_HOTT_TEXT_COLUMNS + 1, "  %3d.%06d",
							(uint16_t) simpleTelemtryData.homeLon, getFraction(simpleTelemtryData.homeLon, 6));
					snprintf((char *) &msg.text.text[7], MODULE_HOTT_TEXT_COLUMNS + 1, " Distance   :%d.%02dm",
							(uint16_t) distance, getFraction(distance, 2));
					break;
				case 2:

					//															        012345678901234567890
					snprintf((char *) &msg.text.text[0], MODULE_HOTT_TEXT_COLUMNS + 1, " AnySense GPS      <>");
					snprintf((char *) &msg.text.text[1], MODULE_HOTT_TEXT_COLUMNS + 1, " HDOP    :%3d.%02d",
							(uint16_t) simpleTelemtryData.hdop, getFraction(simpleTelemtryData.hdop, 2));
					snprintf((char *) &msg.text.text[2], MODULE_HOTT_TEXT_COLUMNS + 1, " VDOP    :%3d.%02d",
							(uint16_t) simpleTelemtryData.vdop, getFraction(simpleTelemtryData.vdop, 2));
					snprintf((char *) &msg.text.text[3], MODULE_HOTT_TEXT_COLUMNS + 1, " Roll    :%3d.%02d",
							(int16_t) (simpleTelemtryData.roll * 180.0 / M_PI),
							getFraction((simpleTelemtryData.roll * 180.0 / M_PI), 2));
					snprintf((char *) &msg.text.text[4], MODULE_HOTT_TEXT_COLUMNS + 1, " Pitch   :%3d.%02d",
							(int16_t) (simpleTelemtryData.pitch * 180.0 / M_PI),
							getFraction((simpleTelemtryData.pitch * 180.0 / M_PI), 2));
					snprintf((char *) &msg.text.text[5], MODULE_HOTT_TEXT_COLUMNS + 1, " Mode    :%s", modeText);
					break;
				case 3:
				{

					if (!active)
					{
						if (session->key == hott_key_up && line > 0)
							line--;
						else if (session->key == hott_key_down && line < 25)
							line++;
					}

					if (session->key == hott_key_set)
						active = !active;

					//															    012345678901234567890
					snprintf((char *) &msg.text.text[0], MODULE_HOTT_TEXT_COLUMNS, " AnySense Tones    < ");
					snprintf((char *) &msg.text.text[1], MODULE_HOTT_TEXT_COLUMNS + 1, "    A    H    O    V");
					snprintf((char *) &msg.text.text[2], MODULE_HOTT_TEXT_COLUMNS + 1, "    B    I    P    W");
					snprintf((char *) &msg.text.text[3], MODULE_HOTT_TEXT_COLUMNS + 1, "    C    J    Q    X");
					snprintf((char *) &msg.text.text[4], MODULE_HOTT_TEXT_COLUMNS + 1, "    D    K    R    Y");
					snprintf((char *) &msg.text.text[5], MODULE_HOTT_TEXT_COLUMNS + 1, "    E    L    S    Z");
					snprintf((char *) &msg.text.text[6], MODULE_HOTT_TEXT_COLUMNS + 1, "    F    M    T     ");
					snprintf((char *) &msg.text.text[7], MODULE_HOTT_TEXT_COLUMNS + 1, "    G    N    U     ");

					msg.text.text[line % 7 + 1][(line / 7) * 5 + 3] = '>';

					if (active)
					{
						//0x80
						msg.text.warning_beeps = line + 1;
						msg.text.text[line % 7 + 1][(line / 7) * 5 + 3] |= 0x80;
						msg.text.text[line % 7 + 1][(line / 7) * 5 + 4] |= 0x80;
					}
				}
					break;

				} //session->sensor == hott_sensor_id_gps
			}

			for (uint8_t row = 0; row < MODULE_HOTT_TEXT_ROWS; row++)
			{
				for (uint8_t col = 0; col < MODULE_HOTT_TEXT_COLUMNS; col++)
				{
					if (msg.text.text[row][col] == 0)
						msg.text.text[row][col] = 0x20;
				}
			}

			size = sizeof(struct hott_msg_text);
		}
			break; //hott_request_type_text
		}

		if (size > 0)
		{

			//Check alarms
			if ((session->request_type == hott_request_type_text || session->sensor == hott_sensor_id_gps)
					&& (session->config->active_sensors & simulate_sensor_gps))
			{
				if (distance > 0)
				{
					for (exec_alarm = 0; exec_alarm < MODULE_HOTT_DISTANCE_ALARMS; exec_alarm++)
					{
						if (distance_alarms[exec_alarm].alarm->level > 0 && distance >= distance_alarms[exec_alarm].alarm->level)
						{
							if (distance_nextTone < ticks
									&& (distance_alarms[exec_alarm].alarm->repeat == 0
											|| distance_alarms[exec_alarm].count < distance_alarms[exec_alarm].alarm->repeat))
							{
								msg.gps.warning_beeps = distance_alarms[exec_alarm].alarm->tone;

								distance_nextTone = ticks + delay_sec(distance_alarms[exec_alarm].alarm->interval);

								if (distance_alarms[exec_alarm].alarm->repeat > 0)
									distance_alarms[exec_alarm].count++;
							}

							if (session->request_type == hott_request_type_binary
									&& distance_alarms[exec_alarm].alarm->invert > 0)
								msg.gps.alarm = hott_msg_alarm_cells;

							break;
						}
					}
				}
			}

			if ((session->request_type == hott_request_type_text || session->sensor == hott_sensor_id_gam)
					&& (session->config->active_sensors & simulate_sensor_gam))
			{
				for (exec_alarm = 0; exec_alarm < MODULE_HOTT_VOLTAGE_ALARMS; exec_alarm++)
				{
					if (alarms[exec_alarm].alarm->level > 0 && voltagePercent <= alarms[exec_alarm].alarm->level)
					{
						if (alarm_nextTone < ticks
								&& (alarms[exec_alarm].alarm->repeat == 0
										|| alarms[exec_alarm].count < alarms[exec_alarm].alarm->repeat))
						{
							msg.gam.warning_beeps = alarms[exec_alarm].alarm->tone;

							alarm_nextTone = ticks + delay_sec(alarms[exec_alarm].alarm->interval);

							if (alarms[exec_alarm].alarm->repeat > 0)
								alarms[exec_alarm].count++;
						}

						if (session->request_type == hott_request_type_binary && alarms[exec_alarm].alarm->invert > 0)
							msg.gam.alarm = hott_msg_alarm_fuel;

						break;
					}
				}
			}

			//Send data

			delay = session->msg_received + MODULE_HOTT_TX_INIT_DELAY;
			ticks = CoGetOSTime();
			if (delay > ticks)
				CoTickDelay(delay - ticks);

			if (ledEnabled)
				hardware_led_toggle_red();

			crc = 0;

			for (pos = 0; pos < size; pos++)
			{
				delay = CoGetOSTime();
				crc = crc + msg.buffer[pos];
				USART_SendData(session->port->port, msg.buffer[pos]);
				ticks = CoGetOSTime();
				CoTickDelay(ticks - delay + MODULE_HOTT_TX_DELAY);
			}

			USART_SendData(session->port->port, crc);
			CoTickDelay(MODULE_HOTT_TX_DELAY);
		}

	}

}

static void RX_Callback(uint8_t* id)
{
	struct hott_session* session = (struct hott_session*) id;

	uint8_t byte = USART_ReceiveData(session->port->port);

	switch (session->state)
	{
	case hott_session_state_idle:
	{
		if (byte == hott_request_type_binary || byte == hott_request_type_text)
		{
			session->request_type = byte;
			session->state = hott_session_state_got_request_type;
		}
	} //case hott_session_state_idle
		break;
	case hott_session_state_got_request_type:
	{
		uint8_t tmpSensor = byte;

		if (session->request_type == hott_request_type_text)
			tmpSensor = (0x80 | (byte >> 4));

		if ((tmpSensor == hott_sensor_id_gam && (session->config->active_sensors & simulate_sensor_gam))
				|| (tmpSensor == hott_sensor_id_gps && (session->config->active_sensors & simulate_sensor_gps))
				|| (tmpSensor == hott_sensor_id_vario && (session->config->active_sensors & simulate_sensor_vario))
				|| (tmpSensor == hott_sensor_id_eam && (session->config->active_sensors & simulate_sensor_eam)))
		{
			session->sensor = tmpSensor;
			session->key = session->request_type == hott_request_type_text ? (byte & 0x0F) : 0;
			session->msg_received = CoGetOSTime();
			isr_SetFlag(session->flag);
		}

		session->state = hott_session_state_idle;
	}
		break;
	default:
		//something went wrong, reset
		session->state = hott_session_state_idle;
		break;
	}
}

__inline__ uint16_t getFraction(float value, uint8_t precision)
{
	if (value < 0)
		value *= -1;

	return (uint16_t) ((value - (int16_t) value) * pow(10, precision));
}

__inline__ double getDistance(double long1, double lat1, double long2, double lat2)
{

	double delta = (long1 - long2) * (M_PI / 180);
	double sdlong = sin(delta);
	double cdlong = cos(delta);
	lat1 = lat1 * (M_PI / 180);
	lat2 = lat2 * (M_PI / 180);
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

__inline__ double calculateAngle(double long1, double lat1, double long2, double lat2)
{
	double off_y = long2 - long1;
	double off_x = cos(M_PI / 180 * long1) * (lat2 - lat1);
	double bearing = atan2(off_y, off_x) / M_PI * 180;

	if (bearing < 0)
		bearing += 360;

	return bearing;
}

int hott_cmp_alarm_distance(const void * a, const void * b)
{
	struct hott_distance_alarm_exec* ae1 = (struct hott_distance_alarm_exec*) a;
	struct hott_distance_alarm_exec* ae2 = (struct hott_distance_alarm_exec*) b;

	if (ae1->alarm->level == ae2->alarm->level)
		return 0;

	if (ae1->alarm->level == 0)
		return 1;

	if (ae2->alarm->level == 0)
		return -1;

	return ae2->alarm->level - ae1->alarm->level;

}

int hott_cmp_alarm_voltage(const void * a, const void * b)
{
	struct hott_voltage_alarm_exec* av1 = (struct hott_voltage_alarm_exec*) a;
	struct hott_voltage_alarm_exec* av2 = (struct hott_voltage_alarm_exec*) b;

	if (av1->alarm->level == av2->alarm->level)
		return 0;

	if (av1->alarm->level == 0)
		return 1;

	if (av2->alarm->level == 0)
		return -1;

	return av1->alarm->level - av2->alarm->level;
}
