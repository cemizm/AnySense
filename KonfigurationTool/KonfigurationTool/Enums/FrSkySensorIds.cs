using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{
    [TypeConverter(typeof(EnumDescriptionTypeConverter))]
    internal enum FrSkySensorId : byte
    {

        [Description("Phy. Id 01 (FVAS-02)")]
        Id_0x00 = 0x00, //Physical 01 - Vario
        [Description("Phy. Id 02 (FLVSS)")]
        Id_0xA1 = 0xA1, //Physical 02 - FLVSS
        [Description("Phy. Id 03 (FCS-40A)")]
        Id_0x22 = 0x22, //Physical 03 - FCS 40A
        [Description("Phy. Id 04 (RPMS-02)")]
        Id_0x83 = 0x83, //Physical 04 - RPM
        [Description("Phy. Id 05 (SP2UART Host)")]
        Id_0xE4 = 0xE4, //Physical 05 - SP2UART (Host)
        [Description("Phy. Id 06 (SP2UART Remote)")]
        Id_0x45 = 0x45, //Physical 06 - SP2UART (Remote)
        [Description("Phy. Id 07")]
        Id_0xC6 = 0xC6, //Physical 07 
        [Description("Phy. Id 08 (FCS-150A)")]
        Id_0x67 = 0x67, //Physical 08 - FCS 150A
        [Description("Phy. Id 09")]
        Id_0x48 = 0x48, //Physical 09
        [Description("Phy. Id 10")]
        Id_0xE9 = 0xE9, //Physical 10
        [Description("Phy. Id 11")]
        Id_0x6A = 0x6A, //Physical 11
        [Description("Phy. Id 12")]
        Id_0xCB = 0xCB, //Physical 12
        [Description("Phy. Id 13")]
        Id_0xAC = 0xAC, //Physical 13
        [Description("Phy. Id 14")]
        Id_0x0D = 0x0D, //Physical 14
        [Description("Phy. Id 15")]
        Id_0x8E = 0x8E, //Physical 15
        [Description("Phy. Id 16")]
        Id_0x2F = 0x2F, //Physical 16

        [Description("Phy. Id 17")]
        Id_0xD0 = 0xD0, //Physical 17
        [Description("Phy. Id 18")]
        Id_0x71 = 0x71, //Physical 18
        [Description("Phy. Id 19")]
        Id_0xF2 = 0xF2, //Physical 19
        [Description("Phy. Id 20")]
        Id_0x53 = 0x53, //Physical 20
        [Description("Phy. Id 21")]
        Id_0x34 = 0x34, //Physical 21
        [Description("Phy. Id 22")]
        Id_0x95 = 0x95, //Physical 22
        [Description("Phy. Id 23")]
        Id_0x16 = 0x16, //Physical 23
        [Description("Phy. Id 24")]
        Id_0xB7 = 0xB7, //Physical 24
        [Description("Phy. Id 25")]
        Id_0x98 = 0x98, //Physical 25
        [Description("Phy. Id 26")]
        Id_0x39 = 0x39, //Physical 26
        [Description("Phy. Id 27")]
        Id_0xBA = 0xBA, //Physical 27
        [Description("Phy. Id 28")]
        Id_0x1B = 0x1B, //Physical 28
        [Description("Phy. Id 29")]
        Id_0x7E = 0x7E, //Physical 29
    }
}
