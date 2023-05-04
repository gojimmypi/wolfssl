#!/bin/bash

/mnt/c/workspace/openssl/apps/openssl asn1parse -in "./output/test_request.crt" | tee test_request.asn.txt

/mnt/c/workspace/openssl/apps/openssl x509 -req -outform der -in "./output/test_request.crt" -out "./output/test_request.der"

/mnt/c/workspace/openssl/apps/openssl x509 -req -in "./output/test_request.crt" -days 10 \
                      -CA "private/CertificateAuthority-4096-PublicKey.crt" \
                      -CAkey "private/CertificateAuthority-4096-PrivateKey.pem" \
                      -passin pass:secret -CAcreateserial \
                      -extfile "private/CertificateExtensions.txt" \
                      -extensions LumosDevice \
                      -out "./output/test-Device-Certificate_hw.crt" -text

/mnt/c/workspace/openssl/apps/openssl x509 -req -outform der -in "./output/test_request.crt" -out "./output/test_request.der"
