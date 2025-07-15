:: exit /b

cd C:\workspace\wolfssl-pr-cert\build-watcom

wlib -q -n wolfssl.lib ^
  +CMakeFiles\wolfssl.dir\wolfcrypt\src\*.obj ^
  +CMakeFiles\wolfssl.dir\src\*.obj

cd ..
