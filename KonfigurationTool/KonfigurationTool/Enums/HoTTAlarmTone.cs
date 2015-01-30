using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{
    [TypeConverter(typeof(EnumDescriptionTypeConverter))]
    enum HoTTAlarmTone : byte
    {
        [Description("No tone")]
        None = 0x00,
        [Description("Tone A - min. Speed")]
        Tone_A = 0x01,
        [Description("Tone B - Vario -3s")]
        Tone_B = 0x02,
        [Description("Tone C - Vario -1s")]
        Tone_C = 0x03,
        [Description("Tone D - max. Distance")]
        Tone_D = 0x04,
        [Description("Tone E - Blink Blink")]
        Tone_E = 0x05,
        [Description("Tone F - min. Temp. S1")]
        Tone_F = 0x06,
        [Description("Tone G - min. Temp. S2")]
        Tone_G = 0x07,
        [Description("Tone H - max. Temp. S1")]
        Tone_H = 0x08,
        [Description("Tone I - max. Temp. S2")]
        Tone_I = 0x09,
        [Description("Tone J - max. Voltage S1")]
        Tone_J = 0x0A,
        [Description("Tone K - max. Voltage S2")]
        Tone_K = 0x0B,
        [Description("Tone L - max. Speed")]
        Tone_L = 0x0C,
        [Description("Tone M - Vario +3s")]
        Tone_M = 0x0D,
        [Description("Tone N - Vario +1s")]
        Tone_N = 0x0E,
        [Description("Tone O - min. Altitude")]
        Tone_O = 0x0F,
        [Description("Tone P - min. Power Voltage")]
        Tone_P = 0x10,
        [Description("Tone Q - min. Cell Voltage")]
        Tone_Q = 0x11,
        [Description("Tone R - min. S1 Voltage")]
        Tone_R = 0x12,
        [Description("Tone S - min. S2 Voltage")]
        Tone_S = 0x13,
        [Description("Tone T - min. RPM")]
        Tone_T = 0x14,
        [Description("Tone U - min. Fuel")]
        Tone_U = 0x15,
        [Description("Tone V - Capacity")]
        Tone_V = 0x16,
        [Description("Tone W - max. Current")]
        Tone_W = 0x17,
        [Description("Tone X - max. Power Voltage")]
        Tone_X = 0x18,
        [Description("Tone Y - max. RPM")]
        Tone_Y = 0x19,
        [Description("Tone Z - max. Altitude")]
        Tone_Z = 0x1A  	//Max. Altitude Z
    }
}
