// This application reads the serial port until the string "*RDY*" is present
// After that, it reads all the bytes 320*240 of a new image sent by Arduino (sketch and instructions can be found in
//   http://www.instructables.com/id/OV7670-Without-FIFO-Very-Simple-Framecapture-With-/?ALLSTEPS)
// 
// By: cesarab

using System;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.IO.Ports;
using System.Threading;
using System.Windows.Forms;

namespace ReadSerialPortWin
{
    public partial class frmPrinc : Form
    {
        private const int HEIGHT = 320;
        private const int WIDTH = 240;

        private Size _bitmapSize = new Size(WIDTH, HEIGHT);

        private SerialPort _mySerialPort { get; set; }

        private Bitmap _bitmap;
        public Bitmap MyBitmap
        {
            get
            {
                if (_bitmap == null)
                {
                    _bitmap = new Bitmap(_bitmapSize.Width, _bitmapSize.Height);
                }
                return _bitmap;
            }
        }


        public bool ReadingImage { get; set; }

        public frmPrinc()
        {
            InitializeComponent();
        }

        private bool CheckSerialPorts()
        {
            var names = SerialPort.GetPortNames();
            if (names.Length > 0)
            {
                cboSerialPorts.Items.Clear();
                foreach (var name in names)
                {
                    cboSerialPorts.Items.Add(name);
                }
                cboSerialPorts.SelectedItem = names[0];
                return true;
            }

            return false;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            if (!CheckSerialPorts())
            {
                btnStart.Enabled = false;
                lblStatus.Text = "Nothing connected to serial ports";
            }
          
            picImage.Image = MyBitmap;
        }

        private void LblStatus(string text)
        {
            lblStatus.Invoke((MethodInvoker)(() =>
            {
                lblStatus.Text = text;
            }));
        }

        private void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                var sw = new Stopwatch();
                sw.Start();

                var sp = (SerialPort) sender;

                var buffer = new byte[5];
                if (!sp.IsOpen)
                    return;
                var bytesRead = sp.Read(buffer, 0, buffer.Length);
                if (bytesRead > 4 && buffer[0] == '*' && buffer[1] == 'R' && buffer[2] == 'D' && buffer[3] == 'Y' &&
                    buffer[4] == '*')
                {
                    LblStatus("Found *RDY*");

                    LblStatus($"Reading image from serial port {cboSerialPorts.SelectedText}...");
                    ReadingImage = true;
                    Application.DoEvents();

                    for (var n2 = 0; n2 < WIDTH; ++n2)
                    {
                        for (var n3 = HEIGHT - 1; n3 >= 0; --n3)
                        {
                            if (!sp.IsOpen)
                                return;

                            var byte1 = sp.ReadByte();

                            var blue = byte1;
                            var green = byte1;
                            var red = byte1;

                            MyBitmap.SetPixel(n2, n3, Color.FromArgb(1, red, green, blue));
                        }
                    }
                    using (var memoryStream = new MemoryStream())
                    {
                        MyBitmap.Save(memoryStream, ImageFormat.Bmp);

                        picImage.Invoke((MethodInvoker) (() =>
                        {
                            picImage.Image = Image.FromStream(memoryStream);
                        }));
                    }

                    sw.Stop();
                    LblStatus($"Image was read. Time taken: {sw.ElapsedMilliseconds} ms");
                    Thread.Sleep(500);
                    Application.DoEvents();
                    if (!btnStop.Enabled)
                        CloseSerialPort();
                }
                else
                {
                    sp.ReadExisting();
                }
            }
            catch (Exception ex)
            {
                LblStatus($"Error: {ex.Message}");
            }
            finally
            {
                ReadingImage = false;
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            CloseSerialPort();
        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            _mySerialPort = new SerialPort(cboSerialPorts.SelectedItem.ToString())
            {
                BaudRate = 1000000,
                Parity = Parity.None,
                StopBits = StopBits.One,
                DataBits = 8,
                Handshake = Handshake.None,
                ReadTimeout = 3000
            };

            _mySerialPort.DataReceived += DataReceivedHandler;
            lblStatus.Text = $"Opening {cboSerialPorts.SelectedItem} port...";
            _mySerialPort.Open();
            lblStatus.Text = $"{cboSerialPorts.SelectedItem} port opened";
            btnStart.Enabled = false;
            btnStop.Enabled = true;
            btnStop.Focus();
        }

        private void CloseSerialPort()
        {
            if (_mySerialPort != null && _mySerialPort.IsOpen)
                _mySerialPort.Close();
            lblStatus.Text = $"{cboSerialPorts.SelectedItem} port closed";
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            btnStart.Enabled = true;
            btnStop.Enabled = false;
            btnStart.Focus();
            if (!ReadingImage) CloseSerialPort();
        }

        private void btnCheckPorts_Click(object sender, EventArgs e)
        {
            if (CheckSerialPorts())
            {
                btnStart.Enabled = true;
            }
            else
            {
                btnStart.Enabled = false;
            }
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            if (dlgSaveFile.ShowDialog(this) == DialogResult.OK)
            {
                MyBitmap.Save(dlgSaveFile.FileName, ImageFormat.Bmp);
            }
        }
    }
}
