using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{
    class ProtocolSpektrumConfiguration : ProtocolConfiguration
    {
        private byte cells;
        private byte alarmVoltage2;
        private UInt16 alarmCapacity1;
        private bool sensorCurrent;
        private bool sensorGPS;
        private bool sensorPowerbox;

        public ProtocolSpektrumConfiguration()
            : base()
        {
            Version = 1;

            Cells = 3;
            alarmVoltage2 = 15;
            alarmCapacity1 = 3500;
            sensorCurrent = true;
            sensorGPS = true;
            sensorPowerbox = true;
        }

        [Browsable(false)]
        [EditorBrowsable(EditorBrowsableState.Never)]
        [Category("1. General")]
        [DisplayName("Config Version")]
        [Description("Version of Configuration for backward compability")]
        public byte Version { get; private set; }

        [Category("1. General")]
        [DisplayName("Number of cells")]
        [Description("Defines the number of cells used. For Phantom 2 or FrSky Lipo Sensors, this value will be ignored.")]
        public byte Cells
        {
            get { return cells; }
            set
            {
                if (cells != value)
                {
                    if (value == 0)
                        throw new InvalidOperationException("Num. cells cannot be 0.");
                    if (value > 6)
                        throw new InvalidOperationException("Num. cells cannot exceed 6.");

                    cells = value;
                }
            }
        }


        [Category("2. Alarms")]
        [DisplayName("Low Voltage 2 (in %)")]
        [Description("PowerBox Voltage 2 Alarm in %.")]
        public byte AlarmVoltage2
        {
            get { return alarmVoltage2; }
            set
            {
                if (alarmVoltage2 != value)
                {
                    if (value > 100)
                        throw new InvalidOperationException("Value cannot exceed 100%");

                    alarmVoltage2 = value;
                }
            }
        }


        [Category("2. Alarms")]
        [DisplayName("Capacity 1 (in mAh)")]
        [Description("PowerBox Capacity 1 Alarm in mAh.")]
        public UInt16 AlarmCapacity1
        {
            get { return alarmCapacity1; }
            set
            {
                if (alarmCapacity1 != value)
                {
                    alarmCapacity1 = value;
                }
            }
        }

        [Category("3. Active Sensor")]
        [DisplayName("Current Sensor")]
        [Description("Emulates Spektrum Current Sensor.")]
        [TypeConverter(typeof(YesNoConverter))]
        public bool SensorCurrent
        {
            get { return sensorCurrent; }
            set { sensorCurrent = value; }
        }
        [Category("3. Active Sensor")]
        [DisplayName("PowerBox Sensor")]
        [Description("Emulates Spektrum PowerBox Sensor.")]
        [TypeConverter(typeof(YesNoConverter))]
        public bool SensorPowerBox
        {
            get { return sensorPowerbox; }
            set { sensorPowerbox = value; }
        }
        [Category("3. Active Sensor")]
        [DisplayName("GPS Sensor")]
        [Description("Emulates Spektrum GPS Sensor.")]
        [TypeConverter(typeof(YesNoConverter))]
        public bool SensorGPS
        {
            get { return sensorGPS; }
            set { sensorGPS = value; }
        }

        public override void DeSerialize(byte[] data)
        {
            Version = data[0];
            Cells = data[1];
            AlarmVoltage2 = data[2];
            AlarmCapacity1 = Converter.ToUInt16(data, 3);

            SensorCurrent = ((Sensor)data[5] & Sensor.Current) == Sensor.Current;
            SensorGPS = ((Sensor)data[5] & Sensor.GPS) == Sensor.GPS;
            sensorPowerbox = ((Sensor)data[5] & Sensor.Powerbox) == Sensor.Powerbox;
        }

        public override byte[] Serialize()
        {
            byte[] data = new byte[128];

            data[0] = Version;
            data[1] = Cells;
            data[2] = AlarmVoltage2;
            Converter.GetBytes(AlarmCapacity1, data, 3);

            Sensor active = Sensor.None;

            if (SensorCurrent)
                active = active | Sensor.Current;

            if (SensorGPS)
                active = active | Sensor.GPS;

            if (SensorPowerBox)
                active = active | Sensor.Powerbox;

            data[5] = (byte)active;

            return data;
        }

        #region Nested Types

        [TypeConverter(typeof(EnumDescriptionTypeConverter))]
        [Flags]
        public enum Sensor : byte
        {
            None = 0,
            GPS = 1,
            Vario = 2,
            Current = 4,
            Powerbox = 8,
        }

        #endregion

    }
}
