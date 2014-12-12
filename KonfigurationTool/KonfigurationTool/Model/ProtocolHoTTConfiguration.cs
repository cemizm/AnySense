using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{
    class ProtocolHoTTConfiguration : ProtocolConfiguration
    {
        const int VOLTAGE_ALARMS = 3;
        const int DISTANCE_ALARMS = 3;
        private byte cells;

        public ProtocolHoTTConfiguration()
            : base()
        {
            Version = 2;

            Cells = 3;
            GPSAltSource = AltSourceEnum.Baro;
            GPSFlightdirectionSource = FlightDirSourceEnum.GPS;

            ActiveGPS = true;
            ActiveEAM = true;
            ActiveGAM = true;
            ActiveVario = true;

            VoltageAlarms = new VoltageAlarm[] { 
                new VoltageAlarm() { Voltage = 20, Tone=HoTTAlarmTone.Tone_P, Interval = 8, Invert=true, Repeat=0 }, 
                new VoltageAlarm() { Voltage = 10, Tone=HoTTAlarmTone.Tone_R, Interval = 5, Invert=true, Repeat=0 }, 
                new VoltageAlarm() { Voltage = 5, Tone=HoTTAlarmTone.Tone_S, Interval = 2, Invert=true, Repeat=0 },  };
            DistanceAlarms = new DistanceAlarm[] { 
                new DistanceAlarm() { Distance=3000, Tone=HoTTAlarmTone.Tone_D, Interval=10, Invert=true, Repeat=0 }, 
                new DistanceAlarm(), 
                new DistanceAlarm(), };
        }

        [Category("1. General")]
        [DisplayName("Config Version")]
        [Description("Version of Configuration for backward compability")]
        public byte Version { get; private set; }

        [Category("1. General")]
        [DisplayName("Number of cells")]
        [Description("Defines the number of cells used")]
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

        [Category("1. General")]
        [DisplayName("Altitude/Vario Source")]
        [Description("Defines the source of Altitude and Vario")]
        public AltSourceEnum GPSAltSource { get; set; }

        [Category("1. General")]
        [DisplayName("GPS Flightdirection Source")]
        [Description("Defines the source of GPS Flightdirectio")]
        public FlightDirSourceEnum GPSFlightdirectionSource { get; set; }

        [Category("2. Active Sensors")]
        [DisplayName("General Air Modul")]
        [Description("General Air modul enabled")]
        public bool ActiveGAM { get; set; }

        [Category("2. Active Sensors")]
        [DisplayName("Electric Air Modul")]
        [Description("Electric Air Modul enbabled")]
        public bool ActiveEAM { get; set; }

        [Category("2. Active Sensors")]
        [DisplayName("GPS")]
        [Description("GPS enbabled")]
        public bool ActiveGPS { get; set; }

        [Category("2. Active Sensors")]
        [DisplayName("Vario")]
        [Description("Vario enbabled")]
        public bool ActiveVario { get; set; }

        [Category("3. Alarms")]
        [DisplayName("Voltage Alarms")]
        [Description("Defines alarms for Voltage")]
        public VoltageAlarm[] VoltageAlarms { get; private set; }

        [Category("3. Alarms")]
        [DisplayName("Distance Alarms")]
        [Description("Defines alarms for distance")]
        public DistanceAlarm[] DistanceAlarms { get; private set; }

        public override void DeSerialize(byte[] data)
        {
            Version = data[0];
            Cells = data[1];

            ActiveEAM = ((Sensors)data[2] & Sensors.EAM) == Sensors.EAM;
            ActiveGAM = ((Sensors)data[2] & Sensors.GAM) == Sensors.GAM;
            ActiveGPS = ((Sensors)data[2] & Sensors.GPS) == Sensors.GPS;
            ActiveVario = ((Sensors)data[2] & Sensors.Vario) == Sensors.Vario;

            GPSAltSource = (AltSourceEnum)data[3];
            GPSFlightdirectionSource = (FlightDirSourceEnum)data[4];
            int offset = 5;

            List<VoltageAlarm> voltageAlarms = new List<VoltageAlarm>();
            for (int i = 0; i < VOLTAGE_ALARMS; i++)
            {
                VoltageAlarm alarm = new VoltageAlarm();
                alarm.Voltage = data[offset++];
                alarm.Tone = (HoTTAlarmTone)data[offset++];
                alarm.Interval = data[offset++];
                alarm.Repeat = data[offset++];
                alarm.Invert = data[offset++] > 0;
                voltageAlarms.Add(alarm);
            }
            VoltageAlarms = voltageAlarms.ToArray();

            List<DistanceAlarm> distanceAlarms = new List<DistanceAlarm>();
            for (int i = 0; i < DISTANCE_ALARMS; i++)
            {
                DistanceAlarm alarm = new DistanceAlarm();
                alarm.Distance = Converter.ToUInt16(data, offset++); offset++;
                alarm.Tone = (HoTTAlarmTone)data[offset++];
                alarm.Interval = data[offset++];
                alarm.Repeat = data[offset++];
                alarm.Invert = data[offset++] > 0;
                distanceAlarms.Add(alarm);
            }
            DistanceAlarms = distanceAlarms.ToArray();
        }

        public override byte[] Serialize()
        {
            byte[] data = new byte[128];

            Sensors active = Sensors.None;

            if (ActiveEAM)
                active = active | Sensors.EAM;

            if (ActiveGAM)
                active = active | Sensors.GAM;

            if (ActiveGPS)
                active = active | Sensors.GPS;

            if (ActiveVario)
                active = active | Sensors.Vario;

            data[0] = Version;
            data[1] = Cells;
            data[2] = (byte)active;
            data[3] = (byte)GPSAltSource;
            data[4] = (byte)GPSFlightdirectionSource;

            int offset = 5;
            foreach (VoltageAlarm alarm in VoltageAlarms)
            {
                data[offset++] = alarm.Voltage;
                data[offset++] = (byte)alarm.Tone;
                data[offset++] = alarm.Interval;
                data[offset++] = alarm.Repeat;
                data[offset++] = alarm.Invert ? (byte)1 : (byte)0;
            }

            foreach (DistanceAlarm alarm in DistanceAlarms)
            {
                Converter.GetBytes(alarm.Distance, data, offset);
                offset += 2;
                data[offset++] = (byte)alarm.Tone;
                data[offset++] = alarm.Interval;
                data[offset++] = alarm.Repeat;
                data[offset++] = alarm.Invert ? (byte)1 : (byte)0;
            }

            return data;
        }

        #region Nested Class/Enum

        public enum Sensors : byte
        {
            None = 0,
            GPS = 1,
            GAM = 2,
            EAM = 4, 
            Vario = 8,
        }

        public enum AltSourceEnum : byte
        {
            GPS = 0,
            Baro = 1,
        }

        public enum FlightDirSourceEnum : byte
        {
            GPS = 0,
            Compass = 1,
        }

        public abstract class Alarm
        {
            [Category("2. Action")]
            [DisplayName("Alarm Tone")]
            [Description("Defines alarms for distance")]
            public HoTTAlarmTone Tone { get; set; }

            [Category("2. Action")]
            [DisplayName("Repeat Alarm")]
            public byte Repeat { get; set; }

            [Category("2. Action")]
            [DisplayName("Interval in seconds")]
            public byte Interval { get; set; }

            [Category("2. Action")]
            [DisplayName("Invert Text Column")]
            public bool Invert { get; set; }

            public abstract string GetTitle();
        }

        [TypeConverter(typeof(AlarmConveter))]
        [DisplayName("Voltage Alarm")]
        public class VoltageAlarm : Alarm
        {
            private byte voltage;

            [Category("1. Event")]
            [DisplayName("Voltage in percent")]
            [Description("Defines the voltage to deceed in percent.")]
            public byte Voltage
            {
                get { return voltage; }
                set
                {
                    if (voltage != value)
                    {
                        if (voltage > 100)
                            throw new InvalidOperationException("Voltage cannot exceed 100 percent.");

                        voltage = value;
                    }
                }
            }

            public override string GetTitle()
            {
                return "lower " + Voltage + "% -> " + Tone;
            }
        }

        [TypeConverter(typeof(AlarmConveter))]
        [DisplayName("Distance Alarm")]
        public class DistanceAlarm : Alarm
        {
            [Category("1. Event")]
            [DisplayName("Distance in meters")]
            [Description("Defines the distance to exceed.")]
            public UInt16 Distance { get; set; }


            public override string GetTitle()
            {
                return "exceed " + Distance + "m -> " + Tone;
            }
        }

        class AlarmConveter : ExpandableObjectConverter
        {
            public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType)
            {
                if (destinationType == typeof(string) && value is Alarm)
                {
                    Alarm alarm = (Alarm)value;
                    return alarm.GetTitle();
                }
                return base.ConvertTo(context, culture, value, destinationType);
            }
        }

        #endregion

    }
}
