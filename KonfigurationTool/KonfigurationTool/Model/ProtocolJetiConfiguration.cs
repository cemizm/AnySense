using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{
    class ProtocolJetiConfiguration : ProtocolConfiguration
    {


        public ProtocolJetiConfiguration()
            : base()
        {
            Version = 1;
        }

        #region "Properties"

        [Category("1. General")]
        [DisplayName("Config Version")]
        [Description("Version of Configuration for backward compability")]
        public byte Version { get; private set; }

        #endregion

        public override void DeSerialize(byte[] data)
        {
            Version = data[0];
        }

        public override byte[] Serialize()
        {
            byte[] data = new byte[128];

            data[0] = Version;

            return data;
        }

    }
}
