using MavLink;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Management;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace KonfigurationTool
{
    public partial class MainForm : Form
    {
        private const int MAX_RETRY = 100;

        private const int MAV_SYSTEM_ID = 0xCE;
        private const uint FIRMWARE_VERSION = 0x00010001;

        private int retry;
        private StateMachineStep currentStep = StateMachineStep.None;
        private Mavlink mavlink;
        private MavlinkPacket mavlink_packet;

        private Task UAHeartbeat = null;
        private Task FCHeartbeat = null;

        private ManagementEventWatcher deviceChanged;

        public MainForm()
        {

            InitializeComponent();

            StateMachineUpdate(StateMachineStep.None);

            mavlink_packet = new MavlinkPacket();
            mavlink_packet.ComponentId = (int)MAV_COMPONENT.MAV_COMP_ID_SYSTEM_CONTROL;
            mavlink_packet.SystemId = 0xCE;

            mavlink = new Mavlink();
            mavlink.PacketReceived += mavlink_PacketReceived;


            byte[] ver = BitConverter.GetBytes(FIRMWARE_VERSION);

            Text = string.Format("AnySense {0}.{1}.{2} - Konfiguration Manager", ver[2], ver[1], ver[0]);
        }

        #region UI Handling

        protected override void OnShown(EventArgs e)
        {
            base.OnShown(e);

            MonitorDeviceChanges();
            UpdateDeviceList();
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            if (!serialPort.IsOpen)
                Connect();
            else
                Disconnect(true);
        }

        private void cmbPort_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (serialPort.IsOpen)
                return;

            if (!string.IsNullOrEmpty(cmbPort.SelectedValue.ToString()))
                serialPort.PortName = cmbPort.SelectedValue.ToString();
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (serialPort.IsOpen)
            {
                Msg_configuration_control msg = new Msg_configuration_control();
                msg.command = (byte)CONFIG_COMMAND.CONFIG_COMMAND_EXIT;
                mavlink_packet.Message = msg;
                byte[] bytes = mavlink.Send(mavlink_packet);

                try
                {
                    serialPort.Write(bytes, 0, bytes.Length);
                    serialPort.Close();

                    deviceChanged.Stop();
                }
                catch { }
            }
        }

        private void MainForm_KeyUp(object sender, KeyEventArgs e)
        {
            if (currentStep == StateMachineStep.Connected && e.KeyCode == Keys.U)
                btnUpdate.Visible = true;

            if (currentStep == StateMachineStep.None && e.KeyCode == Keys.L)
            {
                try
                {
                    serialPort.Open();
                    StateMachineUpdate(StateMachineStep.Connected);

                }
                catch (Exception ex)
                {
                    MessageBox.Show(this, "Error while open serial port:\n" + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else if (currentStep == StateMachineStep.None && e.KeyCode == Keys.F)
            {
                try
                {
                    Enabled = false;
                    if (UpdateForm.ShowDialog(this, serialPort.PortName, String.Empty) == DialogResult.OK)
                        Connect();

                }
                catch (Exception ex)
                {
                    MessageBox.Show(this, "Error while communication:\n" + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }

                Enabled = true;

            }
        }

        private void linkUrl_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            try
            {
                System.Diagnostics.Process.Start("http://anysense.de/support/");
            }
            catch (Exception ex)
            {
            }
        }

        private void btnPort1Configure_Click(object sender, EventArgs e)
        {
            ConfigurePort(CONFIG_PORT.CONFIG_PORT_1);
        }

        private void btnPort2Configure_Click(object sender, EventArgs e)
        {
            ConfigurePort(CONFIG_PORT.CONFIG_PORT_2);
        }

        #endregion

        #region State Machine

        private void timer_Tick(object sender, EventArgs e)
        {

            if (retry > MAX_RETRY)
            {
                try
                {
                    serialPort.Close();
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error while closing serial port:\n" + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);

                }
                StateMachineUpdate(StateMachineStep.None);

                MessageBox.Show(this, string.Format("No AnySense found on {0}\nMaybe wrong Port?", serialPort.PortName), "Connection failed", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
            else
            {
                retry++;

                try
                {
                    Msg_configuration_control msg = new Msg_configuration_control();
                    msg.command = (byte)CONFIG_COMMAND.CONFIG_COMMAND_GET_VERSION;
                    mavlink_packet.Message = msg;
                    byte[] bytes = mavlink.Send(mavlink_packet);
                    serialPort.Write(bytes, 0, bytes.Length);
                }
                catch (Exception ex)
                {
                    StateMachineUpdate(StateMachineStep.None);
                    MessageBox.Show(this, "Error while communicating:\n" + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void StateMachineUpdate(StateMachineStep step)
        {
            switch (step)
            {
                case StateMachineStep.None:
                    btnConnect.Text = "Open";
                    btnUpdate.Visible = false;
                    timer.Enabled = false;
                    retry = 0;
                    tsStatus.Text = "disconnected";
                    tsStatus.ForeColor = Color.FromArgb(204, 11, 16);
                    tsFWVersion.Text = "-";
                    tsFWVersion.Visible = false;
                    tsLoading.Visible = false;
                    Cursor = Cursors.Default;
                    tsFCHearbeat.Visible = false;
                    tsUniAdapterHeartbeat.Visible = false;
                    ClearLiveData();
                    btnPort1Configure.Enabled = false;
                    btnPort2Configure.Enabled = false;
                    cmbPort.Enabled = true;
                    pbCells.Visible = false;
                    lblUpdateHint.Visible = false;
                    break;
                case StateMachineStep.Open:
                    btnConnect.Text = "Close";
                    btnUpdate.Visible = false;
                    retry = 0;
                    timer.Enabled = true;
                    tsStatus.Text = "connecting";
                    tsStatus.ForeColor = Color.Orange;
                    Cursor = Cursors.WaitCursor;
                    tsFWVersion.Text = "-";
                    tsFWVersion.Visible = false;
                    tsLoading.Visible = true;
                    ClearLiveData();
                    btnPort1Configure.Enabled = false;
                    btnPort2Configure.Enabled = false;
                    cmbPort.Enabled = false;
                    tsFCHearbeat.Visible = false;
                    tsUniAdapterHeartbeat.Visible = false;
                    pbCells.Visible = false;
                    lblUpdateHint.Visible = false;
                    break;
                case StateMachineStep.Connected:
                    btnConnect.Text = "Close";
                    btnUpdate.Visible = false;
                    timer.Enabled = false;
                    tsStatus.Text = "connected";
                    tsStatus.ForeColor = Color.Green;
                    Cursor = Cursors.Default;
                    tsLoading.Visible = false;
                    btnPort1Configure.Enabled = false;
                    btnPort2Configure.Enabled = false;
                    cmbPort.Enabled = false;
                    tsFCHearbeat.Visible = true;
                    tsUniAdapterHeartbeat.Visible = true;
                    tsFWVersion.Visible = true;
                    pbCells.Visible = false;
                    lblUpdateHint.Visible = false;

                    /*
                    Task.Factory.StartNew(() =>
                    {
                        try
                        {
                            string cid = "";

                            try
                            {
                                if (string.IsNullOrWhiteSpace(cid))
                                    cid = Microsoft.Win32.Registry.CurrentUser.OpenSubKey("Software").OpenSubKey("xeniC").OpenSubKey("AnySense").GetValue("user", "").ToString();
                            }
                            catch { }

                            try
                            {
                                if (string.IsNullOrWhiteSpace(cid))
                                {
                                    cid = Guid.NewGuid().ToString();
                                    Microsoft.Win32.Registry.CurrentUser.CreateSubKey("Software").CreateSubKey("xeniC").CreateSubKey("AnySense").SetValue("user", cid);
                                }
                            }
                            catch { }

                            using (WebClient client = new WebClient())
                            {
                                byte[] response =
                                client.UploadValues("http://www.google-analytics.com/collect", new NameValueCollection() {
                                { "v", "1" },
                                { "tid", "UA-57362047-1" },
                                { "cid", cid },
                                { "t", "pageview" },
                                { "dh", "anysense.de" },
                                { "dp", "/checkupdate/"+ tsFWVersion.Text + "/" },
                                { "dt", "Check for AnySense Update" }
                                });

                                string result = System.Text.Encoding.UTF8.GetString(response);
                            }
                        }
                        catch { }
                    });
                     * */

                    break;
                case StateMachineStep.Updating:
                    tsFWVersion.Text = "-";
                    ClearLiveData();
                    btnPort1Configure.Enabled = false;
                    btnPort2Configure.Enabled = false;
                    cmbPort.Enabled = false;
                    pbCells.Visible = false;
                    lblUpdateHint.Visible = false;
                    break;
                default:
                    break;
            }

            currentStep = step;
        }

        #endregion

        #region AnySense Handling

        private void ClearLiveData()
        {
            lblFlightMode.Text = "-";
            lblPort1.Text = "-";
            lblPort2.Text = "-";
            lblBattery.Text = "-";
            lblCurrent.Text = "-";
            lblStatLost.Text = "-";
            lblStatDrop.Text = "-";
            lblStatCorrupted.Text = "-";
            lblCOG.Text = "-";
            lblAltitude.Text = "-";
            lblGPSFix.Text = "-";
            lblHDOP.Text = "-";
            lblVDOP.Text = "-";
            lblLatitude.Text = "-";
            lblLongitude.Text = "-";
            lblSpeed.Text = "-";
            lblNumSat.Text = "-";
            lblClimb.Text = "-";
            lblHeading.Text = "-";
            lblThrottle.Text = "-";
            lblRoll.Text = "-";
            lblPitch.Text = "-";
            lblRC1.Text = "-";
            lblRC2.Text = "-";
            lblRC3.Text = "-";
            lblRC4.Text = "-";
            lblRC5.Text = "-";
            lblRC6.Text = "-";
            lblRC7.Text = "-";
            lblRC8.Text = "-";
            toolTip1.RemoveAll();
        }

        private void mavlink_PacketReceived(object sender, MavlinkPacket e)
        {
            if (e.Message == null)
                return;

            if (!Disposing && !IsDisposed)
                BeginInvoke((Action<MavlinkMessage>)ProcessMessage, e.Message);
        }

        private void ProcessMessage(MavlinkMessage msg)
        {
            if (msg == null)
                return;

            Type t = msg.GetType();

            if (currentStep != StateMachineStep.Connected && t != typeof(Msg_configuration_version) && t != typeof(Msg_configuration_version2))
                return;

            if (t == typeof(Msg_configuration_control))
            {
                Msg_configuration_control msg_control = (msg as Msg_configuration_control);
                switch ((CONFIG_COMMAND)msg_control.command)
                {
                    case CONFIG_COMMAND.CONFIG_COMMAND_ACK:

                        if (msg_control.param1 == (byte)CONFIG_PORT.CONFIG_PORT_1)
                        {
                            lblPort1.Text = ((ProtocolType)msg_control.param2).GetDescription();
                        }
                        else if (msg_control.param1 == (byte)CONFIG_PORT.CONFIG_PORT_2)
                        {
                            lblPort2.Text = ((ProtocolType)msg_control.param2).GetDescription();
                        }

                        tsLoading.Visible = false;

                        btnPort1Configure.Enabled = true;
                        btnPort2Configure.Enabled = true;


                        break;
                    case CONFIG_COMMAND.CONFIG_COMMAND_NACK:
                        tsLoading.Visible = false;

                        btnPort1Configure.Enabled = true;
                        btnPort2Configure.Enabled = true;

                        MessageBox.Show(this, string.Format("Error while saving Configuration for Port {0}", msg_control.param1), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);

                        break;
                    case CONFIG_COMMAND.CONFIG_COMMAND_BOOTLOADER:
                        serialPort.Close();
                        StateMachineUpdate(StateMachineStep.None);
                        break;
                    default:
                        break;
                }
            }
            else if (t == typeof(Msg_configuration_port))
            {
                Msg_configuration_port msg_config = (msg as Msg_configuration_port);
                try
                {
                    PortConfiguration config = new PortConfiguration();
                    config.Protocol = (ProtocolType)msg_config.protocol;
                    if (config.Configuration != null)
                        config.Configuration.DeSerialize(msg_config.data);

                    tsLoading.Visible = false;

                    if (PortConfigurationForm.ShowDialog(this, config) == DialogResult.OK)
                    {
                        try
                        {
                            Msg_configuration_port msg_send = new Msg_configuration_port();
                            msg_send.port = msg_config.port;
                            msg_send.protocol = (byte)config.Protocol;
                            if (config.Configuration != null)
                                msg_send.data = config.Configuration.Serialize();
                            else
                                msg_send.data = new byte[128];

                            mavlink_packet.Message = msg_send;

                            byte[] bytes = mavlink.Send(mavlink_packet);
                            serialPort.Write(bytes, 0, bytes.Length);

                            tsLoading.Visible = true;

                            btnPort1Configure.Enabled = false;
                            btnPort2Configure.Enabled = false;

                            Task.Factory.StartNew(() =>
                            {
                                Extensions.TaskExtensions.Delay(5000).Wait();

                                if (!Disposing && !IsDisposed)
                                {
                                    BeginInvoke(((Action)(() =>
                                    {

                                        if (!Disposing && !IsDisposed)
                                        {
                                            if (tsLoading.Visible)
                                            {
                                                tsLoading.Visible = false;

                                                btnPort1Configure.Enabled = true;
                                                btnPort2Configure.Enabled = true;

                                                MessageBox.Show(this, "Could not load Configuration. A connection timeout occurred.", "Timeout", MessageBoxButtons.OK, MessageBoxIcon.Error);

                                            }
                                        }

                                    })));
                                }
                            });
                        }
                        catch (Exception ex)
                        {
                            StateMachineUpdate(StateMachineStep.None);
                            MessageBox.Show(this, "Error while communicating:\n" + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        }

                    }
                    else
                    {
                        btnPort1Configure.Enabled = true;
                        btnPort2Configure.Enabled = true;
                    }
                }
                catch (Exception ex)
                {
                    tsLoading.Visible = false;

                    btnPort1Configure.Enabled = true;
                    btnPort2Configure.Enabled = true;

                    MessageBox.Show(this, string.Format("Error loading Configuration for Port {0}\n{1}", msg_config.port, ex.Message), "Error",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else if (t == typeof(Msg_configuration_version) || t == typeof(Msg_configuration_version2))
            {
                StateMachineUpdate(StateMachineStep.Connected);

                uint version = 0;

                if (t == typeof(Msg_configuration_version))
                {
                    //0x0009
                    Msg_configuration_version msg_version = (msg as Msg_configuration_version);
                    lblPort1.Text = ((ProtocolType)msg_version.port1).GetDescription();
                    lblPort2.Text = ((ProtocolType)msg_version.port2).GetDescription();

                    version = msg_version.fw_version;
                    version = version << 8;
                }
                else
                {
                    //0x00000901
                    Msg_configuration_version2 msg_version = (msg as Msg_configuration_version2);
                    version = msg_version.fw_version;

                    lblPort1.Text = ((ProtocolType)msg_version.port1).GetDescription();
                    lblPort2.Text = ((ProtocolType)msg_version.port2).GetDescription();
                }

                tsFWVersion.Text = string.Format("{0}.{1}.{2}", (byte)(version >> 16), (byte)(version >> 8), (byte)version);

                btnUpdate.Visible = FIRMWARE_VERSION > version;
                lblUpdateHint.Visible = FIRMWARE_VERSION != version;

                lblUpdateHint.Text = (FIRMWARE_VERSION > version ? "Your Firmware ist outdated.\n Please click Update to flash your AnySense with latest Firmware Version." :
                                                                   "Your Konfiguration Manager is outdated.\nPlease visit our support page for more information.");

                btnPort1Configure.Enabled = FIRMWARE_VERSION == version;
                btnPort2Configure.Enabled = FIRMWARE_VERSION == version;
            }
            else if (t == typeof(Msg_heartbeat))
            {
                Msg_heartbeat heartbeat = (msg as Msg_heartbeat);
                lblFlightMode.Text = ((FlightMode)heartbeat.custom_mode).GetDescription();
                tsUniAdapterHeartbeat.BackgroundImage = KonfigurationTool.Properties.Resources.led_g_an;
                if (UAHeartbeat == null || UAHeartbeat.Status == TaskStatus.RanToCompletion)
                {
                    UAHeartbeat = Task.Factory.StartNew(() =>
                    {
                        Extensions.TaskExtensions.Delay(300).Wait();
                        if (!Disposing && !IsDisposed)
                        {
                            BeginInvoke(((Action)(() =>
                            {
                                if (!Disposing && !IsDisposed)
                                    tsUniAdapterHeartbeat.BackgroundImage = KonfigurationTool.Properties.Resources.led_aus;
                            })));
                        }
                    });
                }
            }
            else if (t == typeof(Msg_configuration_naza_heartbeat))
            {
                tsFCHearbeat.BackgroundImage = KonfigurationTool.Properties.Resources.led_b_an;
                if (FCHeartbeat == null || FCHeartbeat.Status == TaskStatus.RanToCompletion)
                {
                    FCHeartbeat = Task.Factory.StartNew(() =>
                    {
                        Extensions.TaskExtensions.Delay(300).Wait();

                        if (!Disposing && !IsDisposed)
                        {
                            BeginInvoke(((Action)(() =>
                            {
                                if (!Disposing && !IsDisposed)
                                    tsFCHearbeat.BackgroundImage = KonfigurationTool.Properties.Resources.led_aus;
                            })));
                        }
                    });
                }
            }
            else if (t == typeof(Msg_sys_status))
            {
                Msg_sys_status sys = (msg as Msg_sys_status);
                lblBattery.Text = (sys.voltage_battery / 1000f).ToString("0.00 V");
                lblCurrent.Text = (sys.current_battery / 100f).ToString("0.00 A");
                lblStatLost.Text = sys.errors_count1.ToString();
                lblStatDrop.Text = sys.errors_count2.ToString();
                lblStatCorrupted.Text = sys.errors_count3.ToString();
            }
            else if (t == typeof(Msg_gps_raw_int))
            {
                Msg_gps_raw_int gps = (msg as Msg_gps_raw_int);
                lblCOG.Text = (gps.cog / 100f).ToString("0.00 °");
                lblAltitude.Text = (gps.alt / 1000f).ToString("0.00 m");
                lblGPSFix.Text = ((FixType)gps.fix_type).GetDescription();
                lblHDOP.Text = (gps.eph / 100f).ToString("0.00");
                lblVDOP.Text = (gps.epv / 100f).ToString("0.00");
                lblLatitude.Text = (gps.lat / 10000000.0f).ToString("0.000000");
                lblLongitude.Text = (gps.lon / 10000000.0f).ToString("0.000000");
                lblSpeed.Text = (gps.vel / 100f).ToString("0.00 m/s");
                lblNumSat.Text = gps.satellites_visible.ToString();

            }
            else if (t == typeof(Msg_vfr_hud))
            {
                Msg_vfr_hud vfr = (msg as Msg_vfr_hud);
                lblClimb.Text = vfr.climb.ToString("0.00 m/s");
                lblAltitude.Text = vfr.alt.ToString("0.00 m");
                lblSpeed.Text = vfr.groundspeed.ToString("0.00 m/s");
                lblHeading.Text = vfr.heading.ToString("0 °");
                lblThrottle.Text = vfr.throttle.ToString() + " %";

            }
            else if (t == typeof(Msg_attitude))
            {
                Msg_attitude att = (msg as Msg_attitude);
                lblRoll.Text = (att.roll * 180.0 / Math.PI).ToString("0.00 °");
                lblPitch.Text = (att.pitch * 180.0 / Math.PI).ToString("0.00 °");
            }
            else if (t == typeof(Msg_rc_channels))
            {
                Msg_rc_channels rc = (msg as Msg_rc_channels);
                lblRC1.Text = rc.chan2_raw.ToString();
                lblRC2.Text = rc.chan3_raw.ToString();
                lblRC3.Text = rc.chan6_raw.ToString();
                lblRC4.Text = rc.chan4_raw.ToString();
                lblRC5.Text = rc.chan8_raw.ToString();
                lblRC6.Text = rc.chan9_raw.ToString();
                lblRC7.Text = rc.chan5_raw.ToString();
                lblRC8.Text = rc.chan10_raw.ToString();

            }
            else if (t == typeof(Msg_battery_status))
            {
                Msg_battery_status batt = (msg as Msg_battery_status);


                string text = "";
                int index = 1;
                foreach (ushort cell in batt.voltages)
                {
                    if (!string.IsNullOrEmpty(text))
                        text += Environment.NewLine;

                    text += string.Format("{0:00}. {1:0.00} V", index, ((float)cell / 1000f));
                    index++;
                }

                if (!string.IsNullOrEmpty(text))
                {
                    toolTip1.SetToolTip(pbCells, text);
                    pbCells.Visible = true;
                }
            }

        }

        private void Connect()
        {
            try
            {
                serialPort.Open();
                StateMachineUpdate(StateMachineStep.Open);

            }
            catch (Exception ex)
            {
                MessageBox.Show(this, "Error while open serial port:\n" + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void Disconnect(bool notifyDevice)
        {
            try
            {
                if (notifyDevice)
                {
                    Msg_configuration_control msg = new Msg_configuration_control();
                    msg.command = (byte)CONFIG_COMMAND.CONFIG_COMMAND_EXIT;
                    mavlink_packet.Message = msg;
                    byte[] bytes = mavlink.Send(mavlink_packet);
                    serialPort.Write(bytes, 0, bytes.Length);
                }
                serialPort.Close();

            }
            catch (Exception ex)
            {
                MessageBox.Show(this, "Error while communication:\n" + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            if (!serialPort.IsOpen)
                StateMachineUpdate(StateMachineStep.None);
        }

        private void btnUpdate_Click(object sender, EventArgs e)
        {
            if (tsLoading.Visible)
                return;


            string path = string.Empty;

            try
            {
                Msg_configuration_control msg = new Msg_configuration_control();
                msg.command = (byte)CONFIG_COMMAND.CONFIG_COMMAND_BOOTLOADER;
                mavlink_packet.Message = msg;
                byte[] bytes = mavlink.Send(mavlink_packet);
                serialPort.Write(bytes, 0, bytes.Length);

                Disconnect(false);
                Enabled = false;
                UpdateForm.ShowDialog(this, serialPort.PortName, path);

            }
            catch (Exception ex)
            {
                MessageBox.Show(this, "Error while communication:\n" + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            Enabled = true;
            Connect();
        }

        private void ConfigurePort(CONFIG_PORT port)
        {
            Msg_configuration_control msg = new Msg_configuration_control();
            msg.command = (byte)CONFIG_COMMAND.CONFIG_COMMAND_GET_CONFIGURATION;
            msg.param1 = (byte)port;
            mavlink_packet.Message = msg;
            byte[] bytes = mavlink.Send(mavlink_packet);
            serialPort.Write(bytes, 0, bytes.Length);

            tsLoading.Visible = true;

            btnPort1Configure.Enabled = false;
            btnPort2Configure.Enabled = false;

            Task.Factory.StartNew(() =>
            {
                Extensions.TaskExtensions.Delay(5000).Wait();

                if (!Disposing && !IsDisposed)
                {
                    BeginInvoke(((Action)(() =>
                    {

                        if (!Disposing && !IsDisposed)
                        {
                            if (tsLoading.Visible)
                            {
                                tsLoading.Visible = false;

                                btnPort1Configure.Enabled = true;
                                btnPort2Configure.Enabled = true;

                                MessageBox.Show(this, "Could not load Configuration. A connection timeout occurred.", "Timeout", MessageBoxButtons.OK, MessageBoxIcon.Error);

                            }
                        }

                    })));
                }
            });

        }

        #endregion

        #region Serial Port Handling

        private void serialPort_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            List<byte> bytes = new List<byte>();
            while (serialPort.IsOpen && serialPort.BytesToRead > 0)
            {
                if (serialPort.IsOpen)
                    bytes.Add((byte)serialPort.ReadByte());
            }

            mavlink.ParseBytes(bytes.ToArray());
        }

        private void MonitorDeviceChanges()
        {
            try
            {
                var deviceChangedQuery = new WqlEventQuery("SELECT * FROM Win32_DeviceChangeEvent WHERE EventType = 2 or EventType = 3 GROUP WITHIN 3");

                deviceChanged = new ManagementEventWatcher(deviceChangedQuery);

                deviceChanged.EventArrived += (o, args) =>
                {
                    try
                    {
                        UpdateDeviceList();
                    }
                    catch { }
                };

                // Start listening for events
                deviceChanged.Start();
            }
            catch { }
        }

        private void UpdateDeviceList()
        {
            if (InvokeRequired)
                BeginInvoke((Action)UpdateDeviceList);
            else
            {
                IList ports = serialPort.GetPortInformations();
                cmbPort.DataSource = ports;
                cmbPort.ValueMember = "Key";
                cmbPort.DisplayMember = "Value";
                cmbPort.Refresh();

                if (currentStep == StateMachineStep.None)
                {
                    bool found = false;

                    foreach (KeyValuePair<string, string> pair in ports)
                    {
                        if (pair.Value.ToLower().Contains("prolific"))
                        {
                            cmbPort.SelectedValue = pair.Key;
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        foreach (KeyValuePair<string, string> pair in ports)
                        {
                            if (pair.Value.ToLower().Contains("silicon"))
                            {
                                cmbPort.SelectedValue = pair.Key;
                                found = true;
                                break;
                            }
                        }
                    }
                    if (!found && ports.Count > 0)
                        cmbPort.SelectedValue = ((KeyValuePair<string, string>)ports[0]).Key;

                }
                else if (!serialPort.IsOpen)
                {
                    Disconnect(false);
                }
                else
                {
                    foreach (KeyValuePair<string, string> pair in ports)
                    {
                        if (string.Compare(serialPort.PortName, pair.Key, true) == 0)
                        {
                            cmbPort.SelectedValue = pair.Key;
                            break;
                        }
                    }

                }
            }
        }

        #endregion

    }
}
