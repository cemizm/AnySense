using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{
    class ProtocolMavlinkConfiguration : ProtocolConfiguration
    {
        const int VOLTAGE_ALARMS = 3;
        const int DISTANCE_ALARMS = 3;
        private byte cells;

        public ProtocolMavlinkConfiguration()
            : base()
        {
            Version = 1;

            Cells = 3;
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


        public override void DeSerialize(byte[] data)
        {
            Version = data[0];
            Cells = data[1];

        }

        public override byte[] Serialize()
        {
            byte[] data = new byte[128];

            data[0] = Version;
            data[1] = Cells;
           
            return data;
        }

        

    }
}
