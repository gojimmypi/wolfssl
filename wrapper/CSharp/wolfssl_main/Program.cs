using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using wolfSSL.CSharp;

namespace wolfssl_main
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Testing wolfSSL C# wrapper...");

            try
            {
                // Call your wrapper initialization function
                // Replace 'WolfSSL' and 'Init' with your actual method/class
                if (wolfssl.Init() == wolfssl.SUCCESS)
                {
                    Console.WriteLine("wolfSSL initialized successfully.");
                }

            }
            catch (Exception ex)
            {
                Console.WriteLine("Error initializing wolfSSL: " + ex.Message);
            }

            Console.WriteLine("Press any key to exit...");
            Console.ReadKey();
        }
    }
}
