/* wolfSSL-DTLS-PSK-Client.cs
 *
 * Copyright (C) 2006-2025 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */




using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.IO;
using System.Net;
using System.Net.Sockets;
using wolfSSL.CSharp;



public class wolfSSL_DTLS_PSK_Client
{
    public const int SERVER_PORT = 2256; /* 11111 */
    public const int LOOPS = 100000;
    public const bool IS_BENCHMARK = true;
    public const string USE_CIPHERS = "DHE-PSK-AES128-CBC-SHA256";
    private static DateTime start_time;
    private static DateTime stop_time;

    /// <summary>
    /// Example of a PSK function call back
    /// </summary>
    /// <param name="ssl">pointer to ssl structure</param>
    /// <param name="hint">hint if any from connecting</param>
    /// <param name="identity">identity to send to server</param>
    /// <param name="id_max">max length of identity</param>
    /// <param name="key">buffer to hold key</param>
    /// <param name="max_key">max key size</param>
    /// <returns>size of key set</returns>
    public static uint my_psk_client_cb(IntPtr ssl, string hint, IntPtr identity, uint id_max, IntPtr key, uint max_key)
    {
        Console.WriteLine("Hello my_psk_client_cb!");
        /* C# client */
        byte[] id = { 67, 35, 32, 99, 108, 105, 101, 110, 116 };
        if (id_max < 9)
            return 0;
        Marshal.Copy(id, 0, identity, 9);

        /* Use desired key, note must be a key smaller than max key size parameter
            Replace this with desired key. Is trivial one for testing */
        if (max_key < 4)
            return 0;
        byte[] tmp = { 26, 43, 60, 77 };
        Marshal.Copy(tmp, 0, key, 4);

        return (uint)4;
    }


    private static void clean(IntPtr ssl, IntPtr ctx)
    {
        wolfssl.free(ssl);
        wolfssl.CTX_free(ctx);
        wolfssl.Cleanup();
    }


    public static void Main(string[] args)
    {
        IntPtr ctx;
        IntPtr ssl;
        Socket udp;
        StringBuilder dhparam;

        Console.WriteLine("Begin DTLS PSK Client");

        wolfssl.psk_client_delegate psk_cb = new wolfssl.psk_client_delegate(my_psk_client_cb);

        Console.WriteLine("Current path: " + Environment.CurrentDirectory);
        dhparam = new StringBuilder(wolfssl.setPath("dh2048.pem"));
        dhparam = new StringBuilder(Path.GetFullPath(dhparam.ToString()));
        if (dhparam.Length == 0) {
            Console.WriteLine("Platform not supported");
            return;
        }
        Console.WriteLine("Using cert: " + dhparam.ToString());

        StringBuilder buff = new StringBuilder(1024);
        StringBuilder reply = new StringBuilder("Hello, this is the wolfSSL C# client psk wrapper");

        wolfssl.Init();

        Console.WriteLine("Calling ctx Init from wolfSSL");
        ctx = wolfssl.CTX_new(wolfssl.useDTLSv1_2_client());
        if (ctx == IntPtr.Zero)
        {
            Console.WriteLine("Error creating ctx structure");
            return;
        }
        Console.WriteLine("Finished init of ctx .... now load in cert and key");

        StringBuilder ciphers = new StringBuilder(new String(' ', 4096));
        wolfssl.get_ciphers(ciphers, 4096);
        Console.WriteLine("Ciphers : " + ciphers.ToString());

        short minDhKey = 128;
        wolfssl.CTX_SetMinDhKey_Sz(ctx, minDhKey);
        Console.Write("Setting cipher suite to ");

        /* In order to use static PSK build wolfSSL with the preprocessor flag WOLFSSL_STATIC_PSK */
        StringBuilder set_cipher = new StringBuilder(USE_CIPHERS);
        Console.WriteLine(set_cipher);
        if (wolfssl.CTX_set_cipher_list(ctx, set_cipher) != wolfssl.SUCCESS) {
            Console.WriteLine("Failed to set cipher suite");
            return;
        }

        /* Test psk use with DHE */
        wolfssl.CTX_set_psk_client_callback(ctx, psk_cb);

        /* set up UDP socket */
        udp = new Socket(AddressFamily.InterNetwork, SocketType.Dgram,
                              ProtocolType.Udp);

        try {
            udp.Connect("localhost", SERVER_PORT);
        }
        catch (Exception e) {
            Console.WriteLine("udp.Connect() error " + e.ToString());
            wolfssl.CTX_free(ctx);
            return;
        }
        if (!udp.Connected) {
            Console.WriteLine("udp.Connect() failed!");
            udp.Close();
            wolfssl.CTX_free(ctx);
            return;
        }

        Console.WriteLine("Connected UDP");
        ssl = wolfssl.new_ssl(ctx);
        if (ssl == IntPtr.Zero) {
            Console.WriteLine("Error in creating ssl object");
            wolfssl.CTX_free(ctx);
            return;
        }

        if (wolfssl.set_fd(ssl, udp) != wolfssl.SUCCESS) {
            /* get and print out the error */
            Console.WriteLine(wolfssl.get_error(ssl));
            udp.Close();
            clean(ssl, ctx);
            return;
        }

        if (!File.Exists(dhparam.ToString())) {
            Console.WriteLine("Current path: " + Environment.CurrentDirectory);
            Console.WriteLine("Could not find dh file: " + dhparam);
            wolfssl.CTX_free(ctx);
            return;
        }

        wolfssl.SetTmpDH_file(ssl, dhparam, wolfssl.SSL_FILETYPE_PEM);

        if (wolfssl.connect(ssl) != wolfssl.SUCCESS) {
            /* get and print out the error */
            Console.WriteLine(wolfssl.get_error(ssl));
            udp.Close();
            clean(ssl, ctx);
            return;
        }

        /* print out results of DTLS/SSL accept */
        Console.WriteLine("SSL version is " + wolfssl.get_version(ssl));
        Console.WriteLine("SSL cipher suite is " + wolfssl.get_current_cipher(ssl));

        int loop = LOOPS;
        if (IS_BENCHMARK) {
            Console.WriteLine("Benchmark for " + loop.ToString() + " DTLS packets");
            start_time = DateTime.Now;
        }
        do {
            if (wolfssl.write(ssl, reply, reply.Length) != reply.Length) {
                Console.WriteLine("Error in write");
                udp.Close();
                clean(ssl, ctx);
                return;
            }

            /* read and print out the message then reply */
            if (wolfssl.read(ssl, buff, 1023) < 0) {
                Console.WriteLine("Error in read");
                udp.Close();
                clean(ssl, ctx);
                return;
            }

            if (buff.Length > 0) {
                if (!IS_BENCHMARK) {
                    Console.WriteLine("Received a packet! Loop #" + loop.ToString() + buff);
                }
                loop--;
            }
        } while (loop > 0);

        if (IS_BENCHMARK) {
            stop_time = DateTime.Now;
            Console.WriteLine("Duration: " + (stop_time - start_time).TotalMilliseconds
                              + " ms; Packets = "
                              + (LOOPS - loop).ToString());
        }

        Console.WriteLine(buff);

        wolfssl.shutdown(ssl);
        udp.Close();
        clean(ssl, ctx);
    }
}
