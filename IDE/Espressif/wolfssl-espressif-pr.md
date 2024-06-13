# Improving wolfSSL integration with the Espressif ESP-IDF

This is an anchor GutHub issue to track the various upcoming pull requests and other issues related to improving the [wolfSSL](https://www.wolfssl.com/) cryptographic library integration with the Espressif ESP-IDF.

The wolfSSL libraries are available for a given project, but the integrations with the ESP-IDF core such as ESP-TLS and Supplicant are not as robust as it should be.

There's a companion issue at Espressif. TBA

## Reasons for choosing wolfSSL

For serious commercial applications needing or users simply needing more capable, flexible, and actively supported libraries.

wolfSSL is a TLS library. wolfSSL offers optimal performance, rapid integration, hardware crypto support and support for the most current standards. wolfSSL is the best tested crypto support, the #1 TLS in IoT and the first embedded TLS 1.3 platform with TPM 2.0, MQTT, FIPS 140 certification, hardware crypto acceleration and secure enclave support. All products are backed by 24/7 support.  

|                   | **wolfSSL**  | **mbed TLS**    |
| ----------------- | ------------ | --------------- |
| **TECHNOLOGY**                                     |
| Copyright         | wolfSSL Inc. | Multiple Owners | 
| Development Team  | Original developers still on project | Based on XySSL/PolarSSL, not maintained by the original developers |
| Portability       | "Portable Out of the Box <br /> Win32/64, Linux, OS X, Solaris, ThreadX, VxWorks, FreeBSD, NetBSD, OpenBSD, embedded Linux, Haiku, OpenWRT, iPhone (iOS), Android, Nintendo Wii and Gamecube through DevKitPro, QNX, MontaVista, OpenCL, NonStop, TRON/ITRON/µITRON, Micrium's µC OS, FreeRTOS, SafeRTOS, Freescale MQX, Nucleus, TinyOS, HP/UX, Keil RTX, TI-RTOS, Integrity OS" | Win32/64, Linux, OS X, Solaris, FreeBSD, NetBSD, OpenBSD, OpenWRT, iPhone (iOS), Xbox, Android, SeggerOS |
| Standards Support | SSLv3 - TLS 1.3, DTLS 1.0,1.2, 1.3 | TLS 1.2/TLS 1.3 and DTLS 1.2 |
| Server Support | YES | YES |
| Performance | Awesome! See our benchmarks page: https://www.wolfssl.com/docs/benchmarks/ | Average |
| Hardware & Assembly Optimizations | - ARM Assembly Optimizations (Aarch32/Aarch64/Arm32/Cortex-M/Neon) <br >- ARMv8 Cryptography Extensions<br >- RISC-V Assembly<br >- STM32 F2/F4/F7/L4/L5/U5/H5/H7 Hardware Crypto<br >- ATECC608B, ST-SAFEA110, SE050, IoT-Safe<br >- Single Precision Math (C and Assembly) | Some ARM optimizations |
| Command Line Utility | YES | NO |
| Certifications | YES (FIPS 140-3, DO-178 DAL-A)  | NO |
| Certificate Revocation Support | CRL, OCSP, OCSP Stapling | CRL |
| Crypto Library Abstraction Layer | YES | NO | 
| SSL Inspection (Sniffer) Support | YES | NO |
| Compression Support | zlib NO |
| OpenSSL Compatibility Layer | YES (Actively updated - over 1,600) | YES (Out of date) |
| Post Quantum Support | Kyber, LMS, XMSS and Dilithum/Falcon| NO |
| Supported Open Source Projects | OpenSSH, Stunnel, WPA Supplicant, lighttpd (lighty), cURL, mongoose, OpenVPN, NGINX and many others | | 
| Quality Assurance Testing | API Tests, Peer Review, Static Analysis, Product Specific Testing, Multiple Compilers, Benchmarks, Wrappers, Hardware Accelerated Testing, Security fuzzers (wolfSSL internal fuzzer, AFL, TLS Fuzzer, libFuzzer), known user configurations, external validation, big/little endian, multiple platforms (Embedded IOT Devices, Windows, Many Linux variants, MacOS, XCODE, Android) | Broken scripts |
| **SUPPORT    DOCUMENTATION    LICENSING**  | 
| Documentation | YES <br />(complete manual, API reference, build instructions, extensions reference, tutorials, source code, benchmarking, examples) | PARTIAL (build instructions, API reference, source code) | 
| Vulnerabilities Fixes available within a few days Fixes available few months or not at all
| License Dual (GPLv2 / Commercial) |  Dual (GPLv2 / Apache 2.0) | 
| Royalty Free | YES | YES | 
| Up to 24x7 Support |  YES (Full support from native English speakers via email, phone, forums) | NO | 
| **FEATURES** | 
| Random Entropy | wolfRAND, NIST DRBG (SHA-256) | DRBG SHA-1/SHA2-256 | 
| Hashing/Cipher Functions |  AES SIV/CFB/OFB, SHAKE, Blake2b/Blake2s, ECIES (ECC Enc/Dec) | NO | 
| Public Key Options | Single Precision math, ECC Fixed Point cache ECC NIST | "modulo p" speedups | 
| TLS Extensions | SNI, Max Fragment, ALPN, Trusted CA Indication, Truncated HMAC, Secure Renegotiation, Renegotiation Indication, Session Ticket, Extended Master Secret, Encrypt-Then-Mac, Quantum-Safe Hybrid Authentication |  Max Fragment, Encrypt-Then-Mac | 


## Getting Started with wolfSSL

If you are new to wolfSSL on the Espressif ESP32, [this video](https://www.youtube.com/watch?v=CzwA3ZBZBZ8)
can help to get started:

[![Video Preview](https://img.youtube.com/vi/CzwA3ZBZBZ8/0.jpg)](https://www.youtube.com/watch?v=CzwA3ZBZBZ8)

Additional ESP-IDF specifics can be found in [Espressif/ESP-IDF](./ESP-IDF/README.md). The [wolfSSL Manual](https://www.wolfssl.com/documentation/manuals/wolfssl/index.html) is also a useful
resource.

The core Espressif IDE information for wolfSSL can be found here:

https://github.com/wolfSSL/wolfssl/tree/master/IDE/Espressif

Included are the following [examples](https://github.com/wolfSSL/wolfssl/tree/master/IDE/Espressif/ESP-IDF/examples/README.md):

* Bare-bones [Template](https://github.com/wolfSSL/wolfssl/tree/master/IDE/Espressif/ESP-IDF/examples/template/README.md)
* Simple [TLS Client](https://github.com/wolfSSL/wolfssl/tree/master/IDE/Espressif/ESP-IDF/examples/wolfssl_client/README.md) / [TLS Server](https://github.com/wolfSSL/wolfssl/tree/master/IDE/Espressif/ESP-IDF/examples/wolfssl_server/README.md)
* Cryptographic [Test](https://github.com/wolfSSL/wolfssl/tree/master/IDE/Espressif/ESP-IDF/examples/wolfssl_test/README.md)
* Cryptographic [Benchmark](/examples/wolfssl_benchmark/README.md)


 
## Managed Components

The wolfSSL libraries are already available as [Espressif Managed Components](https://docs.espressif.com/projects/idf-component-manager/en/latest/) from the [ESP Registry](https://docs.espressif.com/projects/idf-component-manager/en/latest/) for installation to a specific project.

* [release](https://components.espressif.com/components?q=namespace:wolfssl)
 - `wolfSSL` [wolfssl/wolfssl](https://components.espressif.com/components/wolfssl/wolfssl/)
 - `wolfSSH` [wolfssl/wolfssh](https://components.espressif.com/components/wolfssl/wolfssh/)
 - `wolfMQTT` [wolfssl/wolfmqtt](https://components.espressif.com/components/wolfssl/wolfmqtt/)
 - `wolfTPM` wolfssl/wolftpm` (coming soon, see https://github.com/wolfSSL/wolfTPM/pull/351)

[staging/test](https://components-staging.espressif.com/components?q=namespace:gojimmypi) Managed Components at the Espressif Component Registry.
 - `wolfSSL` [gojimmypi/mywolfssl](https://components-staging.espressif.com/components/gojimmypi/mywolfssl/)
 - `wolfSSH` [gojimmypi/mywolfssh](https://components-staging.espressif.com/components/gojimmypi/mywolfssh/)
 - `wolfMQTT` [gojimmypi/mywolfmqtt](https://components-staging.espressif.com/components/gojimmypi/mywolfmqtt/)
 - `wolfTPM` wolfssl/wolftpm (coming soon, see https://github.com/wolfSSL/wolfTPM/pull/351)

For details on [wolfSSL Managed Components](https://components.espressif.com/components?q=namespace:wolfssl),  see  these blogs:

- https://www.wolfssl.com/wolfssl-now-available-in-espressif-component-registry/
- https://www.wolfssl.com/wolfmqtt-now-available-as-an-espressif-managed-component-includes-aws-iot-mqtt-example/
- https://www.wolfssl.com/wolfssh-now-available-as-an-espressif-managed-component-includes-ssh-echo-server-example/


## PlatformIO

## Arduino

## wolfSSL for the Apple HomeKit on the ESP32

See the https://github.com/AchimPieters/esp32-homekit-demo

https://github.com/AchimPieters/esp32-homekit-demo/pull/3

## Additional wolfSSL updates related to the Espressif environment

See [ESP32 Espressif Improvements - Roadmap Summary #6234](https://github.com/wolfSSL/wolfssl/issues/6234)
