
## This is a staging experimental version!

When testing staging components, be sure to set `IDF_COMPONENT_REGISTRY_URL`:

```
export IDF_COMPONENT_REGISTRY_URL=https://components-staging.espressif.com
```

Set the value when not testing:

```
export IDF_COMPONENT_REGISTRY_URL=https://components.espressif.com
```

This is the Espressif Component Version of wolfSSL 5.6.4-Staging01-2da8811.3, 
Staging Release #12 (version ^5.6.4-Staging01-2da8811.3)

For questions or beta test of this library, please send a message to support@wolfssl.com

For details on how wolfSSL is published to the ESP Component Registry, see the
[INSTALL.md](https://github.com/wolfssl/wolfssl/tree/master/IDE/Espressif/component-manager/INSTALL.md) file.

See the [wolfSSL Manual](https://www.wolfssl.com/documentation/manuals/wolfssl/index.html).

The [wolfSSL embedded TLS library](https://www.wolfssl.com/products/wolfssl/) is a lightweight, portable, 
C-language-based SSL/TLS library targeted at IoT, embedded, and RTOS environments primarily because of its size, 
speed, and feature set. It works seamlessly in desktop, enterprise, and cloud environments as well. 
wolfSSL supports industry standards up to the current [TLS 1.3](https://www.wolfssl.com/tls13) and DTLS 1.3, 
is up to 20 times smaller than OpenSSL, offers a simple API, an OpenSSL compatibility layer,
OCSP and CRL support, is backed by the robust [wolfCrypt cryptography library](https://github.com/wolfssl/wolfssl/tree/master/wolfcrypt), 
and much more.

The CMVP has issued FIPS 140-2 Certificates #3389 and #2425 for the wolfCrypt Module developed by wolfSSL Inc.  
For more information, see our [FIPS FAQ](https://www.wolfssl.com/license/fips/) or contact fips@wolfssl.com.

# Getting Started

Check out the Examples on the right pane of the [wolfssl component page](https://components.espressif.com/components/wolfssl/wolfssl/).

Typically you need only 4 lines to run an example from scratch in the EDP-IDF environment:

```bash
. ~/esp/esp-idf/export.sh
idf.py create-project-from-example "gojimmypi/mywolfssl^5.6.4-Staging01-2da8811.3"
cd wolfssl_benchmark
idf.py -b 115200 flash monitor
```

or for VisualGDB:

```bash
. /mnt/c/SysGCC/esp32/esp-idf/v5.1/export.sh
idf.py create-project-from-example "gojimmypi/mywolfssl^5.6.4-Staging01-2da8811.3"
cd wolfssl_benchmark
idf.py -b 115200 flash monitor
```


### Espressif Component Notes

Here are some ESP Registry-specific details of the wolfssl component.

#### Component Name

The naming convention of the build-system name of a dependency installed by the component manager
is always `namespace__component`. The namespace for wolfSSL is `wolfssl`. The build-system name
is thus `wolfssl__wolfssl`. We'll soon be publishing `wolfssl__wolfssh`, `wolfssl__wolfmqtt` and more.

A project `cmakelists.txt` doesn't need to mention it at all when using wolfSSL as a managed component.


#### Component Manager

To check which version of the [Component Manager](https://docs.espressif.com/projects/idf-component-manager/en/latest/getting_started/index.html#checking-the-idf-component-manager-version) 
is currently available, use the command:

```
python -m idf_component_manager -h
```

The Component Manager should have been installed during the [installation of the ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/#installation). 
If your version of ESP-IDF doesn't come with the IDF Component Manager, 
you can [install it](https://docs.espressif.com/projects/idf-component-manager/en/latest/guides/updating_component_manager.html#installing-and-updating-the-idf-component-manager):

```
python -m pip install --upgrade idf-component-manager
```

For further details on the Espressif Component Manager, see the [idf-component-manager repo](https://github.com/espressif/idf-component-manager/).

#### Contact

Have a specific request or questions? We'd love to hear from you! Please contact us at 
[support@wolfssl.com](mailto:support@wolfssl.com?subject=Espressif%20Component%20Question) or 
[open an issue on GitHub](https://github.com/wolfSSL/wolfssl/issues/new/choose).

# Licensing and Support

wolfSSL (formerly known as CyaSSL) and wolfCrypt are either licensed for use
under the GPLv2 (or at your option any later version) or a standard commercial
license. For our users who cannot use wolfSSL under GPLv2
(or any later version), a commercial license to wolfSSL and wolfCrypt is
available. 

See the [LICENSE.txt](./LICENSE.txt), visit [wolfssl.com/license](https://www.wolfssl.com/license/),
contact us at [licensing@wolfssl.com](mailto:licensing@wolfssl.com?subject=Espressif%20Component%20License%20Question) 
or call +1 425 245 8247

View Commercial Support Options: [wolfssl.com/products/support-and-maintenance](https://www.wolfssl.com/products/support-and-maintenance/)
# wolfSSL Embedded SSL/TLS Library

The [wolfSSL embedded SSL library](https://www.wolfssl.com/products/wolfssl/) 
(formerly CyaSSL) is a lightweight SSL/TLS library written in ANSI C and
targeted for embedded, RTOS, and resource-constrained environments - primarily
because of its small size, speed, and feature set.  It is commonly used in
standard operating environments as well because of its royalty-free pricing
and excellent cross platform support. wolfSSL supports industry standards up
to the current [TLS 1.3](https://www.wolfssl.com/tls13) and DTLS 1.3, is up to
20 times smaller than OpenSSL, and offers progressive ciphers such as ChaCha20,
Curve25519, Blake2b and Post-Quantum TLS 1.3 groups. User benchmarking and
feedback reports dramatically better performance when using wolfSSL over
OpenSSL.

wolfSSL is powered by the wolfCrypt cryptography library. Two versions of
wolfCrypt have been FIPS 140-2 validated (Certificate #2425 and
certificate #3389). FIPS 140-3 validation is in progress. For additional
information, visit the [wolfCrypt FIPS FAQ](https://www.wolfssl.com/license/fips/)
or contact fips@wolfssl.com.

## Why Choose wolfSSL?

There are many reasons to choose wolfSSL as your embedded, desktop, mobile, or
enterprise SSL/TLS solution. Some of the top reasons include size (typical
footprint sizes range from 20-100 kB), support for the newest standards
(SSL 3.0, TLS 1.0, TLS 1.1, TLS 1.2, TLS 1.3, DTLS 1.0, DTLS 1.2, and DTLS 1.3),
current and progressive cipher support (including stream ciphers), multi-platform,
royalty free, and an OpenSSL compatibility API to ease porting into existing
applications which have previously used the OpenSSL package. For a complete
feature list, see [Chapter 4](https://www.wolfssl.com/docs/wolfssl-manual/ch4/)
of the wolfSSL manual.

## Notes, Please Read

### Note 1
wolfSSL as of 3.6.6 no longer enables SSLv3 by default.  wolfSSL also no longer
supports static key cipher suites with PSK, RSA, or ECDH. This means if you
plan to use TLS cipher suites you must enable DH (DH is on by default), or
enable ECC (ECC is on by default), or you must enable static key cipher suites
with one or more of the following defines:

```
WOLFSSL_STATIC_DH
WOLFSSL_STATIC_RSA
WOLFSSL_STATIC_PSK
```
Though static key cipher suites are deprecated and will be removed from future
versions of TLS.  They also lower your security by removing PFS.

When compiling `ssl.c`, wolfSSL will now issue a compiler error if no cipher
suites are available. You can remove this error by defining
`WOLFSSL_ALLOW_NO_SUITES` in the event that you desire that, i.e., you're
not using TLS cipher suites.

### Note 2
wolfSSL takes a different approach to certificate verification than OpenSSL
does. The default policy for the client is to verify the server, this means
that if you don't load CAs to verify the server you'll get a connect error,
no signer error to confirm failure (-188).

If you want to mimic OpenSSL behavior of having `SSL_connect` succeed even if
verifying the server fails and reducing security you can do this by calling:

```c
wolfSSL_CTX_set_verify(ctx, WOLFSSL_VERIFY_NONE, NULL);
```

before calling `wolfSSL_new();`. Though it's not recommended.

### Note 3
The enum values SHA, SHA256, SHA384, SHA512 are no longer available when
wolfSSL is built with `--enable-opensslextra` (`OPENSSL_EXTRA`) or with the
macro `NO_OLD_SHA_NAMES`. These names get mapped to the OpenSSL API for a
single call hash function. Instead the name `WC_SHA`, `WC_SHA256`, `WC_SHA384` and
`WC_SHA512` should be used for the enum name.


# wolfSSL Release 5.6.4 (Oct 30, 2023)

Release 5.6.4 has been developed according to wolfSSL's development and QA process (see link below) and successfully passed the quality criteria.
https://www.wolfssl.com/about/wolfssl-software-development-process-quality-assurance


NOTE: * --enable-heapmath is being deprecated and will be removed by 2024
      * Old CyaSSL/CtaoCrypt shim layer was removed in this release (5.6.4)


## Vulnerabilities

* [Medium] A fix was added, but still under review for completeness, for a Bleichenbacher style attack, leading to being able to decrypt a saved TLS connection and potentially forge a signature after probing with a large number of trial connections. This issue is around RSA decryption and affects static RSA cipher suites on the server side, which are not recommended to be used and are off by default. Static RSA cipher suites were also removed from the TLS 1.3 protocol and only present in TLS 1.2 and lower. All padding versions of RSA decrypt are affected since the code under review is outside of the padding processing. Information about the private keys is NOT compromised in affected code. It's recommended to disable static RSA cipher suites and update the version of wolfSSL used if using RSA private decryption alone outside of TLS. The fix is located in this pull request (https://github.com/wolfSSL/wolfssl/pull/6896)

## New Feature Additions

* DTLS 1.3 PQC: support fragmenting the second ClientHello message. This allows arbitrarily long keys to be used, opening up support for all PQC ciphersuites in DTLS 1.3.
* SM2/SM3/SM4: Chinese cipher support including TLS 1.3 and 1.2 cipher suites. SM2 SP implementation available.
* Ability to parse ASN1 only with SMIME_read_PKCS7
* Added support for MemUse Entropy on Windows
* Added Ada Bindings for wolfSSL
* Added a PEM example that converts to and from DER/PEM.
* Added LMS/HSS and XMSS/XMSS^MT wolfcrypt hooks, both normal and verify-only options.
* Added support for the AES EAX mode of operation
* Port for use with Hitch (https://github.com/varnish/hitch) added
* Add XTS API's to handle multiple sectors in new port to VeraCrypt

## Enhancements and Optimizations

* Turned on SNI by default on hosts with resources
* Improved support for Silicon Labs Simplicity Studio and the ERF32 Gecko SDK
* Thumb-2 and ARM32 Curve25519 and Ed25519 assembly have significantly improved performance.
* Thumb-2 AES assembly code added.
* Thumb-2 and ARM32 SP implementations of RSA, DH and ECC have significantly improved performance.
* Minor performance improvements to SP ECC for Intel x64.
* AES-XTS assembly code added for Intel x64, Aarch64 and ARM32.
* Added support for X963 KDFs to ECIES.
* Added 32-bit type only implementation of AES GMULT using tables.
* Add support for nginx version 1.25.0
* Add support for Kerberos version 5 1.21.1
* Check all CRL entries in case a single issuer has multiple CRL's loaded
* CRL verify the entire chain including loaded CA's
* Added example for building wolfSSL as an Apple universal binary framework using configure
* Sniffer tool now supports decrypting TLS sessions using secrets obtained from a SSLKEYLOGFILE
* Updates made for EBSNET port
* Update "--enable-jni" to include additional defines for expanded JNI support. Also includes JCE and JSSE builds under the single enable option now.

## Fixes

* Fixed error handling when decrypted pre-master secret is too long when using static RSA.
* Added a fix for keymod use with i.MX RT1170 CAAM blobs
* Added a fix for AES-GCM use with Petalinux Xilinx
* Fixed `wc_SignatureGenerate_ex` to not call verify twice
* Fixed wolfCrypt FIPS DLL on Win32
* Fixed TFM math library big-endian reading implementation when a zero length buffer is passed in.
* Fixed NO_CERT configurations to build correctly.
* Fixed ARM AES-GCM streaming assembly when –enable-opensslextra defined.
* Added modulus checks to heap math implementation of mp_exptmod().
* Fixed Windows assembly code to handle that certain XMM registers are non-volatile.
* Aarch64 SP ECC implementation of sp_256_mont_dbl_4 has the register list for the assembly code fixed to include all used registers.
* mp_sqrt_mod_prime fixed to limit the number of iterations of a loop to handle malicious non-prime values being passed in.
* Ignore session ID's shorter than 32 bytes instead of erroring out

For additional vulnerability information visit the vulnerability page at:
https://www.wolfssl.com/docs/security-vulnerabilities/

See INSTALL file for build instructions.
More info can be found on-line at: https://wolfssl.com/wolfSSL/Docs.html


# Resources

[wolfSSL Website](https://www.wolfssl.com/)

[wolfSSL Wiki](https://github.com/wolfSSL/wolfssl/wiki)

[FIPS 140-2/140-3 FAQ](https://wolfssl.com/license/fips)

[wolfSSL Documentation](https://wolfssl.com/wolfSSL/Docs.html)

[wolfSSL Manual](https://wolfssl.com/wolfSSL/Docs-wolfssl-manual-toc.html)

[wolfSSL API Reference](https://wolfssl.com/wolfSSL/Docs-wolfssl-manual-17-wolfssl-api-reference.html)

[wolfCrypt API Reference](https://wolfssl.com/wolfSSL/Docs-wolfssl-manual-18-wolfcrypt-api-reference.html)

[TLS 1.3](https://www.wolfssl.com/docs/tls13/)

[wolfSSL Vulnerabilities](https://www.wolfssl.com/docs/security-vulnerabilities/)

[Additional wolfSSL Examples](https://github.com/wolfssl/wolfssl-examples)