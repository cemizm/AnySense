using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{
    class ProtocolMultiplexConfiguration : ProtocolConfiguration
    {
        private TelemetryValue[] telemetryValue;

        const int MAX_VALUES = 16;

        public ProtocolMultiplexConfiguration()
            : base()
        {
            Version = 1;
            telemetryValue = new TelemetryValue[MAX_VALUES];

            telemetryValue[0] = TelemetryValue.None;
            telemetryValue[1] = TelemetryValue.None;
            telemetryValue[2] = TelemetryValue.Distance;
            telemetryValue[3] = TelemetryValue.Altitude;
            telemetryValue[4] = TelemetryValue.Speed;
            telemetryValue[5] = TelemetryValue.VSpeed;
            telemetryValue[6] = TelemetryValue.Compass;
            telemetryValue[7] = TelemetryValue.COG;
            telemetryValue[8] = TelemetryValue.HomeDirection;
            telemetryValue[9] = TelemetryValue.Flightmode;
            telemetryValue[10] = TelemetryValue.Satellite;
            telemetryValue[11] = TelemetryValue.GPS_Fix;
            telemetryValue[12] = TelemetryValue.Voltage;
            telemetryValue[13] = TelemetryValue.LipoVoltage;
            telemetryValue[14] = TelemetryValue.Current;
            telemetryValue[15] = TelemetryValue.Capacity;
        }

        #region "Properties"

        [Browsable(false)]
        [EditorBrowsable(EditorBrowsableState.Never)]
        [Category("1. General")]
        [DisplayName("Config Version")]
        [Description("Version of Configuration for backward compability")]
        public byte Version { get; private set; }
        
        [Category("1. Telemetry Values")]
        [DisplayName("Address 00")]
        public TelemetryValue Address0
        {
            get { return telemetryValue[0]; }
            set { telemetryValue[0] = value; }
        }

        [Category("1. Telemetry Values")]
        [DisplayName("Address 01")]
        public TelemetryValue Address1
        {
            get { return telemetryValue[1]; }
            set { telemetryValue[1] = value; }
        }

        [Category("1. Telemetry Values")]
        [DisplayName("Address 02")]
        public TelemetryValue Address2
        {
            get { return telemetryValue[2]; }
            set { telemetryValue[2] = value; }
        }

        [Category("1. Telemetry Values")]
        [DisplayName("Address 03")]
        public TelemetryValue Address3
        {
            get { return telemetryValue[3]; }
            set { telemetryValue[3] = value; }
        }

        [Category("1. Telemetry Values")]
        [DisplayName("Address 04")]
        public TelemetryValue Address4
        {
            get { return telemetryValue[4]; }
            set { telemetryValue[4] = value; }
        }

        [Category("1. Telemetry Values")]
        [DisplayName("Address 05")]
        public TelemetryValue Address5
        {
            get { return telemetryValue[5]; }
            set { telemetryValue[5] = value; }
        }

        [Category("1. Telemetry Values")]
        [DisplayName("Address 06")]
        public TelemetryValue Address6
        {
            get { return telemetryValue[6]; }
            set { telemetryValue[6] = value; }
        }

        [Category("1. Telemetry Values")]
        [DisplayName("Address 07")]
        public TelemetryValue Address7
        {
            get { return telemetryValue[7]; }
            set { telemetryValue[7] = value; }
        }

        [Category("1. Telemetry Values")]
        [DisplayName("Address 08")]
        public TelemetryValue Address8
        {
            get { return telemetryValue[8]; }
            set { telemetryValue[8] = value; }
        }

        [Category("1. Telemetry Values")]
        [DisplayName("Address 09")]
        public TelemetryValue Address9
        {
            get { return telemetryValue[9]; }
            set { telemetryValue[9] = value; }
        }

        [Category("1. Telemetry Values")]
        [DisplayName("Address 10")]
        public TelemetryValue Address10
        {
            get { return telemetryValue[10]; }
            set { telemetryValue[10] = value; }
        }

        [Category("1. Telemetry Values")]
        [DisplayName("Address 11")]
        public TelemetryValue Address11
        {
            get { return telemetryValue[11]; }
            set { telemetryValue[11] = value; }
        }

        [Category("1. Telemetry Values")]
        [DisplayName("Address 12")]
        public TelemetryValue Address12
        {
            get { return telemetryValue[12]; }
            set { telemetryValue[12] = value; }
        }

        [Category("1. Telemetry Values")]
        [DisplayName("Address 13")]
        public TelemetryValue Address13
        {
            get { return telemetryValue[13]; }
            set { telemetryValue[13] = value; }
        }

        [Category("1. Telemetry Values")]
        [DisplayName("Address 14")]
        public TelemetryValue Address14
        {
            get { return telemetryValue[14]; }
            set { telemetryValue[14] = value; }
        }

        [Category("1. Telemetry Values")]
        [DisplayName("Address 15")]
        public TelemetryValue Address15
        {
            get { return telemetryValue[15]; }
            set { telemetryValue[15] = value; }
        }

        #endregion

        public override void DeSerialize(byte[] data)
        {
            byte pos = 0;

            Version = data[pos++];
            for (int i = 0; i < telemetryValue.Length; i++)
                telemetryValue[i] = (TelemetryValue)data[pos++];

        }

        public override byte[] Serialize()
        {
            byte[] data = new byte[128];

            byte pos = 0;

            data[pos++] = Version;

            for (int i = 0; i < telemetryValue.Length; i++)
                data[pos++] = (byte)telemetryValue[i];

            return data;
        }

        #region Nested Types

        [TypeConverter(typeof(EnumDescriptionTypeConverter))]
        public enum TelemetryValue : byte
        {
            [Description("Disabled")]
            None = 0,
            [Description("Satellites")]
            Satellite = 1,
            [Description("GPS Fix")]
            GPS_Fix = 2,
            Flightmode = 3,
            Speed = 4,
            Distance = 5,
            Altitude = 6,
            [Description("Vertical Speed (Vario)")]
            VSpeed = 7,
            Compass = 8,
            [Description("Flightdirection (COG)")]
            COG = 9,
            [Description("Home Direction")]
            HomeDirection = 10,
            Voltage = 11,
            Current = 12,
            Capacity = 13,
            [Description("Lipo Voltage (lowest)")]
            LipoVoltage = 14,
            [Description("Lipo Temperature")]
            LipoTemp = 15,
        };

        #endregion

    }

}
