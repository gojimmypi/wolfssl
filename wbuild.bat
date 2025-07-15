@echo off
REM Set this to wherever you installed Open Watcom
set WATCOM=C:\watcom
set PATH=%WATCOM%\binnt;%WATCOM%\binw;%PATH%
set INCLUDE=%WATCOM%\h;%WATCOM%\h\nt
set EDPATH=%WATCOM%\eddat
set WIPFC=%WATCOM%\wipfc
set LIB=%WATCOM%\lib386;%WATCOM%\lib386\nt

goto test2

echo "Config #1"
rmdir /s /q .\build-watcom
cmake -B build-watcom -G "Watcom WMake" ^
  -DCMAKE_SYSTEM_NAME=Windows ^
  -DCMAKE_SYSTEM_PROCESSOR=x86 ^
  -DCMAKE_C_COMPILER=wcl386 ^
  -DCMAKE_C_FLAGS="-bt=nt -mf -zp=1 -5 -dWIN32" ^
  -DCMAKE_EXE_LINKER_FLAGS="system nt" ^
  -DCMAKE_VERBOSE_MAKEFILE=TRUE ^
  -DWOLFSSL_ASM=NO ^
  -DWOLFSSL_EXAMPLES=NO ^
  -DWOLFSSL_CRYPT_TESTS=NO ^
  -DWOLFSSL_SINGLE_THREADED=YES ^
  -DBUILD_SHARED_LIBS=NO ^
  -DCMAKE_POLICY_DEFAULT_CMP0136=NEW ^
  -DCMAKE_WATCOM_RUNTIME_LIBRARY=SingleThreaded
if errorlevel 1 (
    echo "Config 1 failed!"
    exit /b
)
cmake --build build-watcom || (echo "Build 1 failed!" && exit /b)
call wlink_dll.bat


:test2
echo "Config #2"
rmdir /s /q .\build-watcom
cmake -B build-watcom -G "Watcom WMake" ^
  -DCMAKE_SYSTEM_NAME=Windows ^
  -DWOLFSSL_DLL=NO ^
  -DCMAKE_EXECUTABLE_SUFFIX=.exe ^
  -DCMAKE_SYSTEM_PROCESSOR=i386 ^
  -DCMAKE_VERBOSE_MAKEFILE=TRUE ^
  -DWOLFSSL_ASM=NO ^
  -DWOLFSSL_EXAMPLES=YES ^
  -DWOLFSSL_CRYPT_TESTS=NO ^
  -DWOLFSSL_SINGLE_THREADED=YES ^
  -DBUILD_SHARED_LIBS=NO ^
  -DCMAKE_POLICY_DEFAULT_CMP0136=NEW ^
  -DCMAKE_WATCOM_RUNTIME_LIBRARY=SingleThreaded
if errorlevel 1 (
    echo "Config 2 failed!"
    exit /b
)
cmake --build build-watcom --target client || (echo "Build 2 failed!" && exit /b)
call wlink_lib.bat
call wlink_dll.bat

timeout /t 5 >nul

build-watcom\examples\client\client.exe -v 4 -h localhost -p 12345


exit /b





echo "Config #2a"
rmdir /s /q .\build-watcom
cmake -B build-watcom -G "Watcom WMake" ^
  -DWOLFSSL_DLL=NO ^
  -DWOLFSSL_DEBUG_CERTS=YES ^
  -DCMAKE_SYSTEM_NAME=Windows ^
  -DCMAKE_SYSTEM_PROCESSOR=x86 ^
  -DCMAKE_VERBOSE_MAKEFILE=TRUE ^
  -DWOLFSSL_ASM=NO ^
  -DWOLFSSL_EXAMPLES=YES ^
  -DWOLFSSL_CRYPT_TESTS=NO ^
  -DWOLFSSL_SINGLE_THREADED=YES ^
  -DBUILD_SHARED_LIBS=NO ^
  -DCMAKE_POLICY_DEFAULT_CMP0136=NEW ^
  -DCMAKE_WATCOM_RUNTIME_LIBRARY=SingleThreaded
if errorlevel 1 (
    echo "Config 2a failed!"
    exit /b
)
cmake --build build-watcom || (echo "Build 2a failed!" && exit /b)
call wlink.bat
exit /b

echo "Config #3"
rmdir /s /q .\build-watcom
cmake -B build-watcom -G "Watcom WMake" ^
  -DDEBUG_WOLFSSL=YES ^
  -DWOLFSSL_DEBUG_CERTS=YES ^
  -DCMAKE_SYSTEM_NAME=Windows ^
  -DCMAKE_SYSTEM_PROCESSOR=x86 ^
  -DCMAKE_VERBOSE_MAKEFILE=TRUE ^
  -DWOLFSSL_ASM=NO ^
  -DWOLFSSL_EXAMPLES=NO ^
  -DWOLFSSL_CRYPT_TESTS=NO ^
  -DWOLFSSL_SINGLE_THREADED=YES ^
  -DBUILD_SHARED_LIBS=NO ^
  -DCMAKE_POLICY_DEFAULT_CMP0136=NEW ^
  -DCMAKE_WATCOM_RUNTIME_LIBRARY=SingleThreaded
if errorlevel 1 (
    echo "Config 3 failed!"
    exit /b
)
cmake --build build-watcom || (echo "Build 3 failed!" && exit /b)
call wlink.bat


echo "Config #4"
rmdir /s /q .\build-watcom
cmake -B build-watcom -G "Watcom WMake" ^
  -DDEBUG_WOLFSSL=YES ^
  -DNO_WOLFSSL_DEBUG_CERTS=YES ^
  -DCMAKE_SYSTEM_NAME=Windows ^
  -DCMAKE_SYSTEM_PROCESSOR=x86 ^
  -DCMAKE_VERBOSE_MAKEFILE=TRUE ^
  -DWOLFSSL_ASM=NO ^
  -DWOLFSSL_EXAMPLES=NO ^
  -DWOLFSSL_CRYPT_TESTS=NO ^
  -DWOLFSSL_SINGLE_THREADED=YES ^
  -DBUILD_SHARED_LIBS=NO ^
  -DCMAKE_POLICY_DEFAULT_CMP0136=NEW ^
  -DCMAKE_WATCOM_RUNTIME_LIBRARY=SingleThreaded
if errorlevel 1 (
    echo "Config 4 failed!"
    exit /b
)
cmake --build build-watcom || (echo "Build 4 failed!" && exit /b)
call wlink.bat



echo "Config final"
:: rmdir /s /q .\build-watcom
:: cmake --build build-watcom || (echo "Config final failed!" && exit /b)
::call wlink.bat
