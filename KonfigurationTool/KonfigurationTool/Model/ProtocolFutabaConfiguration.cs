using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{
    class ProtocolFutabaConfiguration : ProtocolConfiguration
    {

        private SensorType[] Slots = new SensorType[32];

        public ProtocolFutabaConfiguration()
            : base()
        {
            Version = 1;
            MinFixType = FixType.FIX_2D;

            Slots[1] = SensorType.Vario;
            Slots[3] = SensorType.Current;
            Slots[6] = SensorType.Cell;
            Slots[7] = SensorType.Temperatur;
            Slots[8] = SensorType.GPS;
            Slots[16] = SensorType.Compass;
            Slots[17] = SensorType.Sats;
            Slots[18] = SensorType.Flightmode;
            UpdateSlotMapping();
        }

        [Category("1. General")]
        [DisplayName("Config Version")]
        [Description("Version of Configuration for backward compability")]
        public byte Version { get; private set; }

        [Category("1. General")]
        [DisplayName("Min. Fix Type")]
        [Description("Defines minimum GPS Fix for transmission of GPS depending values")]
        public FixType MinFixType { get; set; }

        #region First Main

        [Category("Slot area 1")]
        [DisplayName("Slot 01")]
        [Description("Mapping for Slot 1")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot1
        {
            get { return GetSlot(1); }
            set
            {
                CheckAssignValue(value, 1);
            }
        }

        [Category("Slot area 1")]
        [DisplayName("Slot 02")]
        [Description("Mapping for Slot 2")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot2
        {
            get { return GetSlot(2); }
            set
            {
                CheckAssignValue(value, 2);
            }
        }

        [Category("Slot area 1")]
        [DisplayName("Slot 03")]
        [Description("Mapping for Slot 3")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot3
        {
            get { return GetSlot(3); }
            set
            {
                CheckAssignValue(value, 3);
            }
        }

        [Category("Slot area 1")]
        [DisplayName("Slot 04")]
        [Description("Mapping for Slot 4")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot4
        {
            get { return GetSlot(4); }
            set
            {
                CheckAssignValue(value, 4);
            }
        }

        [Category("Slot area 1")]
        [DisplayName("Slot 05")]
        [Description("Mapping for Slot 5")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot5
        {
            get { return GetSlot(5); }
            set
            {
                CheckAssignValue(value, 5);
            }
        }

        [Category("Slot area 1")]
        [DisplayName("Slot 06")]
        [Description("Mapping for Slot 6")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot6
        {
            get { return GetSlot(6); }
            set
            {
                CheckAssignValue(value, 6);
            }
        }

        [Category("Slot area 1")]
        [DisplayName("Slot 07")]
        [Description("Mapping for Slot 7")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot7
        {
            get { return GetSlot(7); }
            set
            {
                CheckAssignValue(value, 7);
            }
        }

        #endregion

        #region Second Main

        [Category("Slot area 2")]
        [DisplayName("Slot 08")]
        [Description("Mapping for Slot 8")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot8
        {
            get { return GetSlot(8); }
            set
            {
                CheckAssignValue(value, 8);
            }
        }

        [Category("Slot area 2")]
        [DisplayName("Slot 09")]
        [Description("Mapping for Slot 9")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot9
        {
            get { return GetSlot(9); }
            set
            {
                CheckAssignValue(value, 9);
            }
        }

        [Category("Slot area 2")]
        [DisplayName("Slot 10")]
        [Description("Mapping for Slot 10")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot10
        {
            get { return GetSlot(10); }
            set
            {
                CheckAssignValue(value, 10);
            }
        }

        [Category("Slot area 2")]
        [DisplayName("Slot 11")]
        [Description("Mapping for Slot 11")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot11
        {
            get { return GetSlot(11); }
            set
            {
                CheckAssignValue(value, 11);
            }
        }

        [Category("Slot area 2")]
        [DisplayName("Slot 12")]
        [Description("Mapping for Slot 12")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot12
        {
            get { return GetSlot(12); }
            set
            {
                CheckAssignValue(value, 5);
            }
        }

        [Category("Slot area 2")]
        [DisplayName("Slot 13")]
        [Description("Mapping for Slot 13")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot13
        {
            get { return GetSlot(13); }
            set
            {
                CheckAssignValue(value, 13);
            }
        }

        [Category("Slot area 2")]
        [DisplayName("Slot 14")]
        [Description("Mapping for Slot 14")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot14
        {
            get { return GetSlot(14); }
            set
            {
                CheckAssignValue(value, 14);
            }
        }

        [Category("Slot area 2")]
        [DisplayName("Slot 15")]
        [Description("Mapping for Slot 15")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot15
        {
            get { return GetSlot(15); }
            set
            {
                CheckAssignValue(value, 15);
            }
        }

        #endregion

        #region Thrid Main

        [Category("Slot area 3")]
        [DisplayName("Slot 16")]
        [Description("Mapping for Slot 16")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot16
        {
            get { return GetSlot(16); }
            set
            {
                CheckAssignValue(value, 16);
            }
        }

        [Category("Slot area 3")]
        [DisplayName("Slot 17")]
        [Description("Mapping for Slot 17")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot17
        {
            get { return GetSlot(17); }
            set
            {
                CheckAssignValue(value, 17);
            }
        }

        [Category("Slot area 3")]
        [DisplayName("Slot 18")]
        [Description("Mapping for Slot 18")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot18
        {
            get { return GetSlot(18); }
            set
            {
                CheckAssignValue(value, 18);
            }
        }

        [Category("Slot area 3")]
        [DisplayName("Slot 19")]
        [Description("Mapping for Slot 19")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot19
        {
            get { return GetSlot(19); }
            set
            {
                CheckAssignValue(value, 19);
            }
        }

        [Category("Slot area 3")]
        [DisplayName("Slot 20")]
        [Description("Mapping for Slot 20")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot20
        {
            get { return GetSlot(20); }
            set
            {
                CheckAssignValue(value, 20);
            }
        }

        [Category("Slot area 3")]
        [DisplayName("Slot 21")]
        [Description("Mapping for Slot 21")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot21
        {
            get { return GetSlot(21); }
            set
            {
                CheckAssignValue(value, 21);
            }
        }

        [Category("Slot area 3")]
        [DisplayName("Slot 22")]
        [Description("Mapping for Slot 22")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot22
        {
            get { return GetSlot(22); }
            set
            {
                CheckAssignValue(value, 22);
            }
        }

        [Category("Slot area 3")]
        [DisplayName("Slot 23")]
        [Description("Mapping for Slot 23")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot23
        {
            get { return GetSlot(23); }
            set
            {
                CheckAssignValue(value, 23);
            }
        }

        #endregion

        #region Foorth Main

        [Category("Slot area 4")]
        [DisplayName("Slot 24")]
        [Description("Mapping for Slot 24")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot24
        {
            get { return GetSlot(24); }
            set
            {
                CheckAssignValue(value, 24);
            }
        }

        [Category("Slot area 4")]
        [DisplayName("Slot 25")]
        [Description("Mapping for Slot 25")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot25
        {
            get { return GetSlot(25); }
            set
            {
                CheckAssignValue(value, 25);
            }
        }

        [Category("Slot area 4")]
        [DisplayName("Slot 26")]
        [Description("Mapping for Slot 26")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot26
        {
            get { return GetSlot(26); }
            set
            {
                CheckAssignValue(value, 26);
            }
        }

        [Category("Slot area 4")]
        [DisplayName("Slot 27")]
        [Description("Mapping for Slot 27")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot27
        {
            get { return GetSlot(27); }
            set
            {
                CheckAssignValue(value, 27);
            }
        }

        [Category("Slot area 4")]
        [DisplayName("Slot 28")]
        [Description("Mapping for Slot 28")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot28
        {
            get { return GetSlot(28); }
            set
            {
                CheckAssignValue(value, 28);
            }
        }

        [Category("Slot area 4")]
        [DisplayName("Slot 29")]
        [Description("Mapping for Slot 29")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot29
        {
            get { return GetSlot(29); }
            set
            {
                CheckAssignValue(value, 29);
            }
        }

        [Category("Slot area 4")]
        [DisplayName("Slot 30")]
        [Description("Mapping for Slot 30")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot30
        {
            get { return GetSlot(30); }
            set
            {
                CheckAssignValue(value, 30);
            }
        }

        [Category("Slot area 4")]
        [DisplayName("Slot 31")]
        [Description("Mapping for Slot 31")]
        [ReadOnlyAttribute(false)]
        public SensorType Slot31
        {
            get { return GetSlot(31); }
            set
            {
                CheckAssignValue(value, 31);
            }
        }

        #endregion

        private SensorType GetSlot(byte slot)
        {
            if (Slots[slot] != SensorType.None)
                return Slots[slot];

            byte tmpSlot = (byte)(slot - 1);
            while (tmpSlot > 0)
            {
                if (Slots[tmpSlot] != SensorType.None)
                {
                    if (slot < (tmpSlot + GetSlots(Slots[tmpSlot])))
                        return Slots[tmpSlot];

                    break;
                }

                tmpSlot--;
            }

            return Slots[slot];
        }

        private void CheckAssignValue(SensorType sensor, byte slot)
        {
            byte size = GetSlots(sensor);
            byte areaLeft = (byte)(8 - (slot % 8));
            if (size > areaLeft)
                throw new InvalidOperationException(string.Format("{0} needs {1} slots, but only {2} slots left in this Slot area.", sensor.GetDescription(), size, areaLeft));

            for (byte i = 1; i < size; i++)
            {
                if (Slots[slot + i] != SensorType.None)
                    throw new InvalidOperationException(string.Format("{0} needs {1} slots, but slot {2} is already assigned with {3}.", sensor.GetDescription(), size, slot + i, Slots[slot + i].GetDescription()));
            }

            Slots[slot] = sensor;
            UpdateSlotMapping();
        }

        private void UpdateSlotMapping()
        {
            byte size = 0;

            PropertyDescriptor propDescr;
            ReadOnlyAttribute attr;
            System.Reflection.FieldInfo aField;
            for (byte i = 1; i < Slots.Length; i++)
            {
                propDescr = TypeDescriptor.GetProperties(this.GetType())["Slot" + i];
                attr = propDescr != null ? propDescr.Attributes[typeof(ReadOnlyAttribute)] as ReadOnlyAttribute : null;
                aField = attr != null ? attr.GetType().GetField("isReadOnly", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance) : null;

                if (size > 0)
                {
                    if (aField != null)
                        aField.SetValue(attr, true);
                    size--;
                }
                else
                {
                    if (aField != null)
                        aField.SetValue(attr, false);

                    size = (byte)(GetSlots(Slots[i]) - 1);
                }
            }
        }

        private byte GetSlots(SensorType sensor)
        {
            switch (sensor)
            {
                case SensorType.Vario:
                    return 2;
                case SensorType.Current:
                    return 3;
                case SensorType.GPS:
                    return 8;
                default:
                    return 1;
            }
        }

        public override void DeSerialize(byte[] data)
        {
            Version = data[0];
            MinFixType = (FixType)data[1];
            for (int i = 0; i < Slots.Length; i++)
                Slots[i] = (SensorType)data[2 + i];

            UpdateSlotMapping();
        }

        public override byte[] Serialize()
        {
            byte[] data = new byte[128];


            data[0] = Version;
            data[1] = (byte)MinFixType;

            for (int i = 0; i < Slots.Length; i++)
                data[2 + i] = (byte)Slots[i];


            return data;
        }

        #region Nested Types

        [TypeConverter(typeof(EnumDescriptionTypeConverter))]
        public enum SensorType : byte
        {
            None = 0,
            [Description("Vario (VARIO-1672)")]
            Vario = 1,
            [Description("Compass (SBS-01RM/0)")]
            Compass = 2,
            [Description("Current (CURR-1678)")]
            Current = 3,
            [Description("GPS (SBS-01G)")]
            GPS = 4,
            [Description("Cell (SBS-01RM/0)")]
            Cell = 5,
            [Description("Sats (SBS-01T)")]
            Sats = 6,
            [Description("Flight Mode (SBS-01T)")]
            Flightmode = 7,
            [Description("Temperatur (SBS-01T)")]
            Temperatur = 8,
        }

        #endregion
    }
}
