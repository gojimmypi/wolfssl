
TFM conditionally defines `mp_read_radix`:

```
#if defined(OPENSSL_EXTRA) || !defined(NO_DSA) || defined(HAVE_ECC)
    MP_API int mp_read_radix(mp_int* a, const char* str, int radix);
#endif
```

In a `user_settings.h` file, I have `NO_DSA` defined.

While compileing for the ESP8266, I see this error

```
test.c
/mnt/c/workspace/wolfssl-gojimmypi/wolfcrypt/test/test.c: In function 'mp_test_cmp':
/mnt/c/workspace/wolfssl-gojimmypi/wolfcrypt/test/test.c:55029:5: error: implicit declaration of function 'mp_read_radix'; did you mean 'mp_toradix'? [-Werror=implicit-function-declaration]
     mp_read_radix(b, "1234567890123456789", MP_RADIX_HEX);
     ^~~~~~~~~~~~~
     mp_toradix
cc1: some warnings being treated as errors
ninja: build stopped: subcommand failed.
ninja failed with exit code 1
```
