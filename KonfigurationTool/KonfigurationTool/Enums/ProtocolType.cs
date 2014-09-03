using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{
    enum ProtocolType : byte
    {
        None = 0,
        FrSky_SPort = 1,
        Mavlink = 2,
        HOTT = 3,
        Jeti = 4,
    }
}
