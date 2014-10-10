namespace KonfigurationTool
{
    partial class MainForm
    {
        /// <summary>
        /// Erforderliche Designervariable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Verwendete Ressourcen bereinigen.
        /// </summary>
        /// <param name="disposing">True, wenn verwaltete Ressourcen gelöscht werden sollen; andernfalls False.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Vom Windows Form-Designer generierter Code

        /// <summary>
        /// Erforderliche Methode für die Designerunterstützung.
        /// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.serialPort = new System.IO.Ports.SerialPort(this.components);
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.lblPort = new System.Windows.Forms.Label();
            this.pnlTop = new System.Windows.Forms.Panel();
            this.btnUpdate = new System.Windows.Forms.Button();
            this.btnConnect = new System.Windows.Forms.Button();
            this.cmbPort = new System.Windows.Forms.ComboBox();
            this.groupRC = new System.Windows.Forms.GroupBox();
            this.lblRC8 = new System.Windows.Forms.Label();
            this.lblRC8Label = new System.Windows.Forms.Label();
            this.lblRC7 = new System.Windows.Forms.Label();
            this.lblRC7Label = new System.Windows.Forms.Label();
            this.lblRC6 = new System.Windows.Forms.Label();
            this.lblRC6Label = new System.Windows.Forms.Label();
            this.lblRC5 = new System.Windows.Forms.Label();
            this.lblRC5Label = new System.Windows.Forms.Label();
            this.lblRC2Label = new System.Windows.Forms.Label();
            this.lblRC4 = new System.Windows.Forms.Label();
            this.lblRC2 = new System.Windows.Forms.Label();
            this.lblRC4Label = new System.Windows.Forms.Label();
            this.lblRC1Label = new System.Windows.Forms.Label();
            this.lblRC1 = new System.Windows.Forms.Label();
            this.lblRC3 = new System.Windows.Forms.Label();
            this.lblRuderLabel = new System.Windows.Forms.Label();
            this.grpChannelStats = new System.Windows.Forms.GroupBox();
            this.lblStatCorruptedLabel = new System.Windows.Forms.Label();
            this.lblStatCorrupted = new System.Windows.Forms.Label();
            this.lblStatDropLabel = new System.Windows.Forms.Label();
            this.lblStatDrop = new System.Windows.Forms.Label();
            this.lblStatLostLabel = new System.Windows.Forms.Label();
            this.lblStatLost = new System.Windows.Forms.Label();
            this.gpGenerall = new System.Windows.Forms.GroupBox();
            this.lblBattery = new System.Windows.Forms.Label();
            this.lblBatteryLabel = new System.Windows.Forms.Label();
            this.lblFlightMode = new System.Windows.Forms.Label();
            this.lblFlightModeLabel = new System.Windows.Forms.Label();
            this.groupTelemetry = new System.Windows.Forms.GroupBox();
            this.lblPitch = new System.Windows.Forms.Label();
            this.lblPitchLabel = new System.Windows.Forms.Label();
            this.lblRoll = new System.Windows.Forms.Label();
            this.lblRollLabel = new System.Windows.Forms.Label();
            this.lblThrottle = new System.Windows.Forms.Label();
            this.lblThrottleLabel = new System.Windows.Forms.Label();
            this.lblClimb = new System.Windows.Forms.Label();
            this.lblClimbLabel = new System.Windows.Forms.Label();
            this.lblHeadingLabel = new System.Windows.Forms.Label();
            this.lblCOG = new System.Windows.Forms.Label();
            this.lblHeading = new System.Windows.Forms.Label();
            this.lblCOGLabel = new System.Windows.Forms.Label();
            this.lblAltitudeLabel = new System.Windows.Forms.Label();
            this.lblAltitude = new System.Windows.Forms.Label();
            this.lblSpeed = new System.Windows.Forms.Label();
            this.lblSpeedLabel = new System.Windows.Forms.Label();
            this.groupGPS = new System.Windows.Forms.GroupBox();
            this.lblNumSat = new System.Windows.Forms.Label();
            this.lblNumSatLabel = new System.Windows.Forms.Label();
            this.lblVDOP = new System.Windows.Forms.Label();
            this.lblVDOPLabel = new System.Windows.Forms.Label();
            this.lblHDOP = new System.Windows.Forms.Label();
            this.lblHDOPLabel = new System.Windows.Forms.Label();
            this.lblGPSFix = new System.Windows.Forms.Label();
            this.lblGPSFixLabel = new System.Windows.Forms.Label();
            this.lblLongitude = new System.Windows.Forms.Label();
            this.lblLongitudeLabel = new System.Windows.Forms.Label();
            this.lblLatitude = new System.Windows.Forms.Label();
            this.lblLatitudeLabel = new System.Windows.Forms.Label();
            this.statusStrip = new System.Windows.Forms.StatusStrip();
            this.tsStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.tsStatus = new System.Windows.Forms.ToolStripStatusLabel();
            this.tsLoading = new System.Windows.Forms.ToolStripProgressBar();
            this.tsFWVersionLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.tsFWVersion = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripStatusLabel3 = new System.Windows.Forms.ToolStripStatusLabel();
            this.tsFCHearbeat = new System.Windows.Forms.ToolStripStatusLabel();
            this.tsUniAdapterHeartbeat = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblPort1Label = new System.Windows.Forms.Label();
            this.lblPort1 = new System.Windows.Forms.Label();
            this.btnPort1Configure = new System.Windows.Forms.Button();
            this.grpPorts = new System.Windows.Forms.GroupBox();
            this.btnPort2Configure = new System.Windows.Forms.Button();
            this.lblPort2 = new System.Windows.Forms.Label();
            this.lblPort2Label = new System.Windows.Forms.Label();
            this.pnlTop.SuspendLayout();
            this.groupRC.SuspendLayout();
            this.grpChannelStats.SuspendLayout();
            this.gpGenerall.SuspendLayout();
            this.groupTelemetry.SuspendLayout();
            this.groupGPS.SuspendLayout();
            this.statusStrip.SuspendLayout();
            this.grpPorts.SuspendLayout();
            this.SuspendLayout();
            // 
            // serialPort
            // 
            this.serialPort.BaudRate = 57600;
            this.serialPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort_DataReceived);
            // 
            // timer
            // 
            this.timer.Interval = 300;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // lblPort
            // 
            this.lblPort.Location = new System.Drawing.Point(1, 8);
            this.lblPort.Name = "lblPort";
            this.lblPort.Size = new System.Drawing.Size(58, 25);
            this.lblPort.TabIndex = 0;
            this.lblPort.Text = "Serial Port:";
            this.lblPort.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // pnlTop
            // 
            this.pnlTop.AutoSize = true;
            this.pnlTop.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.pnlTop.Controls.Add(this.btnUpdate);
            this.pnlTop.Controls.Add(this.btnConnect);
            this.pnlTop.Controls.Add(this.cmbPort);
            this.pnlTop.Controls.Add(this.lblPort);
            this.pnlTop.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnlTop.Location = new System.Drawing.Point(0, 0);
            this.pnlTop.Margin = new System.Windows.Forms.Padding(10);
            this.pnlTop.MinimumSize = new System.Drawing.Size(0, 35);
            this.pnlTop.Name = "pnlTop";
            this.pnlTop.Padding = new System.Windows.Forms.Padding(5);
            this.pnlTop.Size = new System.Drawing.Size(666, 41);
            this.pnlTop.TabIndex = 1;
            // 
            // btnUpdate
            // 
            this.btnUpdate.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.btnUpdate.Location = new System.Drawing.Point(358, 8);
            this.btnUpdate.Name = "btnUpdate";
            this.btnUpdate.Size = new System.Drawing.Size(75, 25);
            this.btnUpdate.TabIndex = 3;
            this.btnUpdate.Text = "Update";
            this.btnUpdate.UseVisualStyleBackColor = true;
            this.btnUpdate.Click += new System.EventHandler(this.btnUpdate_Click);
            // 
            // btnConnect
            // 
            this.btnConnect.Location = new System.Drawing.Point(277, 8);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(75, 25);
            this.btnConnect.TabIndex = 2;
            this.btnConnect.Text = "Open";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // cmbPort
            // 
            this.cmbPort.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbPort.FormattingEnabled = true;
            this.cmbPort.Location = new System.Drawing.Point(63, 9);
            this.cmbPort.MinimumSize = new System.Drawing.Size(150, 0);
            this.cmbPort.Name = "cmbPort";
            this.cmbPort.Size = new System.Drawing.Size(208, 21);
            this.cmbPort.TabIndex = 1;
            this.cmbPort.SelectedIndexChanged += new System.EventHandler(this.cmbPort_SelectedIndexChanged);
            // 
            // groupRC
            // 
            this.groupRC.Controls.Add(this.lblRC8);
            this.groupRC.Controls.Add(this.lblRC8Label);
            this.groupRC.Controls.Add(this.lblRC7);
            this.groupRC.Controls.Add(this.lblRC7Label);
            this.groupRC.Controls.Add(this.lblRC6);
            this.groupRC.Controls.Add(this.lblRC6Label);
            this.groupRC.Controls.Add(this.lblRC5);
            this.groupRC.Controls.Add(this.lblRC5Label);
            this.groupRC.Controls.Add(this.lblRC2Label);
            this.groupRC.Controls.Add(this.lblRC4);
            this.groupRC.Controls.Add(this.lblRC2);
            this.groupRC.Controls.Add(this.lblRC4Label);
            this.groupRC.Controls.Add(this.lblRC1Label);
            this.groupRC.Controls.Add(this.lblRC1);
            this.groupRC.Controls.Add(this.lblRC3);
            this.groupRC.Controls.Add(this.lblRuderLabel);
            this.groupRC.Location = new System.Drawing.Point(334, 282);
            this.groupRC.Name = "groupRC";
            this.groupRC.Size = new System.Drawing.Size(316, 168);
            this.groupRC.TabIndex = 6;
            this.groupRC.TabStop = false;
            this.groupRC.Text = "RC Channels (PPM Modulation)";
            // 
            // lblRC8
            // 
            this.lblRC8.AutoSize = true;
            this.lblRC8.Location = new System.Drawing.Point(149, 147);
            this.lblRC8.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRC8.Name = "lblRC8";
            this.lblRC8.Size = new System.Drawing.Size(10, 13);
            this.lblRC8.TabIndex = 39;
            this.lblRC8.Text = "-";
            // 
            // lblRC8Label
            // 
            this.lblRC8Label.AutoSize = true;
            this.lblRC8Label.Location = new System.Drawing.Point(6, 147);
            this.lblRC8Label.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRC8Label.Name = "lblRC8Label";
            this.lblRC8Label.Size = new System.Drawing.Size(55, 13);
            this.lblRC8Label.TabIndex = 38;
            this.lblRC8Label.Text = "Channel 8";
            // 
            // lblRC7
            // 
            this.lblRC7.AutoSize = true;
            this.lblRC7.Location = new System.Drawing.Point(149, 129);
            this.lblRC7.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRC7.Name = "lblRC7";
            this.lblRC7.Size = new System.Drawing.Size(10, 13);
            this.lblRC7.TabIndex = 37;
            this.lblRC7.Text = "-";
            // 
            // lblRC7Label
            // 
            this.lblRC7Label.AutoSize = true;
            this.lblRC7Label.Location = new System.Drawing.Point(6, 129);
            this.lblRC7Label.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRC7Label.Name = "lblRC7Label";
            this.lblRC7Label.Size = new System.Drawing.Size(72, 13);
            this.lblRC7Label.TabIndex = 36;
            this.lblRC7Label.Text = "Channel 7 (U)";
            // 
            // lblRC6
            // 
            this.lblRC6.AutoSize = true;
            this.lblRC6.Location = new System.Drawing.Point(149, 111);
            this.lblRC6.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRC6.Name = "lblRC6";
            this.lblRC6.Size = new System.Drawing.Size(10, 13);
            this.lblRC6.TabIndex = 35;
            this.lblRC6.Text = "-";
            // 
            // lblRC6Label
            // 
            this.lblRC6Label.AutoSize = true;
            this.lblRC6Label.Location = new System.Drawing.Point(6, 111);
            this.lblRC6Label.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRC6Label.Name = "lblRC6Label";
            this.lblRC6Label.Size = new System.Drawing.Size(77, 13);
            this.lblRC6Label.TabIndex = 34;
            this.lblRC6Label.Text = "Channel 6 (X2)";
            // 
            // lblRC5
            // 
            this.lblRC5.AccessibleName = "";
            this.lblRC5.AutoSize = true;
            this.lblRC5.Location = new System.Drawing.Point(149, 93);
            this.lblRC5.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRC5.Name = "lblRC5";
            this.lblRC5.Size = new System.Drawing.Size(10, 13);
            this.lblRC5.TabIndex = 33;
            this.lblRC5.Text = "-";
            // 
            // lblRC5Label
            // 
            this.lblRC5Label.AutoSize = true;
            this.lblRC5Label.Location = new System.Drawing.Point(6, 93);
            this.lblRC5Label.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRC5Label.Name = "lblRC5Label";
            this.lblRC5Label.Size = new System.Drawing.Size(77, 13);
            this.lblRC5Label.TabIndex = 32;
            this.lblRC5Label.Text = "Channel 5 (X1)";
            // 
            // lblRC2Label
            // 
            this.lblRC2Label.AutoSize = true;
            this.lblRC2Label.Location = new System.Drawing.Point(6, 39);
            this.lblRC2Label.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRC2Label.Name = "lblRC2Label";
            this.lblRC2Label.Size = new System.Drawing.Size(71, 13);
            this.lblRC2Label.TabIndex = 26;
            this.lblRC2Label.Text = "Channel 2 (E)";
            // 
            // lblRC4
            // 
            this.lblRC4.AccessibleName = "";
            this.lblRC4.AutoSize = true;
            this.lblRC4.Location = new System.Drawing.Point(149, 75);
            this.lblRC4.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRC4.Name = "lblRC4";
            this.lblRC4.Size = new System.Drawing.Size(10, 13);
            this.lblRC4.TabIndex = 31;
            this.lblRC4.Text = "-";
            // 
            // lblRC2
            // 
            this.lblRC2.AccessibleName = "";
            this.lblRC2.AutoSize = true;
            this.lblRC2.Location = new System.Drawing.Point(149, 39);
            this.lblRC2.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRC2.Name = "lblRC2";
            this.lblRC2.Size = new System.Drawing.Size(10, 13);
            this.lblRC2.TabIndex = 27;
            this.lblRC2.Text = "-";
            // 
            // lblRC4Label
            // 
            this.lblRC4Label.AutoSize = true;
            this.lblRC4Label.Location = new System.Drawing.Point(6, 75);
            this.lblRC4Label.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRC4Label.Name = "lblRC4Label";
            this.lblRC4Label.Size = new System.Drawing.Size(72, 13);
            this.lblRC4Label.TabIndex = 30;
            this.lblRC4Label.Text = "Channel 4 (R)";
            // 
            // lblRC1Label
            // 
            this.lblRC1Label.AutoSize = true;
            this.lblRC1Label.Location = new System.Drawing.Point(6, 21);
            this.lblRC1Label.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRC1Label.Name = "lblRC1Label";
            this.lblRC1Label.Size = new System.Drawing.Size(71, 13);
            this.lblRC1Label.TabIndex = 24;
            this.lblRC1Label.Text = "Channel 1 (A)";
            // 
            // lblRC1
            // 
            this.lblRC1.AccessibleName = "";
            this.lblRC1.AutoSize = true;
            this.lblRC1.Location = new System.Drawing.Point(149, 21);
            this.lblRC1.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRC1.Name = "lblRC1";
            this.lblRC1.Size = new System.Drawing.Size(10, 13);
            this.lblRC1.TabIndex = 25;
            this.lblRC1.Text = "-";
            // 
            // lblRC3
            // 
            this.lblRC3.AccessibleName = "";
            this.lblRC3.AutoSize = true;
            this.lblRC3.Location = new System.Drawing.Point(149, 57);
            this.lblRC3.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRC3.Name = "lblRC3";
            this.lblRC3.Size = new System.Drawing.Size(10, 13);
            this.lblRC3.TabIndex = 29;
            this.lblRC3.Text = "-";
            // 
            // lblRuderLabel
            // 
            this.lblRuderLabel.AutoSize = true;
            this.lblRuderLabel.Location = new System.Drawing.Point(6, 57);
            this.lblRuderLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRuderLabel.Name = "lblRuderLabel";
            this.lblRuderLabel.Size = new System.Drawing.Size(71, 13);
            this.lblRuderLabel.TabIndex = 28;
            this.lblRuderLabel.Text = "Channel 3 (T)";
            // 
            // grpChannelStats
            // 
            this.grpChannelStats.Controls.Add(this.lblStatCorruptedLabel);
            this.grpChannelStats.Controls.Add(this.lblStatCorrupted);
            this.grpChannelStats.Controls.Add(this.lblStatDropLabel);
            this.grpChannelStats.Controls.Add(this.lblStatDrop);
            this.grpChannelStats.Controls.Add(this.lblStatLostLabel);
            this.grpChannelStats.Controls.Add(this.lblStatLost);
            this.grpChannelStats.Location = new System.Drawing.Point(12, 120);
            this.grpChannelStats.Name = "grpChannelStats";
            this.grpChannelStats.Size = new System.Drawing.Size(316, 77);
            this.grpChannelStats.TabIndex = 5;
            this.grpChannelStats.TabStop = false;
            this.grpChannelStats.Text = "Channel Stats";
            // 
            // lblStatCorruptedLabel
            // 
            this.lblStatCorruptedLabel.AutoSize = true;
            this.lblStatCorruptedLabel.Cursor = System.Windows.Forms.Cursors.Default;
            this.lblStatCorruptedLabel.Location = new System.Drawing.Point(6, 57);
            this.lblStatCorruptedLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblStatCorruptedLabel.Name = "lblStatCorruptedLabel";
            this.lblStatCorruptedLabel.Size = new System.Drawing.Size(95, 13);
            this.lblStatCorruptedLabel.TabIndex = 12;
            this.lblStatCorruptedLabel.Text = "Packets Corrupted";
            // 
            // lblStatCorrupted
            // 
            this.lblStatCorrupted.AutoSize = true;
            this.lblStatCorrupted.Cursor = System.Windows.Forms.Cursors.Default;
            this.lblStatCorrupted.Location = new System.Drawing.Point(149, 57);
            this.lblStatCorrupted.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblStatCorrupted.Name = "lblStatCorrupted";
            this.lblStatCorrupted.Size = new System.Drawing.Size(10, 13);
            this.lblStatCorrupted.TabIndex = 13;
            this.lblStatCorrupted.Text = "-";
            // 
            // lblStatDropLabel
            // 
            this.lblStatDropLabel.AutoSize = true;
            this.lblStatDropLabel.Cursor = System.Windows.Forms.Cursors.Default;
            this.lblStatDropLabel.Location = new System.Drawing.Point(6, 39);
            this.lblStatDropLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblStatDropLabel.Name = "lblStatDropLabel";
            this.lblStatDropLabel.Size = new System.Drawing.Size(72, 13);
            this.lblStatDropLabel.TabIndex = 10;
            this.lblStatDropLabel.Text = "Packets Drop";
            // 
            // lblStatDrop
            // 
            this.lblStatDrop.AutoSize = true;
            this.lblStatDrop.Cursor = System.Windows.Forms.Cursors.Default;
            this.lblStatDrop.Location = new System.Drawing.Point(149, 39);
            this.lblStatDrop.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblStatDrop.Name = "lblStatDrop";
            this.lblStatDrop.Size = new System.Drawing.Size(10, 13);
            this.lblStatDrop.TabIndex = 11;
            this.lblStatDrop.Text = "-";
            // 
            // lblStatLostLabel
            // 
            this.lblStatLostLabel.AutoSize = true;
            this.lblStatLostLabel.Location = new System.Drawing.Point(6, 21);
            this.lblStatLostLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblStatLostLabel.Name = "lblStatLostLabel";
            this.lblStatLostLabel.Size = new System.Drawing.Size(69, 13);
            this.lblStatLostLabel.TabIndex = 8;
            this.lblStatLostLabel.Text = "Packets Lost";
            // 
            // lblStatLost
            // 
            this.lblStatLost.AutoSize = true;
            this.lblStatLost.Location = new System.Drawing.Point(149, 21);
            this.lblStatLost.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblStatLost.Name = "lblStatLost";
            this.lblStatLost.Size = new System.Drawing.Size(10, 13);
            this.lblStatLost.TabIndex = 9;
            this.lblStatLost.Text = "-";
            // 
            // gpGenerall
            // 
            this.gpGenerall.Controls.Add(this.lblBattery);
            this.gpGenerall.Controls.Add(this.lblBatteryLabel);
            this.gpGenerall.Controls.Add(this.lblFlightMode);
            this.gpGenerall.Controls.Add(this.lblFlightModeLabel);
            this.gpGenerall.Location = new System.Drawing.Point(12, 54);
            this.gpGenerall.Name = "gpGenerall";
            this.gpGenerall.Size = new System.Drawing.Size(316, 60);
            this.gpGenerall.TabIndex = 4;
            this.gpGenerall.TabStop = false;
            this.gpGenerall.Text = "General";
            // 
            // lblBattery
            // 
            this.lblBattery.AutoSize = true;
            this.lblBattery.Location = new System.Drawing.Point(149, 39);
            this.lblBattery.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblBattery.Name = "lblBattery";
            this.lblBattery.Size = new System.Drawing.Size(10, 13);
            this.lblBattery.TabIndex = 3;
            this.lblBattery.Text = "-";
            // 
            // lblBatteryLabel
            // 
            this.lblBatteryLabel.AutoSize = true;
            this.lblBatteryLabel.Location = new System.Drawing.Point(6, 39);
            this.lblBatteryLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblBatteryLabel.Name = "lblBatteryLabel";
            this.lblBatteryLabel.Size = new System.Drawing.Size(43, 13);
            this.lblBatteryLabel.TabIndex = 2;
            this.lblBatteryLabel.Text = "Battery:";
            // 
            // lblFlightMode
            // 
            this.lblFlightMode.AutoSize = true;
            this.lblFlightMode.Location = new System.Drawing.Point(149, 20);
            this.lblFlightMode.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblFlightMode.Name = "lblFlightMode";
            this.lblFlightMode.Size = new System.Drawing.Size(10, 13);
            this.lblFlightMode.TabIndex = 1;
            this.lblFlightMode.Text = "-";
            // 
            // lblFlightModeLabel
            // 
            this.lblFlightModeLabel.AutoSize = true;
            this.lblFlightModeLabel.Location = new System.Drawing.Point(6, 21);
            this.lblFlightModeLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblFlightModeLabel.Name = "lblFlightModeLabel";
            this.lblFlightModeLabel.Size = new System.Drawing.Size(65, 13);
            this.lblFlightModeLabel.TabIndex = 0;
            this.lblFlightModeLabel.Text = "Flight Mode:";
            // 
            // groupTelemetry
            // 
            this.groupTelemetry.Controls.Add(this.lblPitch);
            this.groupTelemetry.Controls.Add(this.lblPitchLabel);
            this.groupTelemetry.Controls.Add(this.lblRoll);
            this.groupTelemetry.Controls.Add(this.lblRollLabel);
            this.groupTelemetry.Controls.Add(this.lblThrottle);
            this.groupTelemetry.Controls.Add(this.lblThrottleLabel);
            this.groupTelemetry.Controls.Add(this.lblClimb);
            this.groupTelemetry.Controls.Add(this.lblClimbLabel);
            this.groupTelemetry.Controls.Add(this.lblHeadingLabel);
            this.groupTelemetry.Controls.Add(this.lblCOG);
            this.groupTelemetry.Controls.Add(this.lblHeading);
            this.groupTelemetry.Controls.Add(this.lblCOGLabel);
            this.groupTelemetry.Controls.Add(this.lblAltitudeLabel);
            this.groupTelemetry.Controls.Add(this.lblAltitude);
            this.groupTelemetry.Controls.Add(this.lblSpeed);
            this.groupTelemetry.Controls.Add(this.lblSpeedLabel);
            this.groupTelemetry.Location = new System.Drawing.Point(12, 203);
            this.groupTelemetry.Name = "groupTelemetry";
            this.groupTelemetry.Size = new System.Drawing.Size(316, 168);
            this.groupTelemetry.TabIndex = 3;
            this.groupTelemetry.TabStop = false;
            this.groupTelemetry.Text = "Telemetry";
            // 
            // lblPitch
            // 
            this.lblPitch.AutoSize = true;
            this.lblPitch.Location = new System.Drawing.Point(149, 147);
            this.lblPitch.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblPitch.Name = "lblPitch";
            this.lblPitch.Size = new System.Drawing.Size(10, 13);
            this.lblPitch.TabIndex = 23;
            this.lblPitch.Text = "-";
            // 
            // lblPitchLabel
            // 
            this.lblPitchLabel.AutoSize = true;
            this.lblPitchLabel.Location = new System.Drawing.Point(6, 147);
            this.lblPitchLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblPitchLabel.Name = "lblPitchLabel";
            this.lblPitchLabel.Size = new System.Drawing.Size(31, 13);
            this.lblPitchLabel.TabIndex = 22;
            this.lblPitchLabel.Text = "Pitch";
            // 
            // lblRoll
            // 
            this.lblRoll.AutoSize = true;
            this.lblRoll.Location = new System.Drawing.Point(149, 129);
            this.lblRoll.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRoll.Name = "lblRoll";
            this.lblRoll.Size = new System.Drawing.Size(10, 13);
            this.lblRoll.TabIndex = 21;
            this.lblRoll.Text = "-";
            // 
            // lblRollLabel
            // 
            this.lblRollLabel.AutoSize = true;
            this.lblRollLabel.Location = new System.Drawing.Point(6, 129);
            this.lblRollLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblRollLabel.Name = "lblRollLabel";
            this.lblRollLabel.Size = new System.Drawing.Size(25, 13);
            this.lblRollLabel.TabIndex = 20;
            this.lblRollLabel.Text = "Roll";
            // 
            // lblThrottle
            // 
            this.lblThrottle.AutoSize = true;
            this.lblThrottle.Location = new System.Drawing.Point(149, 111);
            this.lblThrottle.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblThrottle.Name = "lblThrottle";
            this.lblThrottle.Size = new System.Drawing.Size(10, 13);
            this.lblThrottle.TabIndex = 19;
            this.lblThrottle.Text = "-";
            // 
            // lblThrottleLabel
            // 
            this.lblThrottleLabel.AutoSize = true;
            this.lblThrottleLabel.Location = new System.Drawing.Point(6, 111);
            this.lblThrottleLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblThrottleLabel.Name = "lblThrottleLabel";
            this.lblThrottleLabel.Size = new System.Drawing.Size(43, 13);
            this.lblThrottleLabel.TabIndex = 18;
            this.lblThrottleLabel.Text = "Throttle";
            // 
            // lblClimb
            // 
            this.lblClimb.AutoSize = true;
            this.lblClimb.Location = new System.Drawing.Point(149, 93);
            this.lblClimb.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblClimb.Name = "lblClimb";
            this.lblClimb.Size = new System.Drawing.Size(10, 13);
            this.lblClimb.TabIndex = 17;
            this.lblClimb.Text = "-";
            // 
            // lblClimbLabel
            // 
            this.lblClimbLabel.AutoSize = true;
            this.lblClimbLabel.Location = new System.Drawing.Point(6, 93);
            this.lblClimbLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblClimbLabel.Name = "lblClimbLabel";
            this.lblClimbLabel.Size = new System.Drawing.Size(32, 13);
            this.lblClimbLabel.TabIndex = 16;
            this.lblClimbLabel.Text = "Climb";
            // 
            // lblHeadingLabel
            // 
            this.lblHeadingLabel.AutoSize = true;
            this.lblHeadingLabel.Location = new System.Drawing.Point(6, 39);
            this.lblHeadingLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblHeadingLabel.Name = "lblHeadingLabel";
            this.lblHeadingLabel.Size = new System.Drawing.Size(47, 13);
            this.lblHeadingLabel.TabIndex = 8;
            this.lblHeadingLabel.Text = "Heading";
            // 
            // lblCOG
            // 
            this.lblCOG.AutoSize = true;
            this.lblCOG.Location = new System.Drawing.Point(149, 75);
            this.lblCOG.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblCOG.Name = "lblCOG";
            this.lblCOG.Size = new System.Drawing.Size(10, 13);
            this.lblCOG.TabIndex = 15;
            this.lblCOG.Text = "-";
            // 
            // lblHeading
            // 
            this.lblHeading.AutoSize = true;
            this.lblHeading.Location = new System.Drawing.Point(149, 39);
            this.lblHeading.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblHeading.Name = "lblHeading";
            this.lblHeading.Size = new System.Drawing.Size(10, 13);
            this.lblHeading.TabIndex = 9;
            this.lblHeading.Text = "-";
            // 
            // lblCOGLabel
            // 
            this.lblCOGLabel.AutoSize = true;
            this.lblCOGLabel.Location = new System.Drawing.Point(6, 75);
            this.lblCOGLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblCOGLabel.Name = "lblCOGLabel";
            this.lblCOGLabel.Size = new System.Drawing.Size(30, 13);
            this.lblCOGLabel.TabIndex = 14;
            this.lblCOGLabel.Text = "COG";
            // 
            // lblAltitudeLabel
            // 
            this.lblAltitudeLabel.AutoSize = true;
            this.lblAltitudeLabel.Location = new System.Drawing.Point(6, 21);
            this.lblAltitudeLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblAltitudeLabel.Name = "lblAltitudeLabel";
            this.lblAltitudeLabel.Size = new System.Drawing.Size(67, 13);
            this.lblAltitudeLabel.TabIndex = 6;
            this.lblAltitudeLabel.Text = "Baro Altitude";
            // 
            // lblAltitude
            // 
            this.lblAltitude.AutoSize = true;
            this.lblAltitude.Location = new System.Drawing.Point(149, 21);
            this.lblAltitude.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblAltitude.Name = "lblAltitude";
            this.lblAltitude.Size = new System.Drawing.Size(10, 13);
            this.lblAltitude.TabIndex = 7;
            this.lblAltitude.Text = "-";
            // 
            // lblSpeed
            // 
            this.lblSpeed.AutoSize = true;
            this.lblSpeed.Location = new System.Drawing.Point(149, 57);
            this.lblSpeed.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblSpeed.Name = "lblSpeed";
            this.lblSpeed.Size = new System.Drawing.Size(10, 13);
            this.lblSpeed.TabIndex = 13;
            this.lblSpeed.Text = "-";
            // 
            // lblSpeedLabel
            // 
            this.lblSpeedLabel.AutoSize = true;
            this.lblSpeedLabel.Location = new System.Drawing.Point(6, 57);
            this.lblSpeedLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblSpeedLabel.Name = "lblSpeedLabel";
            this.lblSpeedLabel.Size = new System.Drawing.Size(38, 13);
            this.lblSpeedLabel.TabIndex = 12;
            this.lblSpeedLabel.Text = "Speed";
            // 
            // groupGPS
            // 
            this.groupGPS.Controls.Add(this.lblNumSat);
            this.groupGPS.Controls.Add(this.lblNumSatLabel);
            this.groupGPS.Controls.Add(this.lblVDOP);
            this.groupGPS.Controls.Add(this.lblVDOPLabel);
            this.groupGPS.Controls.Add(this.lblHDOP);
            this.groupGPS.Controls.Add(this.lblHDOPLabel);
            this.groupGPS.Controls.Add(this.lblGPSFix);
            this.groupGPS.Controls.Add(this.lblGPSFixLabel);
            this.groupGPS.Controls.Add(this.lblLongitude);
            this.groupGPS.Controls.Add(this.lblLongitudeLabel);
            this.groupGPS.Controls.Add(this.lblLatitude);
            this.groupGPS.Controls.Add(this.lblLatitudeLabel);
            this.groupGPS.Location = new System.Drawing.Point(334, 144);
            this.groupGPS.Name = "groupGPS";
            this.groupGPS.Size = new System.Drawing.Size(318, 132);
            this.groupGPS.TabIndex = 2;
            this.groupGPS.TabStop = false;
            this.groupGPS.Text = "GPS";
            // 
            // lblNumSat
            // 
            this.lblNumSat.AutoSize = true;
            this.lblNumSat.Location = new System.Drawing.Point(149, 21);
            this.lblNumSat.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblNumSat.Name = "lblNumSat";
            this.lblNumSat.Size = new System.Drawing.Size(10, 13);
            this.lblNumSat.TabIndex = 17;
            this.lblNumSat.Text = "-";
            // 
            // lblNumSatLabel
            // 
            this.lblNumSatLabel.AutoSize = true;
            this.lblNumSatLabel.Location = new System.Drawing.Point(6, 21);
            this.lblNumSatLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblNumSatLabel.Name = "lblNumSatLabel";
            this.lblNumSatLabel.Size = new System.Drawing.Size(49, 13);
            this.lblNumSatLabel.TabIndex = 16;
            this.lblNumSatLabel.Text = "Satellites";
            // 
            // lblVDOP
            // 
            this.lblVDOP.AutoSize = true;
            this.lblVDOP.Location = new System.Drawing.Point(149, 111);
            this.lblVDOP.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblVDOP.Name = "lblVDOP";
            this.lblVDOP.Size = new System.Drawing.Size(10, 13);
            this.lblVDOP.TabIndex = 11;
            this.lblVDOP.Text = "-";
            // 
            // lblVDOPLabel
            // 
            this.lblVDOPLabel.AutoSize = true;
            this.lblVDOPLabel.Location = new System.Drawing.Point(6, 111);
            this.lblVDOPLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblVDOPLabel.Name = "lblVDOPLabel";
            this.lblVDOPLabel.Size = new System.Drawing.Size(37, 13);
            this.lblVDOPLabel.TabIndex = 10;
            this.lblVDOPLabel.Text = "VDOP";
            // 
            // lblHDOP
            // 
            this.lblHDOP.AutoSize = true;
            this.lblHDOP.Location = new System.Drawing.Point(149, 93);
            this.lblHDOP.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblHDOP.Name = "lblHDOP";
            this.lblHDOP.Size = new System.Drawing.Size(10, 13);
            this.lblHDOP.TabIndex = 9;
            this.lblHDOP.Text = "-";
            // 
            // lblHDOPLabel
            // 
            this.lblHDOPLabel.AutoSize = true;
            this.lblHDOPLabel.Location = new System.Drawing.Point(6, 93);
            this.lblHDOPLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblHDOPLabel.Name = "lblHDOPLabel";
            this.lblHDOPLabel.Size = new System.Drawing.Size(38, 13);
            this.lblHDOPLabel.TabIndex = 8;
            this.lblHDOPLabel.Text = "HDOP";
            // 
            // lblGPSFix
            // 
            this.lblGPSFix.AutoSize = true;
            this.lblGPSFix.Location = new System.Drawing.Point(149, 37);
            this.lblGPSFix.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblGPSFix.Name = "lblGPSFix";
            this.lblGPSFix.Size = new System.Drawing.Size(10, 13);
            this.lblGPSFix.TabIndex = 5;
            this.lblGPSFix.Text = "-";
            // 
            // lblGPSFixLabel
            // 
            this.lblGPSFixLabel.AutoSize = true;
            this.lblGPSFixLabel.Location = new System.Drawing.Point(6, 39);
            this.lblGPSFixLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblGPSFixLabel.Name = "lblGPSFixLabel";
            this.lblGPSFixLabel.Size = new System.Drawing.Size(72, 13);
            this.lblGPSFixLabel.TabIndex = 4;
            this.lblGPSFixLabel.Text = "GPS Fix Type";
            // 
            // lblLongitude
            // 
            this.lblLongitude.AutoSize = true;
            this.lblLongitude.Location = new System.Drawing.Point(149, 75);
            this.lblLongitude.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblLongitude.Name = "lblLongitude";
            this.lblLongitude.Size = new System.Drawing.Size(10, 13);
            this.lblLongitude.TabIndex = 3;
            this.lblLongitude.Text = "-";
            // 
            // lblLongitudeLabel
            // 
            this.lblLongitudeLabel.AutoSize = true;
            this.lblLongitudeLabel.Location = new System.Drawing.Point(6, 75);
            this.lblLongitudeLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblLongitudeLabel.Name = "lblLongitudeLabel";
            this.lblLongitudeLabel.Size = new System.Drawing.Size(54, 13);
            this.lblLongitudeLabel.TabIndex = 2;
            this.lblLongitudeLabel.Text = "Longitude";
            // 
            // lblLatitude
            // 
            this.lblLatitude.AutoSize = true;
            this.lblLatitude.Location = new System.Drawing.Point(149, 57);
            this.lblLatitude.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblLatitude.Name = "lblLatitude";
            this.lblLatitude.Size = new System.Drawing.Size(10, 13);
            this.lblLatitude.TabIndex = 1;
            this.lblLatitude.Text = "-";
            // 
            // lblLatitudeLabel
            // 
            this.lblLatitudeLabel.AutoSize = true;
            this.lblLatitudeLabel.Location = new System.Drawing.Point(6, 57);
            this.lblLatitudeLabel.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblLatitudeLabel.Name = "lblLatitudeLabel";
            this.lblLatitudeLabel.Size = new System.Drawing.Size(45, 13);
            this.lblLatitudeLabel.TabIndex = 0;
            this.lblLatitudeLabel.Text = "Latitude";
            // 
            // statusStrip
            // 
            this.statusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsStatusLabel,
            this.tsStatus,
            this.tsLoading,
            this.tsFWVersionLabel,
            this.tsFWVersion,
            this.toolStripStatusLabel3,
            this.tsFCHearbeat,
            this.tsUniAdapterHeartbeat});
            this.statusStrip.Location = new System.Drawing.Point(0, 463);
            this.statusStrip.Name = "statusStrip";
            this.statusStrip.Size = new System.Drawing.Size(666, 24);
            this.statusStrip.TabIndex = 3;
            this.statusStrip.Text = "statusStrip1";
            // 
            // tsStatusLabel
            // 
            this.tsStatusLabel.Name = "tsStatusLabel";
            this.tsStatusLabel.Size = new System.Drawing.Size(42, 19);
            this.tsStatusLabel.Text = "Status:";
            // 
            // tsStatus
            // 
            this.tsStatus.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Bold);
            this.tsStatus.Name = "tsStatus";
            this.tsStatus.Size = new System.Drawing.Size(81, 19);
            this.tsStatus.Text = "disconnected";
            // 
            // tsLoading
            // 
            this.tsLoading.Name = "tsLoading";
            this.tsLoading.Size = new System.Drawing.Size(100, 18);
            this.tsLoading.Style = System.Windows.Forms.ProgressBarStyle.Marquee;
            // 
            // tsFWVersionLabel
            // 
            this.tsFWVersionLabel.Margin = new System.Windows.Forms.Padding(10, 3, 0, 2);
            this.tsFWVersionLabel.Name = "tsFWVersionLabel";
            this.tsFWVersionLabel.Size = new System.Drawing.Size(98, 19);
            this.tsFWVersionLabel.Text = "Firmware Version";
            // 
            // tsFWVersion
            // 
            this.tsFWVersion.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Bold);
            this.tsFWVersion.Name = "tsFWVersion";
            this.tsFWVersion.Size = new System.Drawing.Size(24, 19);
            this.tsFWVersion.Text = "1.1";
            // 
            // toolStripStatusLabel3
            // 
            this.toolStripStatusLabel3.Name = "toolStripStatusLabel3";
            this.toolStripStatusLabel3.Size = new System.Drawing.Size(246, 19);
            this.toolStripStatusLabel3.Spring = true;
            // 
            // tsFCHearbeat
            // 
            this.tsFCHearbeat.AutoSize = false;
            this.tsFCHearbeat.BorderSides = System.Windows.Forms.ToolStripStatusLabelBorderSides.Left;
            this.tsFCHearbeat.Margin = new System.Windows.Forms.Padding(5, 3, 0, 2);
            this.tsFCHearbeat.Name = "tsFCHearbeat";
            this.tsFCHearbeat.Size = new System.Drawing.Size(19, 19);
            this.tsFCHearbeat.ToolTipText = "FC Heartbeat";
            // 
            // tsUniAdapterHeartbeat
            // 
            this.tsUniAdapterHeartbeat.AutoSize = false;
            this.tsUniAdapterHeartbeat.BorderSides = System.Windows.Forms.ToolStripStatusLabelBorderSides.Left;
            this.tsUniAdapterHeartbeat.Margin = new System.Windows.Forms.Padding(5, 3, 0, 2);
            this.tsUniAdapterHeartbeat.Name = "tsUniAdapterHeartbeat";
            this.tsUniAdapterHeartbeat.Size = new System.Drawing.Size(19, 19);
            this.tsUniAdapterHeartbeat.ToolTipText = "UniAdapter Heartbeat";
            // 
            // lblPort1Label
            // 
            this.lblPort1Label.AutoSize = true;
            this.lblPort1Label.Location = new System.Drawing.Point(6, 24);
            this.lblPort1Label.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblPort1Label.Name = "lblPort1Label";
            this.lblPort1Label.Size = new System.Drawing.Size(35, 13);
            this.lblPort1Label.TabIndex = 8;
            this.lblPort1Label.Text = "Port 1";
            // 
            // lblPort1
            // 
            this.lblPort1.AutoSize = true;
            this.lblPort1.Location = new System.Drawing.Point(149, 24);
            this.lblPort1.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblPort1.Name = "lblPort1";
            this.lblPort1.Size = new System.Drawing.Size(10, 13);
            this.lblPort1.TabIndex = 9;
            this.lblPort1.Text = "-";
            // 
            // btnPort1Configure
            // 
            this.btnPort1Configure.Location = new System.Drawing.Point(224, 19);
            this.btnPort1Configure.Name = "btnPort1Configure";
            this.btnPort1Configure.Size = new System.Drawing.Size(88, 23);
            this.btnPort1Configure.TabIndex = 12;
            this.btnPort1Configure.Text = "Configuration";
            this.btnPort1Configure.UseVisualStyleBackColor = true;
            this.btnPort1Configure.Click += new System.EventHandler(this.btnPort1Configure_Click);
            // 
            // grpPorts
            // 
            this.grpPorts.Controls.Add(this.btnPort2Configure);
            this.grpPorts.Controls.Add(this.lblPort2);
            this.grpPorts.Controls.Add(this.btnPort1Configure);
            this.grpPorts.Controls.Add(this.lblPort2Label);
            this.grpPorts.Controls.Add(this.lblPort1);
            this.grpPorts.Controls.Add(this.lblPort1Label);
            this.grpPorts.Location = new System.Drawing.Point(334, 54);
            this.grpPorts.Name = "grpPorts";
            this.grpPorts.Size = new System.Drawing.Size(318, 84);
            this.grpPorts.TabIndex = 7;
            this.grpPorts.TabStop = false;
            this.grpPorts.Text = "Port Configuration";
            // 
            // btnPort2Configure
            // 
            this.btnPort2Configure.Location = new System.Drawing.Point(224, 48);
            this.btnPort2Configure.Name = "btnPort2Configure";
            this.btnPort2Configure.Size = new System.Drawing.Size(88, 23);
            this.btnPort2Configure.TabIndex = 12;
            this.btnPort2Configure.Text = "Configuration";
            this.btnPort2Configure.UseVisualStyleBackColor = true;
            this.btnPort2Configure.Click += new System.EventHandler(this.btnPort2Configure_Click);
            // 
            // lblPort2
            // 
            this.lblPort2.AutoSize = true;
            this.lblPort2.Location = new System.Drawing.Point(149, 53);
            this.lblPort2.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblPort2.Name = "lblPort2";
            this.lblPort2.Size = new System.Drawing.Size(10, 13);
            this.lblPort2.TabIndex = 9;
            this.lblPort2.Text = "-";
            // 
            // lblPort2Label
            // 
            this.lblPort2Label.AutoSize = true;
            this.lblPort2Label.Location = new System.Drawing.Point(6, 53);
            this.lblPort2Label.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.lblPort2Label.Name = "lblPort2Label";
            this.lblPort2Label.Size = new System.Drawing.Size(35, 13);
            this.lblPort2Label.TabIndex = 8;
            this.lblPort2Label.Text = "Port 2";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(666, 487);
            this.Controls.Add(this.grpPorts);
            this.Controls.Add(this.groupRC);
            this.Controls.Add(this.statusStrip);
            this.Controls.Add(this.gpGenerall);
            this.Controls.Add(this.grpChannelStats);
            this.Controls.Add(this.groupTelemetry);
            this.Controls.Add(this.pnlTop);
            this.Controls.Add(this.groupGPS);
            this.KeyPreview = true;
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(682, 525);
            this.MinimumSize = new System.Drawing.Size(682, 525);
            this.Name = "MainForm";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "fac";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MainForm_FormClosed);
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.MainForm_KeyUp);
            this.pnlTop.ResumeLayout(false);
            this.groupRC.ResumeLayout(false);
            this.groupRC.PerformLayout();
            this.grpChannelStats.ResumeLayout(false);
            this.grpChannelStats.PerformLayout();
            this.gpGenerall.ResumeLayout(false);
            this.gpGenerall.PerformLayout();
            this.groupTelemetry.ResumeLayout(false);
            this.groupTelemetry.PerformLayout();
            this.groupGPS.ResumeLayout(false);
            this.groupGPS.PerformLayout();
            this.statusStrip.ResumeLayout(false);
            this.statusStrip.PerformLayout();
            this.grpPorts.ResumeLayout(false);
            this.grpPorts.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.IO.Ports.SerialPort serialPort;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.Label lblPort;
        private System.Windows.Forms.Panel pnlTop;
        private System.Windows.Forms.ComboBox cmbPort;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.Button btnUpdate;
        private System.Windows.Forms.StatusStrip statusStrip;
        private System.Windows.Forms.ToolStripStatusLabel tsStatusLabel;
        private System.Windows.Forms.ToolStripStatusLabel tsStatus;
        private System.Windows.Forms.ToolStripProgressBar tsLoading;
        private System.Windows.Forms.ToolStripStatusLabel tsFWVersionLabel;
        private System.Windows.Forms.ToolStripStatusLabel tsFWVersion;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel3;
        private System.Windows.Forms.ToolStripStatusLabel tsFCHearbeat;
        private System.Windows.Forms.ToolStripStatusLabel tsUniAdapterHeartbeat;
        private System.Windows.Forms.GroupBox gpGenerall;
        private System.Windows.Forms.Label lblBattery;
        private System.Windows.Forms.Label lblBatteryLabel;
        private System.Windows.Forms.Label lblFlightMode;
        private System.Windows.Forms.Label lblFlightModeLabel;
        private System.Windows.Forms.GroupBox groupTelemetry;
        private System.Windows.Forms.GroupBox groupGPS;
        private System.Windows.Forms.Label lblLongitude;
        private System.Windows.Forms.Label lblLongitudeLabel;
        private System.Windows.Forms.Label lblLatitude;
        private System.Windows.Forms.Label lblLatitudeLabel;
        private System.Windows.Forms.Label lblGPSFix;
        private System.Windows.Forms.Label lblGPSFixLabel;
        private System.Windows.Forms.Label lblHDOP;
        private System.Windows.Forms.Label lblHDOPLabel;
        private System.Windows.Forms.Label lblAltitude;
        private System.Windows.Forms.Label lblAltitudeLabel;
        private System.Windows.Forms.Label lblCOG;
        private System.Windows.Forms.Label lblCOGLabel;
        private System.Windows.Forms.Label lblSpeed;
        private System.Windows.Forms.Label lblSpeedLabel;
        private System.Windows.Forms.Label lblVDOP;
        private System.Windows.Forms.Label lblVDOPLabel;
        private System.Windows.Forms.Label lblNumSat;
        private System.Windows.Forms.Label lblNumSatLabel;
        private System.Windows.Forms.GroupBox grpChannelStats;
        private System.Windows.Forms.Label lblStatCorruptedLabel;
        private System.Windows.Forms.Label lblStatCorrupted;
        private System.Windows.Forms.Label lblStatDropLabel;
        private System.Windows.Forms.Label lblStatDrop;
        private System.Windows.Forms.Label lblStatLostLabel;
        private System.Windows.Forms.Label lblStatLost;
        private System.Windows.Forms.Label lblClimb;
        private System.Windows.Forms.Label lblClimbLabel;
        private System.Windows.Forms.Label lblHeadingLabel;
        private System.Windows.Forms.Label lblHeading;
        private System.Windows.Forms.Label lblThrottle;
        private System.Windows.Forms.Label lblThrottleLabel;
        private System.Windows.Forms.Label lblPitch;
        private System.Windows.Forms.Label lblPitchLabel;
        private System.Windows.Forms.Label lblRoll;
        private System.Windows.Forms.Label lblRollLabel;
        private System.Windows.Forms.GroupBox groupRC;
        private System.Windows.Forms.Label lblRC8;
        private System.Windows.Forms.Label lblRC8Label;
        private System.Windows.Forms.Label lblRC7;
        private System.Windows.Forms.Label lblRC7Label;
        private System.Windows.Forms.Label lblRC6;
        private System.Windows.Forms.Label lblRC6Label;
        private System.Windows.Forms.Label lblRC5;
        private System.Windows.Forms.Label lblRC5Label;
        private System.Windows.Forms.Label lblRC2Label;
        private System.Windows.Forms.Label lblRC4;
        private System.Windows.Forms.Label lblRC2;
        private System.Windows.Forms.Label lblRC4Label;
        private System.Windows.Forms.Label lblRC1Label;
        private System.Windows.Forms.Label lblRC1;
        private System.Windows.Forms.Label lblRC3;
        private System.Windows.Forms.Label lblRuderLabel;
        private System.Windows.Forms.Label lblPort1Label;
        private System.Windows.Forms.Label lblPort1;
        private System.Windows.Forms.Button btnPort1Configure;
        private System.Windows.Forms.GroupBox grpPorts;
        private System.Windows.Forms.Button btnPort2Configure;
        private System.Windows.Forms.Label lblPort2;
        private System.Windows.Forms.Label lblPort2Label;
    }
}

