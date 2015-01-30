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
            cmbProtocol.DataSource = typeof(ProtocolType).GetDataSource();
            cmbProtocol.DisplayMember = "Value";
            cmbProtocol.ValueMember = "Key";
        }

        public PortConfiguration Configuration
        {
            get { return configuration; }
            set
            {
                configuration = value;
                cmbProtocol.SelectedValue = configuration.Protocol;
                protocolConfiguration.SelectedObject = configuration.Configuration;
                //protocolConfiguration.ExpandAllGridItems();
            }
        }

        private void cmbProtocol_SelectedValueChanged(object sender, EventArgs e)
        {
            if (configuration != null && configuration.Protocol != (ProtocolType)cmbProtocol.SelectedValue)
            {
                try
                {
                    configuration.Protocol = (ProtocolType)cmbProtocol.SelectedValue;
                    protocolConfiguration.SelectedObject = configuration.Configuration;
                    //protocolConfiguration.ExpandAllGridItems();
                }
                catch (NotImplementedException ex)
                {
                    cmbProtocol.SelectedValue = configuration.Protocol;
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
