
Building on the updates in [#5800](https://github.com/wolfSSL/wolfssl/pull/5800), this PR improves the 
[wolfSSL wolfcrypt benchmark utility](https://github.com/wolfSSL/wolfssl/tree/master/wolfcrypt/benchmark).


## Changes

- Fixes [#5814](https://github.com/wolfSSL/wolfssl/issues/5814) Compile-time error when defining `GENERATE_MACHINE_PARSEABLE_REPORT`.
- Add benchmark cycle counts for Espressif Xtensa architecture on ESP32 chips.
- Adds internal `__BENCHMARK_VALUE_TO_STRING(x)` and `__BENCHMARK_VALUE(x)` macros
- Extends the Espressif [xthal_get_ccount()](https://github.com/espressif/esp-idf/blob/f90c12fc3cc5abb47a7f1088ada43a21b31d0357/components/xtensa/include/xtensa/hal.h#L726) to be _single_-rollover tolerant. See `xthal_get_ccount_ex()`
- Allows for fixed-unit specification via the new `#define` options (in this order, if several supplied):
    `WOLFSSL_BENCHMARK_FIXED_UNITS_G` or `WOLFSSL_BENCHMARK_FIXED_UNITS_GB` for GB/GiB units
    `WOLFSSL_BENCHMARK_FIXED_UNITS_M` or `WOLFSSL_BENCHMARK_FIXED_UNITS_MB` for MB/MiB units 
    `WOLFSSL_BENCHMARK_FIXED_UNITS_K` or `WOLFSSL_BENCHMARK_FIXED_UNITS_KB` for KB/KiB units
    `WOLFSSL_BENCHMARK_FIXED_UNITS_B` or `WOLFSSL_BENCHMARK_FIXED_UNITS_BB` for Byte units

- Add support for Expressif Xtensa architecture for `GENERATE_MACHINE_PARSEABLE_REPORT` 
- Properly implement `HAVE_GET_CYCLES` for showing cycle counts on platforms that are known to have this capability.
- Change 'GENERATE_MACHINE_PARSEABLE_REPORT` no longer limits to 80 char lines.
- Define a new internal `__BENCHMARK_MAXIMUM_LINE_LENGTH` to define the maximum allowed line length of each metric. 
- The new maximum line length has been increased from 128 to 150. Also fixes missing LF on some large byte metrics when using `GENERATE_MACHINE_PARSEABLE_REPORT`.

## Review and testing of changes

Visual inspection: the "cycles" count should be approximately equal to the CPU clock frequency, given that most measurements are
for about 1 second duration.

This script may be helpful in testing various parameters of the `benchmark` utility.

```
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

    echo Running benchmark...
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

```

## Not addressed

- There are Intel and Xtensa counts, other platforms may be missing? See `END_INTEL_CYCLES` and `END_ESP_CYCLES` macros.

- The `printf` statements should eventually all be changed to lower-memory-utilization outputs for embedded devices, such as the Espressif `ESP_LOGI().`

- The output examples of turning on `GENERATE_MACHINE_PARSEABLE_REPORT` or not: these formats are similar and could possibly be improved:

```text
(off) RNG                        110 MiB took 1.007 seconds,  109.276 MiB/s Cycles per byte =  26.18
 (on) RNG                        100 MiB took 1.017 seconds,   98.286 MiB/s, 3052336385 cycles, Cycles per byte =  29.11
```

## Known issues

The units of GB are supported, but any resulting fractional numbers less than 1 are shown as zero.

New platforms may have a zero displayed for CPU cycle counts. See 

## Developers Tools:


Developers of ESP32 testing for other platforms may find this helpful.

As the application version of the `benchmark.c` file is in the `components` directory of the ESP-IDF,
it needs to be copied back to the local repo when testing host-based benchmark. In this case on WSL/Ubuntu on Windows 10
with the VisualGDB extension to Visual Studio:

```bash
#!/bin/bash

cp /mnt/c/SysGCC/esp32/esp-idf/v4.4.2/components/wolfssl/wolfcrypt/benchmark/benchmark.c wolfcrypt/benchmark/benchmark.c
make

 ./wolfcrypt/benchmark/benchmark

retVal=$1
if [ $retVal -ne 0 ]; then
    echo "Error"
    exit $retVal
fi
```
