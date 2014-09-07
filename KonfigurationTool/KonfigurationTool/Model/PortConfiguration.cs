﻿using System;
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
                            Configuration = null;
                            break;
                        case ProtocolType.HOTT:
                        case ProtocolType.Jeti:
                        default:
                            throw new NotImplementedException(string.Format("Protocol {0} not implemented yet.", value));
                    }

                    protocol = value;

                }
            }
        }



        public ProtocolConfiguration Configuration { get; set; }

    }
}