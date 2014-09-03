using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{
    enum TelemetryValue : byte
    {
        None = 0,
        GPS_Position = 1,
        Altitude = 2,
        GPS_Altitude = 3,
        Speed = 4,
        GPS_Fix = 5,
        NumSat = 6,
        HeadingComp = 7,
        Heading = 8,
        COG = 9,
        Vario = 10,
        GPS_Vario = 11,
        HDOP = 12,
        VDOP = 13,
        Pitch = 14,
        Roll = 15,
        Voltage = 16,
        Throttle = 17,
        Flight_mode = 18,
        LastHeartbeat = 19,
        GPS_Time = 20,
    }
}
