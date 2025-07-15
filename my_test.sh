#!/bin/bash

g++-12 --version

make distclean
./autogen.sh

./configure CFLAGS="-DWOLFSSL_DEBUG_ERRORS_ONLY"
make -j || { echo >&2 "Initial Build 1 failed"; exit 1; }

./configure CFLAGS="-DWOLF_NO_VARIADIC_MACROS -DWOLFSSL_DEBUG_CERTS"
make -j || { echo >&2 "Initial Build 2 failed"; exit 1; }

./configure CFLAGS="-DDEBUG_WOLFSSL -DWOLFSSL_DEBUG_CERTS"
make -j || { echo >&2 "Initial Build 3 failed"; exit 1; }

./configure CFLAGS="-DDEBUG_WOLFSSL -DNO_WOLFSSL_DEBUG_CERTS"
make -j || { echo >&2 "Initial Build 4 failed"; exit 1; }

./configure CFLAGS="-DWOLFSSL_DEBUG_CERTS"
make -j || { echo >&2 "Initial Build 5 failed"; exit 1; }


CC=g++-12
CFLAGS="-DTEST_ALWAYS_RUN_TO_END" \
CPPFLAGS="-DNO_WOLFSSL_CIPHER_SUITE_TEST -DWOLFSSL_OLD_PRIME_CHECK -Werror=literal-suffix"

./configure \
  --srcdir=. \
  --disable-jobserver \
  --enable-option-checking=fatal \
  --enable-all \
  --enable-testcert \
  --enable-acert \
  --enable-dtls13 \
  --enable-dtls-mtu \
  --enable-dtls-frag-ch \
  --enable-dtlscid \
  --enable-quic \
  --with-sys-crypto-policy \
  --enable-debug \
  --enable-debug-trace-errcodes \
  --enable-sp-math-all \
  --enable-experimental \
  --enable-kyber=yes,original \
  --enable-lms \
  --enable-xmss \
  --enable-dilithium \
  --enable-dual-alg-certs \
  --disable-qt \
  CC=g++-12 \
  CFLAGS="-DTEST_ALWAYS_RUN_TO_END" \
  CPPFLAGS="-DNO_WOLFSSL_CIPHER_SUITE_TEST -DWOLFSSL_OLD_PRIME_CHECK -Werror=literal-suffix" || exit 1


make -j || { echo >&2 "Build failed"; exit 1; }

./configure \
  --srcdir=. \
  --disable-jobserver \
  --enable-option-checking=fatal \
  --enable-c89 \
  --enable-all \
  --disable-all-osp \
  --disable-opensslall \
  --enable-cryptonly \
  --enable-testcert \
  --enable-acert \
  --disable-examples \
  --disable-crypttests \
  --disable-benchmark \
  CC=gcc \
  CFLAGS="-DTEST_ALWAYS_RUN_TO_END -std=c89 -Wno-overlength-strings -pedantic -Wdeclaration-after-statement -DTEST_LIBWOLFSSL_SOURCES_INCLUSION_SEQUENCE -Wvla -Wdeclaration-after-statement -Wconversion -DWOLF_NO_VARIADIC_MACROS -DXSNPRINTF=snprintf -D_ISOC99_SOURCE=1"

  make -j || { echo >&2 "Build 2 failed"; exit 1; }


  CC=g++-12 \
CFLAGS="-DTEST_ALWAYS_RUN_TO_END" \
CPPFLAGS="-DNO_WOLFSSL_CIPHER_SUITE_TEST -DWOLFSSL_OLD_PRIME_CHECK -Werror=literal-suffix" \
./configure \
  --srcdir=. \
  --disable-jobserver \
  --enable-option-checking=fatal \
  --enable-all \
  --enable-testcert \
  --enable-acert \
  --enable-dtls13 \
  --enable-dtls-mtu \
  --enable-dtls-frag-ch \
  --enable-dtlscid \
  --enable-quic \
  --with-sys-crypto-policy \
  --enable-debug \
  --enable-debug-trace-errcodes \
  --enable-sp-math-all \
  --enable-experimental \
  --enable-kyber=yes,original \
  --enable-lms \
  --enable-xmss \
  --enable-dilithium \
  --enable-dual-alg-certs \
  --disable-qt

  make -j || { echo >&2 "Build 3 failed"; exit 1; }

  ./configure --enable-all
  make -j || { echo >&2 "Build 4 failed"; exit 1; }

  ./configure --enable-all CPPFLAGS="-DWOLFSSL_DEBUG_CERTS"
  make -j || { echo >&2 "Build 5 failed"; exit 1; }


