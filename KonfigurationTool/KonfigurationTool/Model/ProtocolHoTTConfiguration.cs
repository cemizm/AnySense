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
            Version = 1;
            Active = Sensors.GPS_AND_GAM;
            Cells = 3;
            VoltageAlarms = new VoltageAlarm[] { 
                new VoltageAlarm() { Voltage = 15, Tone=HoTTAlarmTone.Tone_A, Interval = 3, Invert=true, Repeat=0 }, 
                new VoltageAlarm(), 
                new VoltageAlarm(), };
            DistanceAlarms = new DistanceAlarm[] { 
                new DistanceAlarm() { Distance=3000, Tone=HoTTAlarmTone.Tone_P, Interval=10, Invert=true, Repeat=0 }, 
                new DistanceAlarm(), 
                new DistanceAlarm(), };
        }

        [Category("1. General")]
        [DisplayName("Config Version")]
        [Description("Version of Configuration for backward compability")]
        public byte Version { get; private set; }

        [Category("1. General")]
        [DisplayName("Active Sensors")]
        [Description("Defines the active Sensors to simulate")]
        public Sensors Active { get; set; }


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

        [Category("2. Alarms")]
        [DisplayName("Voltage Alarms")]
        [Description("Defines alarms for Voltage")]
        public VoltageAlarm[] VoltageAlarms { get; private set; }

        [Category("2. Alarms")]
        [DisplayName("Distance Alarms")]
        [Description("Defines alarms for distance")]
        public DistanceAlarm[] DistanceAlarms { get; private set; }

        public override void DeSerialize(byte[] data)
        {
            Version = data[0];
            Cells = data[1];
            Active = (Sensors)data[2];

            int offset = 3;

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

            data[0] = Version;
            data[1] = Cells;
            data[2] = (byte)Active;

            int offset = 3;
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
            GPS = 1,
            GAM = 2,
            GPS_AND_GAM = 3,
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
