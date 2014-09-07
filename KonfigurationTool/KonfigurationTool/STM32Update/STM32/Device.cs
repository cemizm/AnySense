using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace xeniC.Embedded.Utils.STM32
{

    struct MemoryArea
    {
        public MemoryArea(UInt32 start, UInt32 end)
            : this()
        {
            this.Start = start;
            this.End = end;
        }

        public UInt32 Start { get; private set; }
        public UInt32 End { get; private set; }
    }

    /// <summary>
    /// 
    /// </summary>
    struct Device
    {
        public Device(short id, string name, MemoryArea ram, MemoryArea flash, short sectors, short size, MemoryArea options, MemoryArea system)
            : this()
        {
            this.Id = id;
            this.Name = name;
            this.RAM = ram;
            this.Flash = flash;
            this.Sectors = sectors;
            this.Size = size;
            this.Options = options;
            this.System = system;
        }

        /// <summary>
        /// Id of device
        /// </summary>
        public short Id { get; private set; }
        /// <summary>
        /// Name of device
        /// </summary>
        public string Name { get; private set; }
        /// <summary>
        /// RAM memory area
        /// </summary>
        public MemoryArea RAM { get; private set; }
        /// <summary>
        /// Flash memory area
        /// </summary>
        public MemoryArea Flash { get; private set; }
        /// <summary>
        /// Pages per sector
        /// </summary>
        public short Sectors { get; private set; }
        /// <summary>
        /// Page Size
        /// </summary>
        public short Size { get; private set; }
        /// <summary>
        /// Option byte area
        /// </summary>
        public MemoryArea Options { get; private set; }
        /// <summary>
        /// System memory area
        /// </summary>
        public MemoryArea System { get; private set; }

        public static readonly Dictionary<short, Device> Devices = new Dictionary<short, Device>() { 
        { 0x440, new Device(0x440, "STM32F051xx"       ,new MemoryArea(0x20001000, 0x20002000), new MemoryArea(0x08000000, 0x08010000),  4, 1024,  new MemoryArea(0x1FFFF800, 0x1FFFF80B), new MemoryArea(0x1FFFEC00, 0x1FFFF800)) }, 
        { 0x444, new Device(0x444, "STM32F030/F031"    ,new MemoryArea(0x20001000, 0x20002000), new MemoryArea(0x08000000, 0x08010000),  4, 1024,  new MemoryArea(0x1FFFF800, 0x1FFFF80B), new MemoryArea(0x1FFFEC00, 0x1FFFF800)) }, 
        { 0x448, new Device(0x448, "STM32F072xx"       ,new MemoryArea(0x20001800, 0x20004000), new MemoryArea(0x08000000, 0x08020000),  4, 1024,  new MemoryArea(0x1FFFF800, 0x1FFFF80B), new MemoryArea(0x1FFFEC00, 0x1FFFF800)) }, 
        { 0x412, new Device(0x412, "Low-density"       ,new MemoryArea(0x20000200, 0x20002800), new MemoryArea(0x08000000, 0x08008000),  4, 1024,  new MemoryArea(0x1FFFF800, 0x1FFFF80F), new MemoryArea(0x1FFFF000, 0x1FFFF800))},
        { 0x410, new Device(0x410, "Medium-density"    ,new MemoryArea(0x20000200, 0x20005000), new MemoryArea(0x08000000, 0x08020000),  4, 1024,  new MemoryArea(0x1FFFF800, 0x1FFFF80F), new MemoryArea(0x1FFFF000, 0x1FFFF800))},
        { 0x414, new Device(0x414, "High-density"      ,new MemoryArea(0x20000200, 0x20010000), new MemoryArea(0x08000000, 0x08080000),  2, 2048,  new MemoryArea(0x1FFFF800, 0x1FFFF80F), new MemoryArea(0x1FFFF000, 0x1FFFF800))},
	    { 0x420, new Device(0x420, "Medium-density VL" ,new MemoryArea(0x20000200, 0x20002000), new MemoryArea(0x08000000, 0x08020000),  4, 1024,  new MemoryArea(0x1FFFF800, 0x1FFFF80F), new MemoryArea(0x1FFFF000, 0x1FFFF800))},
	    { 0x428, new Device(0x428, "High-density VL"   ,new MemoryArea(0x20000200, 0x20008000), new MemoryArea(0x08000000, 0x08080000),  2, 2048,  new MemoryArea(0x1FFFF800, 0x1FFFF80F), new MemoryArea(0x1FFFF000, 0x1FFFF800))},
	    { 0x418, new Device(0x418, "Connectivity line" ,new MemoryArea(0x20001000, 0x20010000), new MemoryArea(0x08000000, 0x08040000),  2, 2048,  new MemoryArea(0x1FFFF800, 0x1FFFF80F), new MemoryArea(0x1FFFB000, 0x1FFFF800))},
	    { 0x430, new Device(0x430, "XL-density"        ,new MemoryArea(0x20000800, 0x20018000), new MemoryArea(0x08000000, 0x08100000),  2, 2048,  new MemoryArea(0x1FFFF800, 0x1FFFF80F), new MemoryArea(0x1FFFE000, 0x1FFFF800))},
	    { 0x411, new Device(0x411, "STM32F2xx"         ,new MemoryArea(0x20002000, 0x20020000), new MemoryArea(0x08000000, 0x08100000),  4, 16384, new MemoryArea(0x1FFFC000, 0x1FFFC00F), new MemoryArea(0x1FFF0000, 0x1FFF77DF))},
	    { 0x432, new Device(0x432, "STM32F373/8"       ,new MemoryArea(0x20001400, 0x20008000), new MemoryArea(0x08000000, 0x08040000),  2, 2048,  new MemoryArea(0x1FFFF800, 0x1FFFF80F), new MemoryArea(0x1FFFD800, 0x1FFFF800))},
	    { 0x422, new Device(0x422, "F302xB/303xB/358"  ,new MemoryArea(0x20001400, 0x20010000), new MemoryArea(0x08000000, 0x08040000),  2, 2048,  new MemoryArea(0x1FFFF800, 0x1FFFF80F), new MemoryArea(0x1FFFD800, 0x1FFFF800))},
	    { 0x439, new Device(0x439, "STM32F302"         ,new MemoryArea(0x20001800, 0x20004000), new MemoryArea(0x08000000, 0x08040000),  2, 2048,  new MemoryArea(0x1FFFF800, 0x1FFFF80F), new MemoryArea(0x1FFFD800, 0x1FFFF800))},
	    { 0x438, new Device(0x438, "F303x4/334/328"    ,new MemoryArea(0x20001800, 0x20003000), new MemoryArea(0x08000000, 0x08040000),  2, 2048,  new MemoryArea(0x1FFFF800, 0x1FFFF80F), new MemoryArea(0x1FFFD800, 0x1FFFF800))},
	    { 0x413, new Device(0x413, "STM32F40/1"        ,new MemoryArea(0x20002000, 0x20020000), new MemoryArea(0x08000000, 0x08100000),  4, 16384, new MemoryArea(0x1FFFC000, 0x1FFFC00F), new MemoryArea(0x1FFF0000, 0x1FFF77DF))},
	    { 0x419, new Device(0x419, "STM32F427/37"      ,new MemoryArea(0x20002000, 0x20030000), new MemoryArea(0x08000000, 0x08100000),  4, 16384, new MemoryArea(0x1FFFC000, 0x1FFFC00F), new MemoryArea(0x1FFF0000, 0x1FFF77FF))},
	    { 0x423, new Device(0x423, "STM32F401xB(C)"    ,new MemoryArea(0x20003000, 0x20010000), new MemoryArea(0x08000000, 0x08100000),  4, 16384, new MemoryArea(0x1FFFC000, 0x1FFFC00F), new MemoryArea(0x1FFF0000, 0x1FFF77FF))},
	    { 0x433, new Device(0x433, "STM32F401xD(E)"    ,new MemoryArea(0x20003000, 0x20018000), new MemoryArea(0x08000000, 0x08100000),  4, 16384, new MemoryArea(0x1FFFC000, 0x1FFFC00F), new MemoryArea(0x1FFF0000, 0x1FFF77FF))},
	    { 0x417, new Device(0x417, "L05xxx/06xxx"      ,new MemoryArea(0x20001000, 0x20002000), new MemoryArea(0x08000000, 0x08020000), 16,  256,  new MemoryArea(0x1FF80000, 0x1FF8000F), new MemoryArea(0x1FF00000, 0x1FF01000))},
	    { 0x416, new Device(0x416, "L1xxx6(8/B)"       ,new MemoryArea(0x20000800, 0x20004000), new MemoryArea(0x08000000, 0x08020000), 16,  256,  new MemoryArea(0x1FF80000, 0x1FF8000F), new MemoryArea(0x1FF00000, 0x1FF01000))},
	    { 0x429, new Device(0x429, "L1xxx6(8/B)A"      ,new MemoryArea(0x20001000, 0x20008000), new MemoryArea(0x08000000, 0x08020000), 16,  256,  new MemoryArea(0x1FF80000, 0x1FF8000F), new MemoryArea(0x1FF00000, 0x1FF01000))},
	    { 0x427, new Device(0x427, "L1xxxC"            ,new MemoryArea(0x20001000, 0x20008000), new MemoryArea(0x08000000, 0x08020000), 16,  256,  new MemoryArea(0x1FF80000, 0x1FF8000F), new MemoryArea(0x1FF00000, 0x1FF02000))},
	    { 0x436, new Device(0x436, "L1xxxD"            ,new MemoryArea(0x20001000, 0x2000C000), new MemoryArea(0x08000000, 0x08060000), 16,  256,  new MemoryArea(0x1ff80000, 0x1ff8000F), new MemoryArea(0x1FF00000, 0x1FF02000))},
	    { 0x437, new Device(0x437, "L1xxxE"            ,new MemoryArea(0x20001000, 0x20014000), new MemoryArea(0x08000000, 0x08060000), 16,  256,  new MemoryArea(0x1ff80000, 0x1ff8000F), new MemoryArea(0x1FF00000, 0x1FF02000))},
	    { 0x641, new Device(0x641, "Medium_Density PL" ,new MemoryArea(0x20000200, 0x00005000), new MemoryArea(0x08000000, 0x08020000),  4, 1024,  new MemoryArea(0x1FFFF800, 0x1FFFF80F), new MemoryArea(0x1FFFF000, 0x1FFFF800))},
	    { 0x9a8, new Device(0x9a8, "STM32W-128K"       ,new MemoryArea(0x20000200, 0x20002000), new MemoryArea(0x08000000, 0x08020000),  1, 1024,  new MemoryArea(0, 0), new MemoryArea(0, 0))},
	    { 0x9b0, new Device(0x9b0, "STM32W-256K"       ,new MemoryArea(0x20000200, 0x20004000), new MemoryArea(0x08000000, 0x08040000),  1, 2048,  new MemoryArea(0, 0), new MemoryArea(0, 0))},
        };
    }
}
