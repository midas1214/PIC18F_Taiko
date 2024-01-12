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
            while (true);
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
            Console.WriteLine(data[0].ToString());
            // Simulate pressing the 'data' key
            SendKeys.SendWait(data[0].ToString());
            System.Threading.Thread.Sleep(100);
        }

    }
}