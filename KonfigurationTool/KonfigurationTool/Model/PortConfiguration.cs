using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{
    class PortConfiguration
    {

        private ProtocolType protocol;

        public ProtocolType Protocol
        {
            get { return protocol; }
            set
            {
                if (protocol != value)
                {
                    switch (value)
                    {
                        case ProtocolType.FrSky_SPort:
                            Configuration = new ProtocolFrSkySPortConfiguration();
                            break;
                        case ProtocolType.None:
                            Configuration = null;
                            break;
                        case ProtocolType.Mavlink:
                            Configuration = new ProtocolMavlinkConfiguration();
                            break;
                        case ProtocolType.HOTT:
                            Configuration = new ProtocolHoTTConfiguration();
                            break;
                        case ProtocolType.Futaba:
                            Configuration = new ProtocolFutabaConfiguration();
                            break;
                        case ProtocolType.Jeti:
                            Configuration = new ProtocolJetiConfiguration();
                            break;
                        case ProtocolType.Spektrum:
                            if (Port != 1)
                                throw new NotSupportedException("Spektrum is not availible on this Port");

                            Configuration = new ProtocolSpektrumConfiguration();
                            break;
                        default:
                            throw new NotImplementedException(string.Format("Protocol {0} not implemented yet.", value));
                    }

                    protocol = value;

                }
            }
        }

        public ProtocolConfiguration Configuration { get; set; }

        public byte Port { get; set; }
    }
}
