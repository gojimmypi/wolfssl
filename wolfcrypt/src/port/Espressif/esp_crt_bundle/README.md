# wolfSSL Support for ESP-IDF Certificate Bundles

These files are typically only used when integrating wolfSSL with the ESP-IDF
and with the intention of using Certificate Bundles in the esp-tls component.

# Certificate Inspection

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
