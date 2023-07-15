#!/bin/bash
if [ "$1" == "HW" ] || [ "$1" == "SW" ]; then
    echo HW/SW: $1
else
    echo "Not a valid HW/SW value: $1"
    echo "example:  ./cert_peek.sh HW 2048 ESP32"
    exit 1
fi

if [ "$2" == "512" ] || [ "$2" == "1024" ] || [ "$2" == "2048" ]; then
    echo RSA size: $2
else
    echo "No a valid RSA size 512/1024/2048 value: $2"
    echo "example:  ./cert_peek.sh HW 2048 ESP32"
    exit 1
fi

if [ "$3" == "ESP32-S3" ] || [ "$3" == "ESP32" ] || [ "$3" == "ESP32-C3" ]; then
    echo Espressif chip: $3
else
    echo "Not a valid Espressif chip ESP32/ESP32-S3/ESP32-C3 value: $3"
    echo "example:  ./cert_peek.sh HW 2048 ESP32"
    exit 1
fi

echo ""
echo "Files:"
echo ""

ls ./output/gojimmypi-"$3"_"$1"_"$2"* -al
echo ""
openssl rsa -in "./output/gojimmypi-"$3"_"$1"_"$2"_private_key.txt" -noout -text

echo ""
echo "Request:"
echo ""

openssl req -in "./output/gojimmypi-"$3"_"$1"_"$2"_request.crt" -noout -text

echo ""
echo "CA sign:"
echo ""

openssl x509 -req -in "./output/gojimmypi-"$3"_"$1"_"$2"_request.crt" \
        -days 10                                                          \
        -CA "private/CertificateAuthority-4096-PublicKey.crt"             \
        -CAkey "private/CertificateAuthority-4096-PrivateKey.pem"         \
        -passin pass:secret -CAcreateserial                               \
        -extfile "private/CertificateExtensions.txt"                      \
        -extensions LumosDevice                                           \
        -out ./output/gojimmypi-Device-Certificate_"$3"_"$1"_"$2".crt -text
