#!/bin/bash
./configure --enable-all CFLAGS="-DWOLFSSL_BENCHMARK_FIXED_BLOCKTYPE=KB -DWOLFSSL_BENCHMARK_FIXED_BASE2_BLOCKTYPE=KiB" && make && ./wolfcrypt/benchmark/benchmark
