using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using xeniC.Embedded.Utils.STM32;

namespace KonfigurationTool
{
    public partial class UpdateForm : Form
    {
        BootloaderController controller;

        private string fileName;

        public UpdateForm(string portName, string fileName)
        {
            InitializeComponent();

            controller = new BootloaderController(portName);
            lblStatus.Text = "Initializing Update Controller...";

            this.fileName = fileName;
        }

        protected override void OnShown(EventArgs e)
        {
            base.OnShown(e);

            UpdateStatus("Connecting to device...", 0);
            Task.Factory.StartNew(() =>
            {
                try
                {
                    controller.Connect();
                    UpdateStatus("Connected to Device...", 0);

                    Stream s;

                    if (!string.IsNullOrEmpty(fileName))
                    {
                        s = new FileStream(fileName, FileMode.Open, FileAccess.Read);
                    }
                    else
                    {
                        s = Assembly.GetAssembly(GetType()).GetManifestResourceStream(@"KonfigurationTool.Firmware.UNCTelemetrie.bin");
                    }

                    using (s)
                        controller.WriteStream(s, (int percent) => { UpdateStatus("Flashing firmware...", percent); });


                    controller.ResetDevice();

                    Finish();
                }
                catch (Exception ex)
                {
                    ReportError(string.Format("Error while Updating\n{0}", ex.Message));
                }
            });

        }

        private void ReportError(string text)
        {
            if (InvokeRequired)
                BeginInvoke((Action<string>)ReportError, text);
            else
            {
                MessageBox.Show(this, text, "Error", MessageBoxButtons.OK);
                DialogResult = DialogResult.Cancel;
            }
        }

        private void UpdateStatus(string status, int percent)
        {
            if (InvokeRequired)
                BeginInvoke((Action<string, int>)UpdateStatus, status, percent);
            else
            {
                lblStatus.Text = status;
                progressBar.Value = percent;
            }
        }

        private void Finish()
        {
            if (InvokeRequired)
                BeginInvoke((Action)Finish);
            else
                DialogResult = DialogResult.OK;
        }

        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);
            controller.Dispose();
        }

        public static DialogResult ShowDialog(IWin32Window parent, string portName, string fileName)
        {
            using (UpdateForm dlg = new UpdateForm(portName, fileName))
            {
                return dlg.ShowDialog(parent);
            }
        }
    }
}
