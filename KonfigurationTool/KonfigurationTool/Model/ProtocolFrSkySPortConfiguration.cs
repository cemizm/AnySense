using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{


    internal class ProtocolFrSkySPortConfiguration : ProtocolConfiguration
    {

        private const UInt16 CONFIG_VERSION = 0x0001;

        public ProtocolFrSkySPortConfiguration()
        {

            Version = CONFIG_VERSION;
            MinFixType = FixType.FIX_2D;

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
            GPS_COURS = TelemetryValue.HeadingComp;
            GPS_TIME_DATE = TelemetryValue.GPS_Time;
            CELLS = TelemetryValue.Cells;
            CURR = TelemetryValue.Current;
        }

        public override void DeSerialize(byte[] data)
        {
            SensorId = (FrSkySensorId)data[0];
            Version = Converter.ToUInt16(data, 1);

            if (Version != CONFIG_VERSION)
            {
                //Version Missmatch... using default;
                Version = CONFIG_VERSION;
                return;
            }

            MinFixType = (FixType)data[3];

            int offset = 4;
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
            A3 = (TelemetryValue)data[offset++];
            A4 = (TelemetryValue)data[offset++];
            AIR_SPEED = (TelemetryValue)data[offset++];
        }

        public override byte[] Serialize()
        {
            byte[] data = new byte[128];

            data[0] = (byte)SensorId;
            Converter.GetBytes(Version, data, 1);

            data[3] = (byte)MinFixType;

            int offset = 4;
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
            data[offset++] = (byte)A3;
            data[offset++] = (byte)A4;
            data[offset++] = (byte)AIR_SPEED;

            return data;
        }

        [Category("1. General")]
        [DisplayName("Sensor Id")]
        [Description("Defines the FrSky Sensor Id to use")]
        public FrSkySensorId SensorId { get; set; }

        [Category("1. General")]
        [DisplayName("Min. Fix Type")]
        [Description("Defines minimum GPS Fix for transmission of GPS depending values")]
        public FixType MinFixType { get; set; }

        [Browsable(false)]
        [EditorBrowsable(EditorBrowsableState.Never)]
        [Category("1. General")]
        [DisplayName("Config Version")]
        [Description("Version of Configuration for backward compability")]
        public UInt16 Version { get; private set; }

        [Category("2. Additional Mapping")]
        public TelemetryValue T1 { get; set; }
        [Category("2. Additional Mapping")]
        public TelemetryValue T2 { get; set; }

        [Category("3. Expert Mapping")]
        [DisplayName("Position (Long/Lati)")]
        public TelemetryValue GPS_LONG_LATI { get; set; }

        [Category("3. Expert Mapping")]
        [DisplayName("GPS Alt")]
        public TelemetryValue GPS_ALT { get; set; }

        [Category("3. Expert Mapping")]
        [DisplayName("Speed")]
        public TelemetryValue GPS_SPEED { get; set; }

        [Category("3. Expert Mapping")]
        [DisplayName("Alt")]
        public TelemetryValue GPS_COURS { get; set; }

        [Category("3. Expert Mapping")]
        [DisplayName("Time")]
        public TelemetryValue GPS_TIME_DATE { get; set; }

        [Category("3. Expert Mapping")]
        [DisplayName("Alt")]
        public TelemetryValue ALT { get; set; }

        [Category("3. Expert Mapping")]
        [DisplayName("VSpd")]
        public TelemetryValue VARIO { get; set; }

        [Category("3. Expert Mapping")]
        [DisplayName("AccX")]
        public TelemetryValue ACCX { get; set; }

        [Category("3. Expert Mapping")]
        [DisplayName("AccY")]
        public TelemetryValue ACCY { get; set; }

        [Category("3. Expert Mapping")]
        [DisplayName("AccZ")]
        public TelemetryValue ACCZ { get; set; }

        [Category("3. Expert Mapping")]
        [DisplayName("AirSpeed")]
        public TelemetryValue AIR_SPEED { get; set; }

        [Category("3. Expert Mapping")]
        [DisplayName("Vfas")]
        public TelemetryValue VFAS { get; set; }

        [Category("3. Expert Mapping")]
        [DisplayName("Rpm")]
        public TelemetryValue RPM { get; set; }

        [Category("3. Expert Mapping")]
        [DisplayName("Fuel")]
        public TelemetryValue FUEL { get; set; }

        [Category("3. Expert Mapping")]
        [DisplayName("Curr")]
        public TelemetryValue CURR { get; set; }

        [Category("3. Expert Mapping")]
        [DisplayName("Cells")]
        public TelemetryValue CELLS { get; set; }

        [Category("3. Expert Mapping")]
        public TelemetryValue A3 { get; set; }

        [Category("3. Expert Mapping")]
        public TelemetryValue A4 { get; set; }

    }
}
