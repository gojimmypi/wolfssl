#!/bin/bash
openssl asn1parse -in "./output/test_request.crt" | tee test_request.asn.txt

openssl x509 -req -outform der -in "./output/test_request.crt" -out "./output/test_request.der"

openssl x509 -req -in "./output/test_request.crt" -days 10 \
                      -CA "private/CertificateAuthority-4096-PublicKey.crt" \
                      -CAkey "private/CertificateAuthority-4096-PrivateKey.pem" \
                      -passin pass:secret -CAcreateserial \
                      -extfile "private/CertificateExtensions.txt" \
                      -extensions LumosDevice \
                      -out "./output/test-Device-Certificate_hw.crt" -text

openssl x509 -req -outform der -in "./output/test_request.crt" -out "./output/test_request.der"
