using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{
    internal enum StateMachineStep : sbyte
    {
        None = 0,
        Open = 1,
        Connected = 2,
        Updating = 3,
    }
}
