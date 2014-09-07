using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace xeniC.Embedded.Utils
{
    static class Endianess
    {
        public static UInt16 ToLittleEndian(UInt16 data)
        {
            if (BitConverter.IsLittleEndian)
                return data;

            return (UInt16)(((data & 0xFF00) >> 8) | ((data & 0x00FF) << 8));
        }

        public static UInt32 ToLittleEndian(UInt32 data)
        {
            if (BitConverter.IsLittleEndian)
                return data;

            return ((data & 0xFF000000) >> 24) |
                    ((data & 0x00FF0000) >> 8) |
                    ((data & 0x0000FF00) << 8) |
                    ((data & 0x000000FF) << 24);
        }

        public static UInt16 ToBigEndian(UInt16 data)
        {
            if (!BitConverter.IsLittleEndian)
                return data;

            return (UInt16)(((data & 0xFF00) >> 8) | ((data & 0x00FF) << 8));
        }

        public static UInt32 ToBigEndian(UInt32 data)
        {
            if (!BitConverter.IsLittleEndian)
                return data;

            return ((data & 0xFF000000) >> 24) |
                    ((data & 0x00FF0000) >> 8) |
                    ((data & 0x0000FF00) << 8) |
                    ((data & 0x000000FF) << 24);
        }
    }
}
