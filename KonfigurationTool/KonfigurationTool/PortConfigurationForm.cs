using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace KonfigurationTool
{
    internal partial class PortConfigurationForm : Form
    {
        private PortConfiguration configuration;

        public PortConfigurationForm()
        {
            InitializeComponent();
            cmbProtocol.DataSource = Enum.GetValues(typeof(ProtocolType));
        }

        public PortConfiguration Configuration
        {
            get { return configuration; }
            set
            {
                configuration = value;
                cmbProtocol.SelectedItem = configuration.Protocol;
                protocolConfiguration.SelectedObject = configuration.Configuration;
            }
        }

        private void cmbProtocol_SelectedValueChanged(object sender, EventArgs e)
        {
            if (configuration != null && configuration.Protocol != (ProtocolType)cmbProtocol.SelectedItem)
            {
                try
                {
                    configuration.Protocol = (ProtocolType)cmbProtocol.SelectedItem;
                    protocolConfiguration.SelectedObject = configuration.Configuration;
                }
                catch (NotImplementedException ex)
                {
                    cmbProtocol.SelectedItem = configuration.Protocol;
                    MessageBox.Show(this, ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }

        }

        public static DialogResult ShowDialog(IWin32Window parent, PortConfiguration config)
        {
            using (PortConfigurationForm frm = new PortConfigurationForm())
            {
                frm.Configuration = config;
                return frm.ShowDialog();
            }
        }
    }
}
