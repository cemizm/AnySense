using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;

namespace xeniC.Embedded.Utils.STM32
{
    class BootloaderController : IDisposable
    {
        private SerialPort port;

        public BootloaderController(string portName)
        {
            port = new SerialPort(portName);
            port.RtsEnable = false;
            port.BaudRate = 57600;
            port.DataBits = 8;
            port.DtrEnable = false;
            port.Parity = Parity.Even;
            port.RtsEnable = false;
            port.StopBits = StopBits.One;
            port.ReadTimeout = 1500;
        }

        #region Properties

        public Device Device { get; private set; }

        public byte Version { get; private set; }

        public byte Option1 { get; private set; }

        public byte Option2 { get; private set; }

        public byte BootloaderVersion { get; private set; }

        public ReadOnlyCollection<Command> SupportedCommands { get; private set; }

        #endregion

        #region Public Methods

        public void Connect()
        {
            port.Open();

            port.Write(new byte[] { ((byte)Command.Init) }, 0, 1);
            Command res = (Command)port.ReadByte();

            if (res != Command.Ack)
                throw new InvalidOperationException("Failed to get init ACK from device!");

            if (!SendCommand(Command.GetVersion))
                throw new InvalidOperationException("Failed to get version and supported Commands from device!");

            List<Command> cmds = new List<Command>();

            byte length = (byte)(port.ReadByte() + 1);

            BootloaderVersion = (byte)port.ReadByte(); --length;

            while (length-- > 0)
            {
                cmds.Add((Command)port.ReadByte());
            }

            res = (Command)port.ReadByte();
            if (res != Command.Ack)
                throw new InvalidOperationException("Protocol error, Bootloader did not returned correct!");

            if (!cmds.Contains(Command.GetVersion) ||
                !cmds.Contains(Command.GetID) ||
                !cmds.Contains(Command.GetVersionReadProtection))
                throw new InvalidOperationException("Protocol error, Bootloader did not returned correct!");

            SupportedCommands = cmds.AsReadOnly();


            if (!SendCommand(Command.GetVersionReadProtection))
                throw new InvalidOperationException("Could not read protection status!");


            Version = (byte)port.ReadByte();
            Option1 = (byte)port.ReadByte();
            Option2 = (byte)port.ReadByte();

            res = (Command)port.ReadByte();
            if (res != Command.Ack)
                throw new InvalidOperationException("Protocol error, could not read protection status ACK!");

            if (!SendCommand(Command.GetID))
                throw new InvalidOperationException("Could not read ID command");

            length = (byte)(port.ReadByte() + 1);

            if (length < 2)
                throw new InvalidOperationException(string.Format("Only {0} bytes sent in the PID, unknown/unsupported device", length));

            short tmpId = (short)(((byte)port.ReadByte() << 8) | (byte)port.ReadByte());
            length -= 2;
            if (length > 0)
            {
                while (length-- > 0)
                    port.ReadByte();
            }

            res = (Command)port.ReadByte();
            if (res != Command.Ack)
                throw new InvalidOperationException("Protocol error, could not read Id ACK!");

            if (!Device.Devices.ContainsKey(tmpId))
                throw new InvalidOperationException(string.Format("Unknown/unsupported device (Device ID: 0x{0:X4})\n", tmpId));

            Device = Device.Devices[tmpId];
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="path"></param>
        /// <param name="progress"></param>
        public void WriteStream(Stream s, Action<int> progress)
        {
            Erase();
            WriteStreamToMemory(Device.Flash.Start, s, progress);
        }

        /// <summary>
        /// Reads <paramref name="length"/> of byte starting at <paramref name="address"/>.
        /// </summary>
        /// <param name="address">Address to start reading</param>
        /// <param name="length">Number of bytes to read.</param>
        /// <returns>bytes readed</returns>
        public byte[] ReadMemory(UInt32 address, UInt16 length)
        {
            if (length < 1 || length > 256)
                throw new ArgumentOutOfRangeException("length");

            if (address % 4 != 0)
                throw new ArgumentException("must be 32bit aligned", "address");

            if (!SupportedCommands.Contains(Command.ReadMemory))
                throw new InvalidOperationException("Read command not implemented in bootloader.\n");

            if (!SendCommand(Command.ReadMemory))
                throw new InvalidOperationException("Could not start ReadMemory Command");

            byte i = (byte)(length - 1);

            port.Write(new byte[] { i, (byte)(i ^ 0xFF) }, 0, 2);

            if ((Command)port.ReadByte() != Command.Ack)
                throw new InvalidOperationException("Invalid length of bytes reported");

            byte[] buffer = new byte[length];
            port.Read(buffer, 0, length);

            return buffer;
        }

        public void WriteMemory(UInt32 address, byte[] data, UInt16 length)
        {
            if (!SupportedCommands.Contains(Command.WriteMemory))
                throw new InvalidOperationException("Write command not implemented in bootloader.");

            if (!SendCommand(Command.WriteMemory))
                throw new InvalidOperationException("Error write command ack");

            SendAdress(address);

            byte extra = (byte)(length % 4);
            byte cs = (byte)(length - 1 + extra);

            port.Write(new byte[] { cs }, 0, 1);

            for (int i = 0; i < length; i++)
                cs ^= data[i];

            port.Write(data, 0, length);

            /* write the alignment padding */
            for (byte c = 0; c < extra; ++c)
            {
                port.Write(new byte[] { 0xFF }, 0, 1);
                cs ^= 0xFF;
            }

            /* send the checksum */
            port.Write(new byte[] { cs }, 0, 1);

            if ((Command)(byte)port.ReadByte() != Command.Ack)
                throw new InvalidOperationException("Write command error!");
        }

        public void WriteUnprotected()
        {

            if (!SupportedCommands.Contains(Command.WriteUnprotect))
                throw new InvalidOperationException("Write unportect command not implemented in bootloader!");

            if (!SendCommand(Command.WriteUnprotect))
                throw new InvalidOperationException("Write unprotect command error!");

            if ((Command)port.ReadByte() != Command.Ack)
                throw new InvalidOperationException("Write unprotect command error!");
        }

        public void ReadUnportected()
        {

            if (!SupportedCommands.Contains(Command.ReadUnprotect))
                throw new InvalidOperationException("Read unportect command not implemented in bootloader!");

            if (!SendCommand(Command.ReadUnprotect))
                throw new InvalidOperationException("Read unprotect command error!");

            if ((Command)port.ReadByte() != Command.Ack)
                throw new InvalidOperationException("Read unprotect command error!");

        }

        public void ReadProtected()
        {

            if (!SupportedCommands.Contains(Command.ReadUnprotect))
                throw new InvalidOperationException("Read portect command not implemented in bootloader!");

            if (!SendCommand(Command.ReadUnprotect))
                throw new InvalidOperationException("Read protect command error!");

            if ((Command)port.ReadByte() != Command.Ack)
                throw new InvalidOperationException("Read protect command error!");
        }

        /// <summary>
        /// Erase full chip
        /// </summary>
        public void Erase()
        {
            Erase(0, 0xFF);
        }

        /// <summary>
        /// Erases from <paramref name="spage"/> count of <paramref name="pages"/>
        /// </summary>
        /// <param name="spage">Start erasing at page</param>
        /// <param name="pages">Erase count</param>
        public void Erase(byte spage, byte pages)
        {
            Command eraseCmd = Command.Erase;
            if (pages == 0)
                return;

            if (!SupportedCommands.Contains(Command.Erase) &&
                !SupportedCommands.Contains(Command.ExtendedErase))
                throw new InvalidOperationException("Erase command not implemented in bootloader.");

            if (SupportedCommands.Contains(Command.ExtendedErase))
                eraseCmd = Command.ExtendedErase;

            if (!SendCommand(eraseCmd))
                throw new InvalidOperationException("Can't initiate chip erase!");

            /* The erase command reported by the bootloader is either 0x43 or 0x44 */
            /* 0x44 is Extended Erase, a 2 byte based protocol and needs to be handled differently. */
            if (eraseCmd == Command.ExtendedErase)
            {
                /* Not all chips using Extended Erase support mass erase */
                /* Currently known as not supporting mass erase is the Ultra Low Power STM32L15xx range */
                /* So if someone has not overridden the default, but uses one of these chips, take it out of */
                /* mass erase mode, so it will be done page by page. This maximum might not be correct either! */
                if (Device.Id == 0x416 && pages == 0xFF)/* works for the STM32L152RB with 128Kb flash */
                    pages = 0xF8;


                if (pages == 0xFF)
                {
                    port.Write(new byte[] { 0xFF, 0xFF, 0x00 }, 0, 3);

                    if ((Command)port.ReadByte() != Command.Ack)
                        throw new InvalidOperationException("Mass erase failed. Try specifying the number of pages to be erased.");

                    return;
                }

                UInt16 pg_num;
                byte pg_byte;
                byte cs = 0;

                pg_byte = (byte)((pages - 1) >> 8);
                port.Write(new byte[] { pg_byte }, 0, 1);

                cs ^= pg_byte;
                pg_byte = (byte)((pages - 1) & 0xFF);
                port.Write(new byte[] { pg_byte }, 0, 1);
                cs ^= pg_byte;

                for (pg_num = spage; pg_num < spage + pages; pg_num++)
                {
                    pg_byte = (byte)(pg_num >> 8);
                    cs ^= pg_byte;
                    port.Write(new byte[] { pg_byte }, 0, 1);
                    pg_byte = (byte)(pg_num & 0xFF);
                    cs ^= pg_byte;
                    port.Write(new byte[] { pg_byte }, 0, 1);
                }
                port.Write(new byte[] { cs }, 0, 1);

                if ((Command)port.ReadByte() != Command.Ack)
                    throw new InvalidOperationException("Page-by-page erase failed. Check the maximum pages your device supports.\n");
            }

            /* And now the regular erase (0x43) for all other chips */
            if (pages == 0xFF)
            {
                if (!SendCommand(0xFF))
                    throw new InvalidOperationException("Mass erase failed.");
            }
            else
            {
                byte cs = 0;
                byte pg_num;
                port.Write(new byte[] { (byte)(pages - 1) }, 0, 1);

                cs ^= (byte)(pages - 1);
                for (pg_num = spage; pg_num < (pages + spage); pg_num++)
                {
                    port.Write(new byte[] { pg_num }, 0, 1);
                    cs ^= pg_num;
                }
                port.Write(new byte[] { cs }, 0, 1);


                if ((Command)port.ReadByte() != Command.Ack)
                    throw new InvalidOperationException("Page-by-page erase failed. Check the maximum pages your device supports.\n");
            }
        }

        /// <summary>
        /// Start Execution of <paramref name="address"/>.
        /// </summary>
        /// <param name="address">Address for starting execution.</param>
        public void Go(UInt32 address)
        {
            if (!SupportedCommands.Contains(Command.Go))
                throw new InvalidOperationException("GO command not implemented in bootloader.");

            if (!SendCommand(Command.Go))
                throw new InvalidOperationException("G0 command ACK error!");

            SendAdress(address);
        }

        /// <summary>
        /// Reset the Device.
        /// </summary>
        public void ResetDevice()
        {
            RunRawCommand(Device.RAM.Start, Constants.ResetCode);
        }

        public void Dispose()
        {
            try
            {
                if (port != null && port.IsOpen)
                    port.Close();
            }
            catch { }
        }

        #endregion

        #region Private Methods

        private void RunRawCommand(UInt32 target_address, byte[] code)
        {
            UInt32 stack_le = Endianess.ToLittleEndian(0x20002000);
            UInt32 code_address_le = Endianess.ToLittleEndian(target_address + 8);
            UInt32 length = (UInt32)code.Length + 8;

            using (MemoryStream ms = new MemoryStream())
            {
                ms.Write(BitConverter.GetBytes(stack_le), 0, sizeof(UInt32));
                ms.Write(BitConverter.GetBytes(code_address_le), 0, sizeof(UInt32));
                ms.Write(code, 0, code.Length);

                ms.Seek(0, SeekOrigin.Begin);

                WriteStreamToMemory(target_address, ms, null);
            }

            Go(target_address);
        }

        private void WriteStreamToMemory(UInt32 address, Stream s, Action<int> report)
        {
            if (report != null)
                report(0);

            byte[] buffer = new byte[256];
            UInt16 read = 0;
            UInt32 totalSend = 0;

            do
            {
                read = (UInt16)s.Read(buffer, 0, buffer.Length);

                if (read > 0)
                {
                    WriteMemory(address, buffer, read);
                    address += read;
                    totalSend += read;

                    if (report != null)
                        report((int)((100.0f / s.Length) * totalSend));
                }

            } while (read > 0);
        }

        private bool SendCommand(Command cmd)
        {
            return SendCommand((byte)cmd);
        }

        private bool SendCommand(byte data)
        {
            port.Write(new byte[] { data, (byte)(data ^ 0xFF) }, 0, 2);

            Command response = (Command)port.ReadByte();

            return response == Command.Ack;

        }

        #endregion

        #region Helpers

        private byte GenerateChecksum(UInt32 data)
        {
            return (byte)(((data & 0xFF000000) >> 24) ^
                ((data & 0x00FF0000) >> 16) ^
                ((data & 0x0000FF00) >> 8) ^
                ((data & 0x000000FF) >> 0));
        }

        private void SendAdress(UInt32 address)
        {
            if (address % 4 != 0)
                throw new ArgumentException("Must be 32-bit aligned", "address");

            address = Endianess.ToBigEndian(address);
            byte cs = GenerateChecksum(address);

            port.Write(new byte[] { (byte)((address & 0x000000FF)),       // 1. Byte
                                    (byte)((address & 0x0000FF00) >> 8),  // 2. Byte
                                    (byte)((address & 0x00FF0000) >> 16), // 3. Byte
                                    (byte)((address & 0xFF000000) >> 24), // 4. Byte
                                    cs },                                 // 5. Byte
                                    0, 5);

            if ((Command)port.ReadByte() != Command.Ack)
                throw new InvalidOperationException("Invalid Address reported");
        }

        #endregion

    }
}
