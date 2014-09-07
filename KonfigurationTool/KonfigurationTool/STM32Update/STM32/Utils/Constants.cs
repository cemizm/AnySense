using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace xeniC.Embedded.Utils.STM32
{
    static class Constants
    {
        public static readonly byte[] ResetCode = {
	0x01, 0x49,		// ldr     r1, [pc, #4] ; (<AIRCR_OFFSET>)
	0x02, 0x4A,		// ldr     r2, [pc, #8] ; (<AIRCR_RESET_VALUE>)
	0x0A, 0x60,		// str     r2, [r1, #0]
	0xfe, 0xe7,		// endless: b endless
	0x0c, 0xed, 0x00, 0xe0,	// .word 0xe000ed0c <AIRCR_OFFSET> = NVIC AIRCR register address
	0x04, 0x00, 0xfa, 0x05	// .word 0x05fa0004 <AIRCR_RESET_VALUE> = VECTKEY | SYSRESETREQ
};
    }
}
