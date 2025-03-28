using System;
using System.Linq;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using wolfSSL.CSharp;

namespace SmartDeviceProject1
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            Console.WriteLine("Testing wolfSSL C# wrapper...");

            try
            {
                // Call your wrapper initialization function
                // Replace 'WolfSSL' and 'Init' with your actual method/class
                if (wolfssl.Init() == wolfssl.SUCCESS)
                {
                    label1.Text += "wolfSSL initialized successfully.";
                    Console.WriteLine("wolfSSL initialized successfully.");
                }
                else
                {
                    label1.Text += "Failed to initialize wolfSSL.";
                    Console.WriteLine("Failed to initialize wolfSSL.");
                }

            }
            catch (Exception ex)
            {
                Console.WriteLine("Error initializing wolfSSL: " + ex.Message);
            }

        }
    }
}