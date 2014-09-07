using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace xeniC.Embedded.Utils.STM32
{
    enum Command : byte
    {
        /// <summary>
        /// Acknowledge a Command
        /// </summary>
        Ack = 0x79,
        /// <summary>
        /// Not acknowledge a Command
        /// </summary>
        NAck = 0x1F,
        /// <summary>
        /// Initalize
        /// </summary>
        Init = 0x7F,
        /// <summary>
        /// Get the version and command supported
        /// </summary>
        GetVersion = 0x00,
        /// <summary>
        /// Get version and read protection status
        /// </summary>
        GetVersionReadProtection = 0x01,
        /// <summary>
        /// Get ID
        /// </summary>
        GetID = 0x02,
        /// <summary>
        /// Read Memory
        /// </summary>
        ReadMemory = 0x11,
        /// <summary>
        /// Go
        /// </summary>
        Go = 0x21,
        /// <summary>
        /// Write memory
        /// </summary>
        WriteMemory = 0x31,
        /// <summary>
        /// Erase
        /// </summary>
        Erase = 0x43,
        /// <summary>
        /// Extended Erase
        /// </summary>
        ExtendedErase = 0x44,
        /// <summary>
        /// Write Protected
        /// </summary>
        WriteProtect = 0x63,
        /// <summary>
        /// Write Unprotected
        /// </summary>
        WriteUnprotect = 0x73,
        /// <summary>
        /// Read Protected
        /// </summary>
        ReadProtect = 0x82,
        /// <summary>
        /// Read Unprotected
        /// </summary>
        ReadUnprotect = 0x92,
        /// <summary>
        /// Error or unknown Command
        /// </summary>
        Error = 0xFF,
    }
}
