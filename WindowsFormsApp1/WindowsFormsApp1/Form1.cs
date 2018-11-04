using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.IO;
using MLApp;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        //private static Type matlabAppType = System.Type.GetTypeFromProgID("Matlab.Application");
        //private MLApp.MLApp matlab = System.Activator.CreateInstance(matlabAppType) as MLApp.MLApp;

        private static string baseDir = Environment.CurrentDirectory;

        private static byte[] result = new byte[1024];
        private bool connState = false;
        private Socket clientSock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

        private static Icon icon = new Icon(baseDir.Substring(0,baseDir.Length-9)+ "circle_control_pad.ico");
        private bool iconMoving = false;

        public Form1()
        {
            InitializeComponent();
        }

        private void btn_conn_Click(object sender, EventArgs e)
        {
            if (!connState)
            {
                clientSock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                string connString = textBox1.Text;
                IPAddress ip = IPAddress.Parse(connString.Split(':')[0]);
                int port = int.Parse(connString.Split(':')[1]);
                try
                {
                    clientSock.Connect(new IPEndPoint(ip, port));
                    clientSock.ReceiveTimeout = 200;
                    btn_conn.Text = "断开连接";
                    connState = true;
                }
                catch(Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
            else
            {
                clientSock.Close();
                btn_conn.Text = "连接";
                connState = false;
            }
        }

        private void pictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                iconMoving = true;
                timer1.Interval = int.Parse(tb_Interval.Text);
                timer1.Start();
            }
        }

        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if (iconMoving)
            {
                int x = e.Location.X;
                int y = e.Location.Y;
                if (!cb_x.Checked)
                {
                    x = pictureBox1.Width/2;
                }
                if (!cb_y.Checked)
                {
                    y = pictureBox1.Height/2;
                }
                reDraw(x, y);
                x = (int)((x - pictureBox1.Width / 2) * (60.0 / (pictureBox1.Width / 2)));
                y = (int)(-1*(y - pictureBox1.Height / 2) * (255.0 / (pictureBox1.Height / 2)));
                if (x > 60)
                {
                    x = 60;
                }
                else if (x < -60)
                {
                    x = -60;
                }
                if (y > 255)
                {
                    y = 255;
                }
                else if (y<-255)
                {
                    y = -255;
                }
                lb_x.Text = "x:" + x;
                lb_y.Text = "y:" + y;
            }
        }

        private void pictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            if (iconMoving)
            {
                string person = tb_addr.Text;
                clientSock.Send(Encoding.ASCII.GetBytes('@' + person + ' ' + "f000r000" + "\r\n"));
                iconMoving = false;
                reDraw(pictureBox1.Width / 2, pictureBox1.Height / 2);
                lb_x.Text = "x:" + 0;
                lb_y.Text = "y:" + 0;
                timer1.Stop();
            }
        }

        private void reDraw(int x,int y)
        {
            using (Graphics graphics = pictureBox1.CreateGraphics())
            {
                graphics.Clear(Color.Gray);
                graphics.DrawIcon(icon, x-icon.Width/2, y-icon.Height/2);
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (connState)
            {
                string cmd = "";
                int pwm = int.Parse(lb_y.Text.Split(':')[1]);
                int pos = int.Parse(lb_x.Text.Split(':')[1]);
                if (pwm < 0)
                {
                    cmd += 'b';
                }
                else
                {
                    cmd += 'f';
                }
                cmd += Math.Abs(pwm).ToString("000");
                cmd += 'r';
                cmd += pos.ToString();
                string person = tb_addr.Text;
                clientSock.Send(Encoding.ASCII.GetBytes('@' + person + ' ' + cmd + "\r\n"));
            }
        }

        private void btn_updateLb_Click(object sender, EventArgs e)
        {
            if (connState)
            {
                clientSock.Send(Encoding.ASCII.GetBytes(".list\r\n"));
                int receiveLength = clientSock.Receive(result);
                listBox1.Items.Clear();
                if (receiveLength > 0)
                {
                    string[] clients = Encoding.ASCII.GetString(result, 0, receiveLength).Split('\n');
                    for (int i = 0; i < clients.Length; i++)
                    {
                        listBox1.Items.Add(clients[i]);
                    }
                }
            }
        }

        private void btn_send_Click(object sender, EventArgs e)
        {
            string line = tb_send.Text;
            if (connState&&line.Length>0)
            {
                DateTime dt = DateTime.Now;
                string t = "我[" + dt.ToString() + "]:" + line;
                List<string> lines = richTextBox1.Lines.ToList();
                lines.Add(t);
                richTextBox1.Lines = lines.ToArray();
                tb_send.Text = "";
                try
                {
                    clientSock.Send(Encoding.ASCII.GetBytes(line + "\r\n"));
                    int receiveLength = clientSock.Receive(result);
                    if (receiveLength > 0)
                    {
                        t = "服务器[" + dt.ToString() + ']' + Encoding.ASCII.GetString(result, 0, receiveLength);
                        lines = richTextBox1.Lines.ToList();
                        lines.Add(t);
                        richTextBox1.Lines = lines.ToArray();
                    }
                }
                catch
                {

                }
            }
        }

        private void btn_clearTB_Click(object sender, EventArgs e)
        {
            richTextBox1.Clear();
        }

        private void btn_sendFile_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                string fn = openFileDialog1.FileName;
                string txt = File.ReadAllText(@fn);
                tb_send.Text += ' ' + txt;
            }
        }

        private void btn_mltest_Click(object sender, EventArgs e)
        {
            //matlab.visible = 0;
            //matlab.execute("cd('" + basedir + "')");
            //matlab.execute("clear all");
            //matlab.execute("agv_path");
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {

        }
    }
}
