

## Changes

- Fixes [#5814](https://github.com/wolfSSL/wolfssl/issues/5814) Compile-time error when defining `GENERATE_MACHINE_PARSEABLE_REPORT`.
- Add benchmark cycle counts for Espressif Xtensa architecture on ESP32 chips.
- Adds internal `__BENCHMARK_UNIT_VALUE_TO_STRING(x)` and `__BENCHMARK_UNIT_VALUE(x)` macros
- Extends the Espressif [xthal_get_ccount()](https://github.com/espressif/esp-idf/blob/f90c12fc3cc5abb47a7f1088ada43a21b31d0357/components/xtensa/include/xtensa/hal.h#L726) to be _single_-rollover tolerant. See `xthal_get_ccount_ex()`
- Allows for fixed-unit specification via the new `#define` options (in this order, if several supplied):
    `WOLFSSL_BENCHMARK_FIXED_UNITS_G` or `WOLFSSL_BENCHMARK_FIXED_UNITS_GB` for GB/GiB units
    `WOLFSSL_BENCHMARK_FIXED_UNITS_M` or `WOLFSSL_BENCHMARK_FIXED_UNITS_MB` for MB/MiB units 
    `WOLFSSL_BENCHMARK_FIXED_UNITS_K` or `WOLFSSL_BENCHMARK_FIXED_UNITS_KB` for KB/KiB units
    `WOLFSSL_BENCHMARK_FIXED_UNITS_B` or `WOLFSSL_BENCHMARK_FIXED_UNITS_BB` for Byte units

- Add support for Expressif Xtensa architecture for `GENERATE_MACHINE_PARSEABLE_REPORT` 
- Properly implement `HAVE_GET_CYCLES` for showing cycle counts on platforms that are known to have this capability.

## Review and testing of changes

Visual inspection: the "cycles" count should be approximately equal to the frequency given that most measurements are
for about 1 second duration.

## Not addressed:

There are Intel and Xtensa counts, other platforms may be missing? See `END_INTEL_CYCLES` and `END_ESP_CYCLES`
The `printf` statements should eventually all be changed to lower-memory-utilization outputs for embedded devices, such as the Espressif `ESP_LOGI().`

The output examples of turning on `GENERATE_MACHINE_PARSEABLE_REPORT` or not: these formats are similar and could possibly be improved:

```text
(off) RNG                        110 MiB took 1.007 seconds,  109.276 MiB/s Cycles per byte =  26.18
 (on) RNG                        100 MiB took 1.017 seconds,   98.286 MiB/s, 3052336385 cycles, Cycles per byte =  29.11
```