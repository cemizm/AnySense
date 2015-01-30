using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace KonfigurationTool
{

    [TypeConverter(typeof(EnumDescriptionTypeConverter))]
    enum FixType
    {
        [Description("No Fix")]
        None = 0,
        [Description("2D GPS Fix")]
        FIX_2D = 2,
        [Description("3D GPS Fix")]
        FIX_3D = 3,
        [Description("DGPS Fix")]
        FIX_DGPS = 4,
    }
}
