using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{

    [TypeConverter(typeof(EnumDescriptionTypeConverter))]
    enum ProtocolType : byte
    {
        [Description("Disabled")]
        None = 0,
        [Description("FrSky S.Port")]
        FrSky_SPort = 1,
        [Description("Mavlink")]
        Mavlink = 2,
        [Description("Graupner/SJ HoTT")]
        HOTT = 3,
        [Description("Jeti Duplex EX")]
        Jeti = 4,
        [Description("Futaba SBUS2")]
        Futaba = 5,
        [Description("Spektrum X-Bus")]
        Spektrum = 6,
    }
}
