
Original:
```
openssl x509 -req -in "Input/BLS-001 - ESP32s3 - no HW acceleration - new key - signing request.csr" -days 10   -CA "CertificateAuthority-4096-PublicKey.crt" -CAkey "CertificateAuthority-4096-PrivateKey.pem" -passin pass:secret -CAcreateserial   -extfile "CertificateExtensions.txt" -extensions LumosDevice   -out Output\BLS-001-Device-Certificate.crt -text

Certificate request self-signature ok
subject=C = NZ, L = Hamilton, O = "Blue Leaf Software, Ltd", CN = Lumos, serialNumber = BLS-001
```

Revised:
```
openssl x509 -req -in "Input/Sign existing certificate - ESP32s3.csr" -days 10   -CA "CertificateAuthority-4096-PublicKey.crt" -CAkey "CertificateAuthority-4096-PrivateKey.pem" -passin pass:secret -CAcreateserial   -extfile "CertificateExtensions.txt" -extensions LumosDevice   -out Output\test-Device-Certificate.crt -text
```

Testing success step 1:
```
openssl x509 -req -in "Input/BLS-001 - ESP32s3 - no HW acceleration - new key - signing request.csr" -days 10   -CA "private/CertificateAuthority-4096-PublicKey.crt" -CAkey "private/CertificateAuthority-4096-PrivateKey.pem" -passin pass:secret -CAcreateserial   -extfile "private/CertificateExtensions.txt" -extensions LumosDevice   -out Output\test-Device-Certificate.crt -text
```


Testing step 2:
```
openssl x509 -req -in "Input/BLS-001 - ESP32s3 - HW acceleration - New private key - Jim ED25519_SHA2_fix -signing request.csr" -days 10   -CA "private/CertificateAuthority-4096-PublicKey.crt" -CAkey "private/CertificateAuthority-4096-PrivateKey.pem" -passin pass:secret -CAcreateserial   -extfile "private/CertificateExtensions.txt" -extensions LumosDevice   -out Output\BLS-001-Device-Certificate.crt -text
```


NO Hardware Acceleration ESP32-S3
```
openssl x509 -req -in "Input/gojimmypi_private_rsa_generated_key_no_hw.txt" -days 10   -CA "private/CertificateAuthority-4096-PublicKey.crt" -CAkey "private/CertificateAuthority-4096-PrivateKey.pem" -passin pass:secret -CAcreateserial   -extfile "private/CertificateExtensions.txt" -extensions LumosDevice   -out Output\gojimmypi-Device-Certificate.crt -text
```

Hardware Acceleration ESP32-S3
```
openssl x509 -req -in "Input/gojimmypi_private_rsa_generated_key_hw.txt"    -days 10   -CA "private/CertificateAuthority-4096-PublicKey.crt" -CAkey "private/CertificateAuthority-4096-PrivateKey.pem" -passin pass:secret -CAcreateserial   -extfile "private/CertificateExtensions.txt" -extensions LumosDevice   -out Output\gojimmypi-Device-Certificate.crt -text140226713358976:error:0909006C:PEM routines:get_name:no start line:../crypto/pem/pem_lib.c:745:Expecting: CERTIFICATE REQUEST
```

```
openssl x509 -req -in "Input/BLS-001 - ESP32s3 - HW acceleration - New private key - Jim ED25519_SHA2_fix -signing request.csr" -days 10   -CA "private/CertificateAuthority-4096-PublicKey.crt" -CAkey "private/CertificateAuthority-4096-PrivateKey.pem" -passin pass:secret -CAcreateserial   -extfile "private/CertificateExtensions.txt" -extensions LumosDevice   -out Output\Test-Device-Certificate.crt -text
```
[Certificate Signing Request (CSR) generation with wolfSSL](https://www.wolfssl.com/certificate-signing-request-csr-generation-wolfssl/)

```
./configure --enable-certreq --enable-certgen
```