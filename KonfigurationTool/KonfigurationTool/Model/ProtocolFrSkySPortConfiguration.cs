﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{


    internal class ProtocolFrSkySPortConfiguration : ProtocolConfiguration
    {
        private MavLink.FrameworkBitConverter converter;

        private const UInt16 CONFIG_VERSION = 0x0001;

        public ProtocolFrSkySPortConfiguration()
        {
            converter = new MavLink.FrameworkBitConverter();
            converter.SetDataIsLittleEndian(MavLink.MavlinkSettings.IsLittleEndian);

            Version = CONFIG_VERSION;

            T1 = TelemetryValue.NumSat;
            T2 = TelemetryValue.GPS_Fix;
            ALT = TelemetryValue.Altitude;
            VARIO = TelemetryValue.Vario;
            ACCX = TelemetryValue.Roll;
            ACCY = TelemetryValue.Pitch;
            VFAS = TelemetryValue.Voltage;
            GPS_LONG_LATI = TelemetryValue.GPS_Position;
            GPS_ALT = TelemetryValue.GPS_Altitude;
            GPS_SPEED = TelemetryValue.Speed;
            GPS_COURS = TelemetryValue.Heading;
            GPS_TIME_DATE = TelemetryValue.GPS_Time;
        }

        public override void DeSerialize(byte[] data)
        {
            SensorId = (FrSkySensorId)data[0];
            Version = converter.ToUInt16(data, 1);

            if (Version != CONFIG_VERSION)
            {
                //Version Missmatch... using default;
                Version = CONFIG_VERSION;
                return;
            }

            int offset = 3;
            T1 = (TelemetryValue)data[offset++];
            T2 = (TelemetryValue)data[offset++];
            RPM = (TelemetryValue)data[offset++];
            FUEL = (TelemetryValue)data[offset++];
            ALT = (TelemetryValue)data[offset++];
            VARIO = (TelemetryValue)data[offset++];
            ACCX = (TelemetryValue)data[offset++];
            ACCY = (TelemetryValue)data[offset++];
            ACCZ = (TelemetryValue)data[offset++];
            CURR = (TelemetryValue)data[offset++];
            VFAS = (TelemetryValue)data[offset++];
            CELLS = (TelemetryValue)data[offset++];
            GPS_LONG_LATI = (TelemetryValue)data[offset++];
            GPS_ALT = (TelemetryValue)data[offset++];
            GPS_SPEED = (TelemetryValue)data[offset++];
            GPS_COURS = (TelemetryValue)data[offset++];
            GPS_TIME_DATE = (TelemetryValue)data[offset++];
            BETA_VARIO = (TelemetryValue)data[offset++];
            BETA_BARO_ALT = (TelemetryValue)data[offset++];
        }

        public override byte[] Serialize()
        {
            byte[] data = new byte[128];

            data[0] = (byte)SensorId;
            converter.GetBytes(Version, data, 1);

            int offset = 3;
            data[offset++] = (byte)T1;
            data[offset++] = (byte)T2;
            data[offset++] = (byte)RPM;
            data[offset++] = (byte)FUEL;
            data[offset++] = (byte)ALT;
            data[offset++] = (byte)VARIO;
            data[offset++] = (byte)ACCX;
            data[offset++] = (byte)ACCY;
            data[offset++] = (byte)ACCZ;
            data[offset++] = (byte)CURR;
            data[offset++] = (byte)VFAS;
            data[offset++] = (byte)CELLS;
            data[offset++] = (byte)GPS_LONG_LATI;
            data[offset++] = (byte)GPS_ALT;
            data[offset++] = (byte)GPS_SPEED;
            data[offset++] = (byte)GPS_COURS;
            data[offset++] = (byte)GPS_TIME_DATE;
            data[offset++] = (byte)BETA_VARIO;
            data[offset++] = (byte)BETA_BARO_ALT;

            return data;
        }

        [Category("1. General")]
        [DisplayName("Sensor Id")]
        [Description("Defines the Id to use on this Port")]
        public FrSkySensorId SensorId { get; set; }

        [Category("1. General")]
        [DisplayName("Config Version")]
        [Description("Version of Configuration for backward compability")]
        public UInt16 Version { get; private set; }

        [Category("2. GPS")]
        [DisplayName("Position (Long/Lati)")]
        public TelemetryValue GPS_LONG_LATI { get; set; }

        [Category("2. GPS")]
        [DisplayName("Altitude")]
        public TelemetryValue GPS_ALT { get; set; }

        [Category("2. GPS")]
        [DisplayName("Speed")]
        public TelemetryValue GPS_SPEED { get; set; }

        [Category("2. GPS")]
        [DisplayName("Heading")]
        public TelemetryValue GPS_COURS { get; set; }

        [Category("2. GPS")]
        [DisplayName("Datetime")]
        public TelemetryValue GPS_TIME_DATE { get; set; }


        [Category("3. Mag/Acc/Baro")]
        [DisplayName("Altitude")]
        public TelemetryValue ALT { get; set; }
        [Category("3. Mag/Acc/Baro")]
        [DisplayName("Vertical Speed")]
        public TelemetryValue VARIO { get; set; }
        [Category("3. Mag/Acc/Baro")]
        [DisplayName("Acceleration X")]
        public TelemetryValue ACCX { get; set; }
        [Category("3. Mag/Acc/Baro")]
        [DisplayName("Acceleration Y")]
        public TelemetryValue ACCY { get; set; }
        [Category("3. Mag/Acc/Baro")]
        [DisplayName("Acceleration Z")]
        public TelemetryValue ACCZ { get; set; }
        [Category("3. Mag/Acc/Baro")]
        public TelemetryValue BETA_VARIO { get; set; }
        [Category("3. Mag/Acc/Baro")]
        public TelemetryValue BETA_BARO_ALT { get; set; }

        [Category("4. Others")]
        public TelemetryValue T1 { get; set; }
        [Category("4. Others")]
        public TelemetryValue T2 { get; set; }
        [Category("4. Others")]
        public TelemetryValue VFAS { get; set; }
        [Category("4. Others")]
        public TelemetryValue RPM { get; set; }
        [Category("4. Others")]
        public TelemetryValue FUEL { get; set; }
        [Category("4. Others")]
        public TelemetryValue CURR { get; set; }
        [Category("4. Others")]
        public TelemetryValue CELLS { get; set; }

    }
}