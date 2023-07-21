#!/bin/bash
./scripts/dertoc.pl ./certs/sm2/client-sm2.der      client_sm2      client_sm2.c
./scripts/dertoc.pl ./certs/sm2/client-sm2-priv.der client_sm2_priv client_sm2_priv.c
./scripts/dertoc.pl ./certs/sm2/root-sm2.der        root_sm2        root_sm2.c

# Public Key (-c)
./scripts/dertoc.pl ./certs/sm2/server-sm2.der      server_sm2      server_sm2.c

# Private Key (-k)
./scripts/dertoc.pl ./certs/sm2/server-sm2-priv.der server_sm2_priv server_sm2_priv.c


openssl x509 -in ./certs/sm2/client-sm2.pem -outform der -out ./my-client-sm2.der
openssl ec -in ./certs/sm2/client-sm2-priv.pem -outform der -out ./my-client-sm2-priv.der

./scripts/dertoc.pl ./certs/sm2/client-sm2.pem      client_sm2      ./my_client_sm2.c
./scripts/dertoc.pl ./my-client-sm2-priv.der        client_sm2_priv ./my_client_sm2_priv.c

./scripts/dertoc.pl ./certs/sm2/client-sm2-priv.pem client_sm2_priv ./my_client_sm2_priv.c


./scripts/dertoc.pl ./certs/sm2/ca-sm2.der          ca_sm2      ./my_ca_sm2.c

./scripts/dertoc.pl  ./certs/sm2/server-sm2.pem       server_sm2      ./my_server-sm2_pem.c
./scripts/dertoc.pl  ./certs/sm2/server-sm2-priv.pem  server_sm2_priv ./my_server_sm2_priv_pem.c
./scripts/dertoc.pl  ./certs/sm2/client-sm2.pem       client_sm2      ./my_client_sm2_pem.c

./certs/sm2/root-sm2.pem

# ./examples/client/client -v 4 -l TLS13-SM4-GCM-SM3 \
#     -c ./certs/sm2/client-sm2.pem -k ./certs/sm2/client-sm2-priv.pem \
#     -A ./certs/sm2/root-sm2.pem -C

# ./examples/client/client  -h 192.168.1.108 -v 3 -l ECDHE-ECDSA-SM4-CBC-SM3     -c ./certs/sm2/client-sm2.pem -k ./certs/sm2/client-sm2-priv.pem     -A ./certs/sm2/root-sm2.pem -C





# ./examples/server/server                   -v 3 -l ECDHE-ECDSA-SM4-CBC-SM3     -c ./certs/sm2/server-sm2.pem -k ./certs/sm2/server-sm2-priv.pem     -A ./certs/sm2/client-sm2.pem -V
