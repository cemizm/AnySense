using System;
using System.Collections;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Management;
using System.Text;

namespace KonfigurationTool
{
    public static class SerialPortExtensions
    {
        public static IList GetPortInformations(this SerialPort serial)
        {
            string[] portnames = SerialPort.GetPortNames();

            using (var searcher = new ManagementObjectSearcher("SELECT * FROM Win32_PnPEntity WHERE ConfigManagerUserConfig=0"))
            using (var erg = searcher.Get())
            {
                var ports = erg.Cast<ManagementBaseObject>().ToList();
                var tList = (from n in portnames
                             from p in ports
                             where p["Caption"].ToString().Contains(n)
                             select new KeyValuePair<string, string>(n, p != null ? p["Caption"].ToString() : n)).ToList();


                return tList;
            }
        }
    }
}
