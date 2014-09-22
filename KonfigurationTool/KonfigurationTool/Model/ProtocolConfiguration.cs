using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{
    internal abstract class ProtocolConfiguration
    {
        private MavLink.FrameworkBitConverter converter;

        public ProtocolConfiguration()
        {
            converter = new MavLink.FrameworkBitConverter();
            converter.SetDataIsLittleEndian(MavLink.MavlinkSettings.IsLittleEndian);
        }

        protected MavLink.FrameworkBitConverter Converter { get { return converter; } }

        public abstract void DeSerialize(byte[] data);
        public abstract byte[] Serialize();

    }
}
