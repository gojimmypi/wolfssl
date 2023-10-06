#!/bin/bash

# zd16451: SM example for ZD16451 work by Jim:
#
# ./zd16451 contains SM certs in the wolfssl-master-sm directory
# ./mycerts contains non-SM certs in the wolfssl-master directory

# this file is located in /mnt/c/workspace/wolfssl-master-sm/zd16451
# this is wolfSSL master with the SM ciphers installed and enabled
# git clone https://github.com/wolfSSL/wolfssl.git wolfssl-master-sm

# install SM ciphers from wolfsm
# cd /mnt/c/workspace/wolfsm-gojimmypi
# ./install.sh /mnt/c/workspace/wolfssl-master-sm

# go back to wolfssl master that was the target of SM install and build
# cd /mnt/c/workspace/wolfssl-master-sm
# ./autogen.sh
# ./configure --enable-sm2 --enable-sm3 --enable-sm4-gcm --enable-sm4-cbc --enable-sm4-ccm
# make

# non SM ciphers:
# wolfssl-master  directory
# ./autogen.sh
# ./configure --enable-all

# from wolfssl root:
# cd zd16451

# Create the necessary directories
mkdir -p ./demoCA/private
mkdir -p ./demoCA/newcerts

# Initialize the index.txt file
cat /dev/null > ./demoCA/index.txt

# Set the serial number to "01"
echo "01" > ./demoCA/serial

echo -e "\n 1. Generate the CA key (SM2)"
openssl ecparam -genkey -name SM2 -outform pem -out ./demoCA/private/cakey.pem

echo -e "\n 2. Create a CSR for the CA"
  openssl req -new -sm3 -key ./demoCA/private/cakey.pem -out ec_ca.csr -subj '/C=US/ST=California/L=Demo/O=gojimmypi/OU=dev/CN=mycomputer/emailAddress=gojimmypi@gmail.com'
# openssl req -new      -key ./demoCA/private/cakey.pem -out ec_ca.csr -subj '/C=US/ST=California/L=Demo/O=gojimmypi/OU=dev/CN=mycomputer/emailAddress=gojimmypi@gmail.com'

echo -e "\n 3. Self-sign the CA certificate"
openssl x509 -req -days 3650 -in ec_ca.csr -signkey ./demoCA/private/cakey.pem -out ./demoCA/cacert.pem

echo -e "\n 4. Remove the CSR file"
rm -rf ec_ca.csr

echo -e "\n 5. Generate the client key (SM2)"
openssl ecparam -genkey -name SM2 -outform pem -out ./demoCA/private/clientkey.pem

echo -e "\n 6. Create a CSR for the client"
  openssl req -new -sm3 -key ./demoCA/private/clientkey.pem -out clientkey.csr -subj '/C=US/ST=California/L=Demo/O=gojimmypi/OU=dev2/CN=mycomputer/emailAddress=gojimmypi@gmail.com'
# openssl req -new      -key ./demoCA/private/clientkey.pem -out clientkey.csr -subj '/C=US/ST=California/L=Demo/O=gojimmypi/OU=dev2/CN=mycomputer/emailAddress=gojimmypi@gmail.com'

echo -e "\n 7. Sign the client certificate using the CA"
openssl ca -in clientkey.csr -out clientcert.pem -days 365

echo -e "\n 8. Remove the CSR file"
rm -rf clientkey.csr

echo -e "\n 9. Generate the server key (SM2)"
openssl ecparam -genkey -name SM2 -outform pem -out ./demoCA/private/serverkey.pem

echo -e "\n 10. Create a CSR for the server"
  openssl req -new -sm3 -key ./demoCA/private/serverkey.pem -out serverkey.csr -subj '/C=US/ST=California/L=Demo/O=gojimmypi/OU=dev3/CN=mycomputer/emailAddress=gojimmypi@gmail.com'
# openssl req -new      -key ./demoCA/private/serverkey.pem -out serverkey.csr -subj '/C=US/ST=California/L=Demo/O=gojimmypi/OU=dev3/CN=mycomputer/emailAddress=gojimmypi@gmail.com'

echo -e "\n 11. Sign the server certificate using the CA"
openssl ca -in serverkey.csr -out servercert.pem -days 365

# Remove the CSR file
rm -rf serverkey.csr


# Run client (from separate prompt, same diretory)
# ./examples/client/client  -h 127.0.0.1 -v 4 -l TLS13-SM4-CCM-SM3 -c ./zd16451/clientcert.pem -k ./zd16451/demoCA/private/clientkey.pem -A ./zd16451/demoCA/cacert.pem -C
# Fails for SM:
# wolfSSL_connect error -188, ASN no signer error to confirm failure
# wolfSSL error: wolfSSL_connect failed

# echo -e "run client in another window from same directory (copy & paste): \n\n ./examples/client/client  -h 127.0.0.1 -v 4 -l TLS13-AES128-GCM-SHA256 -c ./mycerts/clientcert.pem -k ./mycerts/demoCA/private/clientkey.pem -A ./mycerts/demoCA/cacert.pem -C"
  echo -e "run client in another window from same directory (copy & paste): \n\n ./examples/client/client  -h 127.0.0.1 -v 4 -l TLS13-SM4-CCM-SM3         -c ./zd16451/clientcert.pem -k ./zd16451/demoCA/private/clientkey.pem -A ./zd16451/demoCA/cacert.pem -C"
# Success
# Alternate cert chain used
#  issuer : /C=US/ST=California/L=Demo/O=gojimmypi/OU=dev/CN=mycomputer/emailAddress=gojimmypi@gmail.com
#  subject: /C=US/ST=California/O=gojimmypi/OU=dev3/CN=mycomputer/emailAddress=gojimmypi@gmail.com
#  serial number:02
# SSL version is TLSv1.3
# SSL cipher suite is TLS13-AES128-GCM-SHA256
# SSL curve name is SECP256R1

# Run server:
cd ..
./examples/server/server -v 4 -b -d -p 11111 -c ./zd16451/servercert.pem -k ./zd16451/demoCA/private/serverkey.pem -A ./zd16451/clientcert.pem -V
# Fails for SM:
# SSL_accept error -313, received alert fatal error
# wolfSSL error: SSL_accept failed
#
# cd ..
# ./examples/server/server -v 4 -b -d -p 11111 -c ./mycerts/servercert.pem -k ./mycerts/demoCA/private/serverkey.pem -A ./mycerts/clientcert.pem -V
# Success:
# Early Data was not sent.
# peer has no cert!
# SSL version is TLSv1.3
# SSL cipher suite is TLS13-AES128-GCM-SHA256
# SSL signature algorithm is (null)
# SSL curve name is SECP256R1
# Server Random : 6F3174AA799E11FBA6E14688C3CC8DD8BF8C89E0CFF5F5B5DDA5179CFC8ED164
# Client message: hello wolfssl!


# Run client (from separate prompt, same diretory)
# ./examples/client/client  -h 127.0.0.1 -v 4 -l TLS13-SM4-CCM-SM3 -c ./zd16451/clientcert.pem -k ./zd16451/demoCA/private/clientkey.pem -A ./zd16451/demoCA/cacert.pem -C

# wolfSSL_connect error -188, ASN no signer error to confirm failure
# wolfSSL error: wolfSSL_connect failed

# Relax params, same error:

# Run server:
# ./examples/server/server -b -d -p 11111 -c ./zd16451/servercert.pem -k ./zd16451/demoCA/private/serverkey.pem -A ./zd16451/clientcert.pem -V

# Run client (from separate prompt, same diretory)
# ./examples/client/client  -h 127.0.0.1  -c ./zd16451/clientcert.pem -k ./zd16451/demoCA/private/clientkey.pem -A ./zd16451/demoCA/cacert.pem -C


# Our Cert Server (success):
# ./examples/server/server -v 4 -b -d -p 11111 -c ./certs/sm2/server-sm2.pem -k ./certs/sm2/server-sm2-priv.pem -A ./certs/sm2/client-sm2.pem -V
# SSL version is TLSv1.3
# SSL cipher suite is TLS_SM4_CCM_SM3
# SSL curve name is SM2P256V1
# Client message: hello wolfssl!

# Our Cert Client (success):
# ./examples/client/client  -h 127.0.0.1 -v 4 -l TLS13-SM4-CCM-SM3 -c ./certs/sm2/client-sm2.pem -k ./certs/sm2/client-sm2-priv.pem -A ./certs/sm2/root-sm2.pem -C
# SSL version is TLSv1.3
# SSL cipher suite is TLS_SM4_CCM_SM3
# SSL curve name is SM2P256V1
# I hear you fa shizzle!

