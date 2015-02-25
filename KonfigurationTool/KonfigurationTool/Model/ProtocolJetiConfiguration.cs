using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{
    class ProtocolJetiConfiguration : ProtocolConfiguration
    {
        private TelemetryValue[] telemetryValue;
        private Alarm[] alarms;

        const int MAX_VALUES = 15;
        const int MAX_ALARMS = 6;

        public ProtocolJetiConfiguration()
            : base()
        {
            Version = 1;
            telemetryValue = new TelemetryValue[MAX_VALUES];
            alarms = new Alarm[MAX_ALARMS];

            telemetryValue[0] = TelemetryValue.Latitude;
            telemetryValue[1] = TelemetryValue.Longitude;
            telemetryValue[2] = TelemetryValue.Satellite;
            telemetryValue[3] = TelemetryValue.GPS_Fix;
            telemetryValue[4] = TelemetryValue.Speed;
            telemetryValue[5] = TelemetryValue.Distance;
            telemetryValue[6] = TelemetryValue.Altitude;
            telemetryValue[7] = TelemetryValue.VSpeed;
            telemetryValue[8] = TelemetryValue.Compass;
            telemetryValue[9] = TelemetryValue.HomeDirection;
            telemetryValue[10] = TelemetryValue.Voltage;
            telemetryValue[11] = TelemetryValue.Current;
            telemetryValue[12] = TelemetryValue.Capacity;
            telemetryValue[13] = TelemetryValue.LipoVoltage;
            telemetryValue[14] = TelemetryValue.LipoTemp;


            alarms[0] = new Alarm() { Event = EventType.A, Value = 500, Type = AlarmType.Alitutude };
            alarms[1] = new Alarm() { Event = EventType.D, Value = 2000, Type = AlarmType.Distance };
            alarms[2] = new Alarm() { Event = EventType.U, Value = 11500, Type = AlarmType.Battery };
            alarms[3] = new Alarm() { Event = EventType.C, Value = 4500, Type = AlarmType.Capacity };
            alarms[4] = new Alarm() { Event = EventType.A, Value = 0, Type = AlarmType.None };
            alarms[5] = new Alarm() { Event = EventType.A, Value = 0, Type = AlarmType.None };
        }

        #region "Properties"

        [Category("1. General")]
        [DisplayName("Config Version")]
        [Description("Version of Configuration for backward compability")]
        public byte Version { get; private set; }

        [Category("2. Telemetry Values")]
        [DisplayName("Value 01")]
        public TelemetryValue Value1
        {
            get { return telemetryValue[0]; }
            set { telemetryValue[0] = value; }
        }

        [Category("2. Telemetry Values")]
        [DisplayName("Value 02")]
        public TelemetryValue Value2
        {
            get { return telemetryValue[1]; }
            set { telemetryValue[1] = value; }
        }

        [Category("2. Telemetry Values")]
        [DisplayName("Value 03")]
        public TelemetryValue Value3
        {
            get { return telemetryValue[2]; }
            set { telemetryValue[2] = value; }
        }

        [Category("2. Telemetry Values")]
        [DisplayName("Value 04")]
        public TelemetryValue Value4
        {
            get { return telemetryValue[3]; }
            set { telemetryValue[3] = value; }
        }

        [Category("2. Telemetry Values")]
        [DisplayName("Value 05")]
        public TelemetryValue Value5
        {
            get { return telemetryValue[4]; }
            set { telemetryValue[4] = value; }
        }

        [Category("2. Telemetry Values")]
        [DisplayName("Value 06")]
        public TelemetryValue Value6
        {
            get { return telemetryValue[5]; }
            set { telemetryValue[5] = value; }
        }

        [Category("2. Telemetry Values")]
        [DisplayName("Value 07")]
        public TelemetryValue Value7
        {
            get { return telemetryValue[6]; }
            set { telemetryValue[6] = value; }
        }

        [Category("2. Telemetry Values")]
        [DisplayName("Value 08")]
        public TelemetryValue Value8
        {
            get { return telemetryValue[7]; }
            set { telemetryValue[7] = value; }
        }

        [Category("2. Telemetry Values")]
        [DisplayName("Value 09")]
        public TelemetryValue Value9
        {
            get { return telemetryValue[8]; }
            set { telemetryValue[8] = value; }
        }

        [Category("2. Telemetry Values")]
        [DisplayName("Value 10")]
        public TelemetryValue Value10
        {
            get { return telemetryValue[9]; }
            set { telemetryValue[9] = value; }
        }

        [Category("2. Telemetry Values")]
        [DisplayName("Value 11")]
        public TelemetryValue Value11
        {
            get { return telemetryValue[10]; }
            set { telemetryValue[10] = value; }
        }

        [Category("2. Telemetry Values")]
        [DisplayName("Value 12")]
        public TelemetryValue Value12
        {
            get { return telemetryValue[11]; }
            set { telemetryValue[11] = value; }
        }

        [Category("2. Telemetry Values")]
        [DisplayName("Value 13")]
        public TelemetryValue Value13
        {
            get { return telemetryValue[12]; }
            set { telemetryValue[12] = value; }
        }

        [Category("2. Telemetry Values")]
        [DisplayName("Value 14")]
        public TelemetryValue Value14
        {
            get { return telemetryValue[13]; }
            set { telemetryValue[13] = value; }
        }

        [Category("2. Telemetry Values")]
        [DisplayName("Value 15")]
        public TelemetryValue Value15
        {
            get { return telemetryValue[14]; }
            set { telemetryValue[14] = value; }
        }

        [Category("3. Alarms")]
        [DisplayName("Alarm 1")]
        public Alarm Alarm1 { get { return alarms[0]; } }

        [Category("3. Alarms")]
        [DisplayName("Alarm 2")]
        public Alarm Alarm2 { get { return alarms[1]; } }

        [Category("3. Alarms")]
        [DisplayName("Alarm 3")]
        public Alarm Alarm3 { get { return alarms[2]; } }

        [Category("3. Alarms")]
        [DisplayName("Alarm 4")]
        public Alarm Alarm4 { get { return alarms[3]; } }

        [Category("3. Alarms")]
        [DisplayName("Alarm 5")]
        public Alarm Alarm5 { get { return alarms[4]; } }

        [Category("3. Alarms")]
        [DisplayName("Alarm 6")]
        public Alarm Alarm6 { get { return alarms[5]; } }

        #endregion

        public override void DeSerialize(byte[] data)
        {
            byte pos = 0;

            Version = data[pos++];
            for (int i = 0; i < telemetryValue.Length; i++)
                telemetryValue[i] = (TelemetryValue)data[pos++];


            for (int i = 0; i < alarms.Length; i++)
            {
                Alarm alarm = new Alarm();
                alarm.Value = Converter.ToUInt16(data, pos++); pos++;
                alarm.Event = (EventType)data[pos++];
                alarm.Type = (AlarmType)data[pos++];
                alarms[i] = alarm;
            }
        }

        public override byte[] Serialize()
        {
            byte[] data = new byte[128];

            byte pos = 0;

            data[pos++] = Version;

            for (int i = 0; i < telemetryValue.Length; i++)
                data[pos++] = (byte)telemetryValue[i];

            for (int i = 0; i < alarms.Length; i++)
            {
                Converter.GetBytes(alarms[i].Value, data, pos++); pos++;

                data[pos++] = (byte)alarms[i].Event;
                data[pos++] = (byte)alarms[i].Type;
            }



            return data;
        }

        #region Nested Types

        [TypeConverter(typeof(EnumDescriptionTypeConverter))]
        public enum TelemetryValue : byte
        {
            [Description("Disabled")]
            None = 0,
            Latitude = 1,
            Longitude = 2,
            [Description("Satellites")]
            Satellite = 3,
            [Description("GPS Fix")]
            GPS_Fix = 4,
            Flightmode = 5,
            Speed = 6,
            Distance = 7,
            Altitude = 8,
            [Description("Vertical Speed (Vario)")]
            VSpeed = 9,
            Compass = 10,
            [Description("Flightdirection (COG)")]
            COG = 11,
            [Description("Home Direction")]
            HomeDirection = 12,
            Voltage = 13,
            Current = 14,
            Capacity = 15,
            [Description("Lipo Voltage (lowest)")]
            LipoVoltage = 16,
            [Description("Lipo Temperature")]
            LipoTemp = 17,
            [Description("Home Latitude")]
            Home_Latitude = 18,
            [Description("Home Longitude")]
            Home_Longitude = 19,

        };

        [TypeConverter(typeof(EnumDescriptionTypeConverter))]
        public enum AlarmType : byte
        {

            [Description("Disabled")]
            None = 0,

            Alitutude = 1,

            Capacity = 2,

            Distance = 3,

            [Description("Voltage")]
            Battery = 4,

        }

        [TypeConverter(typeof(EnumDescriptionTypeConverter))]
        public enum EventType : byte
        {
            [Description("Alarm - A")]
            A = 65,
            [Description("Alarm - B")]
            B = 66,
            [Description("Alarm - C")]
            C = 67,
            [Description("Alarm - D")]
            D = 68,
            [Description("Alarm - E")]
            E = 69,
            [Description("Alarm - F")]
            F = 70,
            [Description("Alarm - G")]
            G = 71,
            [Description("Alarm - H")]
            H = 72,
            [Description("Alarm - I")]
            I = 73,
            [Description("Alarm - J")]
            J = 74,
            [Description("Alarm - K")]
            K = 75,
            [Description("Alarm - L")]
            L = 76,
            [Description("Alarm - M")]
            M = 77,
            [Description("Alarm - N")]
            N = 78,
            [Description("Alarm - O")]
            O = 79,
            [Description("Alarm - P")]
            P = 80,
            [Description("Alarm - Q")]
            Q = 81,
            [Description("Alarm - R")]
            R = 82,
            [Description("Alarm - S")]
            S = 83,
            [Description("Alarm - T")]
            T = 84,
            [Description("Alarm - U")]
            U = 85,
            [Description("Alarm - V")]
            V = 86,
            [Description("Alarm - W")]
            W = 87,
            [Description("Alarm - X")]
            X = 88,
            [Description("Alarm - Y")]
            Y = 89,
            [Description("Alarm - Z")]
            Z = 90,
        }

        [TypeConverter(typeof(AlarmConveter))]
        [DisplayName("Alarm")]
        public class Alarm : INotifyPropertyChanged
        {
            private AlarmType type;
            private UInt16 _value;
            private EventType _event;

            [DisplayName("Alarm Type")]
            [Description("Type of Alarm")]
            public AlarmType Type
            {
                get { return type; }
                set
                {
                    if (type != value)
                    {
                        type = value;
                        RaisePropertyChanged("Type");
                        RaisePropertyChanged("Value");
                    }
                }
            }

            [DisplayName("Condition Value")]
            [Description("Value to exceed/deceed, depending on Alarm Type.")]
            public UInt16 Value
            {
                get { return _value; }
                set
                {
                    if (_value != value)
                    {
                        _value = value;
                        RaisePropertyChanged("Value");
                    }
                }
            }

            [Description("Event to execute, see alarms on your Transmitter / Jeti Box Profi")]
            public EventType Event
            {
                get { return _event; }
                set
                {
                    if (_event != value)
                    {
                        _event = value;
                        RaisePropertyChanged("Event");
                    }
                }
            }

            public event PropertyChangedEventHandler PropertyChanged;

            protected virtual void RaisePropertyChanged(string property)
            {
            }

            protected virtual void OnPropertyChanged(PropertyChangedEventArgs e)
            {
                if (PropertyChanged != null)
                    PropertyChanged(this, e);
            }
        }

        class AlarmConveter : ExpandableObjectConverter
        {
            public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType)
            {
                if (destinationType == typeof(string) && value is Alarm)
                {
                    Alarm alarm = (Alarm)value;

                    if (alarm.Type == AlarmType.None)
                        return "Alarm Disabled";
                    if (alarm.Type == AlarmType.Alitutude)
                        return string.Format("Alitutde: exeeds {0}m > Event {1}", alarm.Value, alarm.Event.GetDescription());
                    if (alarm.Type == AlarmType.Distance)
                        return string.Format("Distance: exeeds {0}m > Event {1}", alarm.Value, alarm.Event.GetDescription());
                    if (alarm.Type == AlarmType.Capacity)
                        return string.Format("Capacity: exeeds {0}mAh > Event {1}", alarm.Value, alarm.Event.GetDescription());
                    if (alarm.Type == AlarmType.Battery)
                        return string.Format("Battery: deceeds {0}mV > Event {1}", alarm.Value, alarm.Event.GetDescription());

                    return alarm.ToString();
                }
                return base.ConvertTo(context, culture, value, destinationType);
            }

            public override PropertyDescriptorCollection GetProperties(ITypeDescriptorContext context, object value, Attribute[] attributes)
            {
                PropertyDescriptorCollection props = base.GetProperties(context, value, attributes);

                if (value is Alarm)
                {
                    Alarm alarm = (Alarm)value;
                    List<PropertyDescriptor> list = new List<PropertyDescriptor>(props.Count);
                    foreach (PropertyDescriptor prop in props)
                    {
                        switch (prop.Name)
                        {
                            case "Value":
                                switch (alarm.Type)
                                {
                                    case AlarmType.None:
                                        list.Add(new DisplayNamePropertyDescriptor(prop, "Condition Value", ""));
                                        break;
                                    case AlarmType.Alitutude:
                                        list.Add(new DisplayNamePropertyDescriptor(prop, "Condition Value", "Altitude in meters to exeed for alarm condition"));
                                        break;
                                    case AlarmType.Capacity:
                                        list.Add(new DisplayNamePropertyDescriptor(prop, "Condition Value", "Capacity in milliampere hours (mAh) to exeed for alarm condition"));
                                        break;
                                    case AlarmType.Distance:
                                        list.Add(new DisplayNamePropertyDescriptor(prop, "Condition Value", "Distance in meters to exeed for alarm condition"));
                                        break;
                                    case AlarmType.Battery:
                                        list.Add(new DisplayNamePropertyDescriptor(prop, "Condition Value", "Voltage in millivolts (mV) to exeed for alarm condition"));
                                        break;
                                    default:
                                        break;
                                }

                                break;
                            default:
                                list.Add(prop);
                                break;
                        }
                    }
                    return new PropertyDescriptorCollection(list.ToArray(), true);
                }

                return props;
            }
        }


        class DisplayNamePropertyDescriptor : PropertyDescriptor
        {
            private readonly string displayName;
            private readonly PropertyDescriptor parent;
            private readonly string description;
            public DisplayNamePropertyDescriptor(PropertyDescriptor parent, string displayName, string description)
                : base(parent)
            {
                this.displayName = displayName;
                this.description = description;
                this.parent = parent;
            }
            public override string DisplayName
            { get { return displayName; } }

            public override bool ShouldSerializeValue(object component)
            { return parent.ShouldSerializeValue(component); }

            public override void SetValue(object component, object value)
            {
                parent.SetValue(component, value);
            }
            public override object GetValue(object component)
            {
                return parent.GetValue(component);
            }
            public override void ResetValue(object component)
            {
                parent.ResetValue(component);
            }
            public override bool CanResetValue(object component)
            {
                return parent.CanResetValue(component);
            }
            public override bool IsReadOnly
            {
                get { return parent.IsReadOnly; }
            }
            public override void AddValueChanged(object component, EventHandler handler)
            {
                parent.AddValueChanged(component, handler);
            }
            public override void RemoveValueChanged(object component, EventHandler handler)
            {
                parent.RemoveValueChanged(component, handler);
            }
            public override bool SupportsChangeEvents
            {
                get { return parent.SupportsChangeEvents; }
            }
            public override Type PropertyType
            {
                get { return parent.PropertyType; }
            }
            public override TypeConverter Converter
            {
                get { return parent.Converter; }
            }
            public override Type ComponentType
            {
                get { return parent.ComponentType; }
            }
            public override string Description
            {
                get { return description; }
            }
            public override PropertyDescriptorCollection GetChildProperties(object instance, Attribute[] filter)
            {
                return parent.GetChildProperties(instance, filter);
            }
            public override string Name
            {
                get { return parent.Name; }
            }

        }


        #endregion

    }

}
