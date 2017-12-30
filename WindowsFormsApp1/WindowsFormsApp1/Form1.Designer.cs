namespace WindowsFormsApp1
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.btn_conn = new System.Windows.Forms.Button();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.lb_y = new System.Windows.Forms.Label();
            this.lb_x = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.label2 = new System.Windows.Forms.Label();
            this.tb_Interval = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.cb_y = new System.Windows.Forms.CheckBox();
            this.cb_x = new System.Windows.Forms.CheckBox();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.btn_updateLb = new System.Windows.Forms.Button();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.label3 = new System.Windows.Forms.Label();
            this.tb_addr = new System.Windows.Forms.TextBox();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // textBox1
            // 
            this.textBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox1.Location = new System.Drawing.Point(174, 12);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(218, 21);
            this.textBox1.TabIndex = 0;
            this.textBox1.Text = "192.168.31.164:5005";
            // 
            // btn_conn
            // 
            this.btn_conn.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_conn.Location = new System.Drawing.Point(398, 10);
            this.btn_conn.Name = "btn_conn";
            this.btn_conn.Size = new System.Drawing.Size(86, 23);
            this.btn_conn.TabIndex = 1;
            this.btn_conn.Text = "连接";
            this.btn_conn.UseVisualStyleBackColor = true;
            this.btn_conn.Click += new System.EventHandler(this.btn_conn_Click);
            // 
            // listBox1
            // 
            this.listBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.listBox1.FormattingEnabled = true;
            this.listBox1.ItemHeight = 12;
            this.listBox1.Location = new System.Drawing.Point(12, 12);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(147, 340);
            this.listBox1.TabIndex = 2;
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Location = new System.Drawing.Point(174, 39);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(314, 353);
            this.tabControl1.TabIndex = 3;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.tb_addr);
            this.tabPage1.Controls.Add(this.label3);
            this.tabPage1.Controls.Add(this.lb_y);
            this.tabPage1.Controls.Add(this.lb_x);
            this.tabPage1.Controls.Add(this.pictureBox1);
            this.tabPage1.Controls.Add(this.label2);
            this.tabPage1.Controls.Add(this.tb_Interval);
            this.tabPage1.Controls.Add(this.label1);
            this.tabPage1.Controls.Add(this.cb_y);
            this.tabPage1.Controls.Add(this.cb_x);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(306, 327);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "遥控器";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // lb_y
            // 
            this.lb_y.AutoSize = true;
            this.lb_y.Location = new System.Drawing.Point(53, 26);
            this.lb_y.Name = "lb_y";
            this.lb_y.Size = new System.Drawing.Size(17, 12);
            this.lb_y.TabIndex = 7;
            this.lb_y.Text = "y:";
            // 
            // lb_x
            // 
            this.lb_x.AutoSize = true;
            this.lb_x.Location = new System.Drawing.Point(6, 26);
            this.lb_x.Name = "lb_x";
            this.lb_x.Size = new System.Drawing.Size(17, 12);
            this.lb_x.TabIndex = 6;
            this.lb_x.Text = "x:";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureBox1.Location = new System.Drawing.Point(7, 50);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(290, 271);
            this.pictureBox1.TabIndex = 5;
            this.pictureBox1.TabStop = false;
            this.pictureBox1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseDown);
            this.pictureBox1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseMove);
            this.pictureBox1.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseUp);
            // 
            // label2
            // 
            this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(268, 7);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 12);
            this.label2.TabIndex = 4;
            this.label2.Text = "毫秒";
            // 
            // tb_Interval
            // 
            this.tb_Interval.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_Interval.Location = new System.Drawing.Point(178, 2);
            this.tb_Interval.Name = "tb_Interval";
            this.tb_Interval.Size = new System.Drawing.Size(84, 21);
            this.tb_Interval.TabIndex = 3;
            this.tb_Interval.Text = "100";
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(117, 7);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(65, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "发送间隔：";
            // 
            // cb_y
            // 
            this.cb_y.AutoSize = true;
            this.cb_y.Checked = true;
            this.cb_y.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cb_y.Location = new System.Drawing.Point(55, 7);
            this.cb_y.Name = "cb_y";
            this.cb_y.Size = new System.Drawing.Size(42, 16);
            this.cb_y.TabIndex = 1;
            this.cb_y.Text = "y轴";
            this.cb_y.UseVisualStyleBackColor = true;
            // 
            // cb_x
            // 
            this.cb_x.AutoSize = true;
            this.cb_x.Checked = true;
            this.cb_x.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cb_x.Location = new System.Drawing.Point(7, 7);
            this.cb_x.Name = "cb_x";
            this.cb_x.Size = new System.Drawing.Size(42, 16);
            this.cb_x.TabIndex = 0;
            this.cb_x.Text = "x轴";
            this.cb_x.UseVisualStyleBackColor = true;
            // 
            // tabPage2
            // 
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(306, 275);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "控制台";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // btn_updateLb
            // 
            this.btn_updateLb.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btn_updateLb.Location = new System.Drawing.Point(12, 369);
            this.btn_updateLb.Name = "btn_updateLb";
            this.btn_updateLb.Size = new System.Drawing.Size(147, 23);
            this.btn_updateLb.TabIndex = 4;
            this.btn_updateLb.Text = "刷新";
            this.btn_updateLb.UseVisualStyleBackColor = true;
            this.btn_updateLb.Click += new System.EventHandler(this.btn_updateLb_Click);
            // 
            // timer1
            // 
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(141, 29);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(41, 12);
            this.label3.TabIndex = 8;
            this.label3.Text = "地址：";
            // 
            // tb_addr
            // 
            this.tb_addr.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_addr.Location = new System.Drawing.Point(178, 26);
            this.tb_addr.Name = "tb_addr";
            this.tb_addr.Size = new System.Drawing.Size(119, 21);
            this.tb_addr.TabIndex = 9;
            this.tb_addr.Text = "192.168.31.136";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(501, 405);
            this.Controls.Add(this.btn_updateLb);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.listBox1);
            this.Controls.Add(this.btn_conn);
            this.Controls.Add(this.textBox1);
            this.MinimumSize = new System.Drawing.Size(500, 300);
            this.Name = "Form1";
            this.Text = "遥控器";
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button btn_conn;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox tb_Interval;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox cb_y;
        private System.Windows.Forms.CheckBox cb_x;
        private System.Windows.Forms.Button btn_updateLb;
        private System.Windows.Forms.Label lb_y;
        private System.Windows.Forms.Label lb_x;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.TextBox tb_addr;
        private System.Windows.Forms.Label label3;
    }
}

