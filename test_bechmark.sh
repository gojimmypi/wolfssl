#!/bin/bash

# sample init:
# git clone https://github.com/wolfSSL/wolfssl.git wolfssl_test
# cd wolfssl_test
# ./autogen.sh

function check_for_error
{
retVal=$1
if [ $retVal -ne 0 ]; then
    echo "Error"
    exit $retVal
fi
}

function test_benchmark # param #1 is CFLAGS value, parameter #2 is output file
{
    echo ./configure --enable-all --enable-jobserver CFLAGS="$1"
         ./configure --enable-all --enable-jobserver CFLAGS="$1"
    check_for_error $?

    echo "Running make..."
    make
    check_for_error $?

    echo Running becnhmark...
    ./wolfcrypt/benchmark/benchmark | tee "$2"
    check_for_error $?
}



test_benchmark "-DWOLFSSL_BENCHMARK_FIXED_UNITS_GB" "benchmark_gb_test.log"

test_benchmark "-DWOLFSSL_BENCHMARK_FIXED_UNITS_MB" "benchmark_mb_test.log"

test_benchmark "-DWOLFSSL_BENCHMARK_FIXED_UNITS_KB" "benchmark_kb_test.log"

test_benchmark "-DWOLFSSL_BENCHMARK_FIXED_UNITS_BB" "benchmark_bb_test.log"


test_benchmark "-DWOLFSSL_BENCHMARK_FIXED_UNITS_GB -DGENERATE_MACHINE_PARSEABLE_REPORT" "benchmark_parse_gb_test.log"

test_benchmark "-DWOLFSSL_BENCHMARK_FIXED_UNITS_MB -DGENERATE_MACHINE_PARSEABLE_REPORT" "benchmark_parse_mb_test.log"

test_benchmark "-DWOLFSSL_BENCHMARK_FIXED_UNITS_KB -DGENERATE_MACHINE_PARSEABLE_REPORT" "benchmark_parse_kb_test.log"

test_benchmark "-DWOLFSSL_BENCHMARK_FIXED_UNITS_BB -DGENERATE_MACHINE_PARSEABLE_REPORT" "benchmark_parse_bb_test.log"

exit

./configure --enable-all CFLAGS="-DWOLFSSL_BENCHMARK_FIXED_UNITS_G" && make
./wolfcrypt/benchmark/benchmark > benchmark_gb_test.log

./configure --enable-all CFLAGS="-DWOLFSSL_BENCHMARK_FIXED_UNITS_M" && make
./wolfcrypt/benchmark/benchmark > benchmark_mb_test.log

./configure --enable-all CFLAGS="-DWOLFSSL_BENCHMARK_FIXED_UNITS_K" && make
./wolfcrypt/benchmark/benchmark > benchmark_kb_test.log

./configure --enable-all CFLAGS="-DWOLFSSL_BENCHMARK_FIXED_UNITS_B" && make
./wolfcrypt/benchmark/benchmark > benchmark_bb_test.log


./configure --enable-all CFLAGS="-DWOLFSSL_BENCHMARK_FIXED_UNITS_G -DGENERATE_MACHINE_PARSEABLE_REPORT" && make
./wolfcrypt/benchmark/benchmark > benchmark_gb_test.log

./configure --enable-all CFLAGS="-DWOLFSSL_BENCHMARK_FIXED_UNITS_M -DGENERATE_MACHINE_PARSEABLE_REPORT" && make
./wolfcrypt/benchmark/benchmark > benchmark_mb_test.log

./configure --enable-all CFLAGS="-DWOLFSSL_BENCHMARK_FIXED_UNITS_K -DGENERATE_MACHINE_PARSEABLE_REPORT" && make
./wolfcrypt/benchmark/benchmark > benchmark_kb_test.log

./configure --enable-all CFLAGS="-DWOLFSSL_BENCHMARK_FIXED_UNITS_B -DGENERATE_MACHINE_PARSEABLE_REPORT" && make
./wolfcrypt/benchmark/benchmark > benchmark_bb_test.log


