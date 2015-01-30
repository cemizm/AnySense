using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{
    [TypeConverter(typeof(EnumDescriptionTypeConverter))]
    enum TelemetryValue : byte
    {
        [Description("Disabled")]
        None = 0,
        [Description("GPS Position")]
        GPS_Position = 1,
        [Description("Altitude")]
        Altitude = 2,
        [Description("GPS Altitude")]
        GPS_Altitude = 3,
        [Description("GPS Speed")]
        Speed = 4,
        [Description("GPS Fix")]
        GPS_Fix = 5,
        [Description("GPS Sats")]
        NumSat = 6,
        [Description("Heading (tilt compensated)")]
        HeadingComp = 7,
        [Description("Heading")]
        Heading = 8,
        [Description("COG (Course over Ground)")]
        COG = 9,
        [Description("Vario")]
        Vario = 10,
        [Description("GPS Vario")]
        GPS_Vario = 11,
        [Description("HDOP")]
        HDOP = 12,
        [Description("VDOP")]
        VDOP = 13,
        [Description("Pitch")]
        Pitch = 14,
        [Description("Roll")]
        Roll = 15,
        [Description("Main Voltage")]
        Voltage = 16,
        [Description("Throttle")]
        Throttle = 17,
        [Description("Flight Mode")]
        Flight_mode = 18,
        [Description("Last FC Heartbeat")]
        LastHeartbeat = 19,
        [Description("GPS Time")]
        GPS_Time = 20,
        [Description("Altitude (relative to Home)")]
        Altitude_Relative = 21,
        [Description("Current (accumulated)")]
        Current = 22,
        [Description("Cells (combined)")]
        Cells = 23,
    }
}
