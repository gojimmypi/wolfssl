:: exit /b

cd C:\workspace\wolfssl-pr-cert\build-watcom

wlink name wolfssl.dll system nt_dll debug all option implib=wolfssl.lib ^
  file { ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\hmac.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\hash.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\cpuid.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\kdf.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\random.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\sha256.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\rsa.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\sp_int.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\aes.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\sha.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\sha512.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\logging.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\wc_port.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\error.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\wc_encrypt.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\signature.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\wolfmath.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\memory.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\dh.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\asn.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\coding.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\poly1305.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\pwdbased.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\pkcs12.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\chacha.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\chacha20_poly1305.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\integer.c.obj ^
    CMakeFiles\wolfssl.dir\wolfcrypt\src\ecc.c.obj ^
    CMakeFiles\wolfssl.dir\src\internal.c.obj ^
    CMakeFiles\wolfssl.dir\src\wolfio.c.obj ^
    CMakeFiles\wolfssl.dir\src\keys.c.obj ^
    CMakeFiles\wolfssl.dir\src\ssl.c.obj ^
    CMakeFiles\wolfssl.dir\src\ocsp.c.obj ^
    CMakeFiles\wolfssl.dir\src\tls.c.obj ^
    CMakeFiles\wolfssl.dir\src\tls13.c.obj ^
  } ^
  library ws2_32.lib ^
  library clib3r.lib

cd ..
