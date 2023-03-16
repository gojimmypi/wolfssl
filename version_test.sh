make clean
make distclean
./autogen.sh
#./configure CC=clang --enable-trackmemory --enable-ed25519 --enable-smallstack --enable-all CFLAGS=-DHAVE_STACK_SIZE && make && ./wolfcrypt/test/testwolfcrypt
./configure CC=clang --enable-trackmemory --enable-ed25519 --disable-version-extended-info CFLAGS=-DHAVE_STACK_SIZE && make && ./wolfcrypt/test/testwolfcrypt
#./configure CC=clang --enable-trackmemory --enable-ed25519 --enable-version-extended-info CFLAGS=-DHAVE_STACK_SIZE && make && ./wolfcrypt/test/testwolfcrypt

echo "nm ./src/.libs/libwolfssl_la-version.o"
nm ./src/.libs/libwolfssl_la-version.o
