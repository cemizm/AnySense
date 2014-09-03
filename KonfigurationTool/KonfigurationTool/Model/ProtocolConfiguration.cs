using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{
    internal abstract class ProtocolConfiguration
    {
        public ProtocolConfiguration()
        { 
        }

        public abstract void DeSerialize(byte[] data);
        public abstract byte[] Serialize();

    }
}
