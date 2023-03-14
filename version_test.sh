make clean
make distclean
./autogen.sh
./configure CC=clang --enable-trackmemory --enable-ed25519 --enable-smallstack --enable-all CFLAGS=-DHAVE_STACK_SIZE && make && ./wolfcrypt/test/testwolfcrypt
