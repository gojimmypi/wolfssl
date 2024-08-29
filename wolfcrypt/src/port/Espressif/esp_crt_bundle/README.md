# wolfSSL Support for ESP-IDF Certificate Bundles

These files are typically only used when integrating wolfSSL with the ESP-IDF
and with the intention of using Certificate Bundles in the esp-tls component.

## Certificate Inspection

The certificates in the bundle are in PEM format. The [gen_crt_bundle.py script](./gen_crt_bundle.py)
converts them to DER format to load into the `x509_crt_imported_bundle_wolfssl_bin_start` binary
array.

To convert a PEM to DER from command-line:

```
MY_CERT_NAME=ISRG_ROOT_X1
openssl x509 -outform der -in "$MY_CERT_NAME".pem -out "$MY_CERT_NAME".der
```

To inspect a DER file:

```
openssl x509 -inform der -in "$MY_CERT_NAME".der -text -noout
```


## Known Problems

Espressif is using the well known https://www.howsmyssl.com/ in the
[examples](https://github.com/espressif/esp-idf/tree/master/examples/protocols/), for instance in
the [esp_http_client](https://github.com/espressif/esp-idf/tree/master/examples/protocols/esp_http_client).

It was recently observed that TLS 1.3 is _not_ currently configured properly on that web site.
See [howsmyssl #716](https://github.com/jmhodges/howsmyssl/issues/716).

As such, when configuring wolfSSL for _only_ TLS 1.3, a `fatal error -313` may occur.

To test if the `howsmyssl` web site has TLS 1.3 working, use the [wolfSSL client example](https://github.com/wolfSSL/wolfssl/tree/master/examples/client):

```bash
./examples/client/client -v 4 -h www.howsmyssl.com -p 443 -g -j
```

Or OpenSSL:

```bash
openssl s_client -connect www.howsmyssl.com:443 -tls1_3 -ciphersuites 'TLS_AES_256_GCM_SHA384'
```

Returns this unintuitive error:

```text
$ openssl s_client -connect www.howsmyssl.com:443 -tls1_3 -ciphersuites 'TLS_AES_256_GCM_SHA384'
CONNECTED(00000003)
4007DA6C617F0000:error:0A000410:SSL routines:ssl3_read_bytes:sslv3 alert handshake failure:../ssl/record/rec_layer_s3.c:1584:SSL alert number 40
---
no peer certificate available
---
No client certificate CA names sent
---
SSL handshake has read 7 bytes and written 247 bytes
Verification: OK
---
New, (NONE), Cipher is (NONE)
Secure Renegotiation IS NOT supported
Compression: NONE
Expansion: NONE
No ALPN negotiated
Early data was not sent
Verify return code: 0 (ok)
---
```



Or OpenSSL,

```bash
openssl s_client -tls1_3 -host www.howsmyssl.com -port 443
```
