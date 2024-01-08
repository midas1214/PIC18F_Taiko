using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace UART_Keypress
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            this.KeyPress += TestForm_KeyPress;

            // UART settings
            SerialPort mySerialPort = new SerialPort("COM4");
            mySerialPort.BaudRate = 1200;
            mySerialPort.Parity = Parity.None;
            mySerialPort.StopBits = StopBits.One;
            mySerialPort.DataBits = 8;
            mySerialPort.Handshake = Handshake.None;
            mySerialPort.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);
            mySerialPort.Open();

            Console.WriteLine("UART Initalized...");
            while (true)
            {
                //System.Threading.Thread.Sleep(1000);
                //Console.WriteLine(Cursor.Position);
                //Console.WriteLine(GetColorFromScreen(Cursor.Position));
                //Point pos = new Point(510, 343);
                //Color c = GetColorFromScreen(pos);
                //if (c.R == 0 && c.G == 0 && c.B == 0)
                //{
                //    continue;
                //}
                //else if (c.R == 255 && c.G == 255 && c.B == 255)
                //{
                //    Console.WriteLine("okay");
                //}
                //else if (c.R > c.B)
                //{
                //    Console.WriteLine("nice");
                //}
                //else if (c.B > c.R)
                //{
                //    Console.WriteLine("not okay");
                //}
                //System.Threading.Thread.Sleep(10);
                //Console.WriteLine(GetColorFromScreen(new Point(-1386, 343)));
            }
        }

        private void TestForm_KeyPress(object sender, KeyPressEventArgs e)
        {
            // Display the pressed key in the console
            Console.WriteLine($"Key Pressed: {e.KeyChar}");
        }
        private static void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;
            string data = sp.ReadExisting();

            SimulateKeypress(data);
        }

        private static void SimulateKeypress(string data)
        {
            Console.Write("Data Received:");
            Console.WriteLine(data);
            // Simulate pressing the 'data' key
            SendKeys.SendWait(data);
        }

        public Bitmap CaptureFromScreen(Rectangle rect)
        {
            Bitmap bmpScreenCapture = null;

            if (rect == Rectangle.Empty)//capture the whole screen
            {
                rect = Screen.PrimaryScreen.Bounds;
            }

            bmpScreenCapture = new Bitmap(rect.Width, rect.Height);
            // Set the desired resolution
            int targetWidth = 1920; // Your desired width
            int targetHeight = 1080; // Your desired height

            // Resize the image to the desired resolution
            Bitmap bmpScreenCaptureResized = ResizeImage(bmpScreenCapture, targetWidth, targetHeight);


            Graphics p = Graphics.FromImage(bmpScreenCaptureResized);


            p.CopyFromScreen(rect.X,
                     rect.Y,
                     0, 0,
                     rect.Size,
                     CopyPixelOperation.SourceCopy);


            p.Dispose();

            return bmpScreenCaptureResized;
        }

        public Color GetColorFromScreen(Point p)
        {
            Rectangle rect = new Rectangle(p, new Size(2, 2));

            Bitmap map = CaptureFromScreen(rect);

            Color c = map.GetPixel(0, 0);

            map.Dispose();

            return c;
        }

        // Function to resize the image
        static Bitmap ResizeImage(Bitmap image, int width, int height)
        {
            Bitmap resizedImage = new Bitmap(width, height);
            using (Graphics g = Graphics.FromImage(resizedImage))
            {
                g.DrawImage(image, 0, 0, width, height);
            }
            return resizedImage;
        }
    }
}
