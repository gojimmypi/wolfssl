@echo off
REM Expect the script at /path/to/wolfssl/IDE/Espressif/ESP-IDF/

echo;
echo wolfSSL Windows Setup.
echo;


if NOT EXIST "setup.sh" ( 
  echo Please run this script at /path/to/wolfssl/IDE/Espressif/ESP-IDF/
  goto :ERR
)

:: see if there was a parameter passed for a specific EDP-IDF directory
:: this may be different than the standard ESP-IDF environment (e.g. VisualGDB)
if not "%1" == "" (
	if not exist "%1" (
		echo "ERROR: optional directory was specified, but not found: %1"
		goto :ERR
	)

	SET IDF_PATH=%1
	echo Using specified IDF_PATH: %IDF_PATH%
)

:: if no IDF_PATH is found, we don't know what to do. Go exit with error.
if "%IDF_PATH%" == "" (
  echo Please launch the script from ESP-IDF command prompt,
  echo or set your desired IDF_PATH environment variable,
  echo or pass a parameter to your directory, such as for VisualGDB with ESP-IDF 4.4:
  echo;
  echo   .\setup_win.bat C:\SysGCC\esp32\esp-idf\v4.4
  echo;
  goto :ERR
)

:: Here we go! 
:: setup some path variables
echo;

set SCRIPTDIR=%CD%
set BASEDIR=%SCRIPTDIR%\..\..\..\
set WOLFSSL_ESPIDFDIR=%BASEDIR%\IDE\Espressif\ESP-IDF
set WOLFSSLLIB_TRG_DIR=%IDF_PATH%\components\wolfssl
set WOLFSSLEXP_TRG_DIR=%IDF_PATH%\examples\protocols

echo Using SCRIPTDIR          = %SCRIPTDIR%
echo Using BASEDIR            = %BASEDIR%
echo Using WOLFSSL_ESPIDFDIR  = %WOLFSSL_ESPIDFDIR%
echo Using WOLFSSLLIB_TRG_DIR = %WOLFSSLLIB_TRG_DIR%
echo Using WOLFSSLEXP_TRG_DIR = %WOLFSSLEXP_TRG_DIR%

echo;
echo Copy files into %IDF_PATH%
rem Remove/Create directories

:: do we really want to erase existing user config?
if exist %WOLFSSLLIB_TRG_DIR% (
  echo;
  echo WARNING: Existing files found in %WOLFSSLLIB_TRG_DIR%
  echo;
  :: clear any prior errorlevel
  call;
  choice /c YN /m "Delete files and proceed with install and overwrite existing config in %WOLFSSLLIB_TRG_DIR%\include\config.h?"
  if errorlevel 2 GOTO :NOCOPY
)

:: purge existing directory

echo "Removing %WOLFSSLLIB_TRG_DIR%"
rmdir /S/Q %WOLFSSLLIB_TRG_DIR%

mkdir      %WOLFSSLLIB_TRG_DIR%
mkdir      %WOLFSSLLIB_TRG_DIR%\src
mkdir      %WOLFSSLLIB_TRG_DIR%\wolfcrypt\src
mkdir      %WOLFSSLLIB_TRG_DIR%\wolfssl
mkdir      %WOLFSSLLIB_TRG_DIR%\wolfssl\openssl
mkdir      %WOLFSSLLIB_TRG_DIR%\test
mkdir      %WOLFSSLLIB_TRG_DIR%\include

rem copying ... files in src/ into $WOLFSSLLIB_TRG_DIR%/src
echo;
echo Copying files to %WOLFSSLLIB_TRG_DIR%\src\
xcopy /Y/Q %BASEDIR%\src\*.c %WOLFSSLLIB_TRG_DIR%\src\
if %errorlevel% NEQ 0 GOTO :COPYERR

echo;
echo Copying src\*.c files to %WOLFSSLLIB_TRG_DIR%\wolfcrypt\src
xcopy /Y/Q %BASEDIR%\wolfcrypt\src\*.c %WOLFSSLLIB_TRG_DIR%\wolfcrypt\src

echo;
echo Copying src\*.i files to %WOLFSSLLIB_TRG_DIR%\wolfcrypt\src
xcopy /Y/Q %BASEDIR%\wolfcrypt\src\*.i %WOLFSSLLIB_TRG_DIR%\wolfcrypt\src

echo;
echo Copying files to %WOLFSSLLIB_TRG_DIR%\wolfcrypt\src\port\
xcopy /E/Y/Q %BASEDIR%\wolfcrypt\src\port %WOLFSSLLIB_TRG_DIR%\wolfcrypt\src\port\

echo;
echo Copying files to %WOLFSSLLIB_TRG_DIR%\wolfcrypt\test\
xcopy /E/Y/Q %BASEDIR%\wolfcrypt\test %WOLFSSLLIB_TRG_DIR%\wolfcrypt\test\

rem Copy dummy test_paths.h to handle the case configure hasn't yet executed 
echo F |xcopy /E/Y %WOLFSSL_ESPIDFDIR%\dummy_test_paths.h %WOLFSSLLIB_TRG_DIR%\wolfcrypt\test\test_paths.h
xcopy /E/Y/Q %WOLFSSL_ESPIDFDIR%\dummy_test_paths.h %WOLFSSLIB_TRG_DIR%\wolfcrypt\test\test_paths.h

echo;
echo Copying files to %WOLFSSLLIB_TRG_DIR%\wolfcrypt\benchmark\
xcopy /E/Y/Q %BASEDIR%\wolfcrypt\benchmark %WOLFSSLLIB_TRG_DIR%\wolfcrypt\benchmark\

echo;
echo Copying files to %WOLFSSLLIB_TRG_DIR%\wolfssl\
xcopy /Y/Q %BASEDIR%\wolfssl\*.h %WOLFSSLLIB_TRG_DIR%\wolfssl\

echo;
echo Copying files to%WOLFSSLLIB_TRG_DIR%\wolfssl\openssl\
xcopy /Y/Q %BASEDIR%\wolfssl\openssl\*.h %WOLFSSLLIB_TRG_DIR%\wolfssl\openssl\

echo;
echo Copying files to %WOLFSSLLIB_TRG_DIR%\wolfssl\wolfcrypt\
xcopy /E/Y/Q %BASEDIR%\wolfssl\wolfcrypt %WOLFSSLLIB_TRG_DIR%\wolfssl\wolfcrypt\

rem user_settings.h
echo;
echo Copying user_settings.h to %WOLFSSLLIB_TRG_DIR%\include\
xcopy %WOLFSSL_ESPIDFDIR%\user_settings.h %WOLFSSLLIB_TRG_DIR%\include\         /S /E /Q

echo;
echo Copying dummy_config_h. to %WOLFSSLLIB_TRG_DIR%\include\config.h
if exist  "%WOLFSSLLIB_TRG_DIR%\include\config.h" (
  echo SKIP!
) else (
  :: echo Creating new config file: %WOLFSSLLIB_TRG_DIR%\include\config.h
  echo new config > %WOLFSSLLIB_TRG_DIR%\include\config.h
  xcopy  %WOLFSSL_ESPIDFDIR%\dummy_config_h. %WOLFSSLLIB_TRG_DIR%\include\config.h /Q /Y

)

rem unit test app
echo;
echo Copying unit files to %WOLFSSLLIB_TRG_DIR%\test\
xcopy /E/Y/Q %WOLFSSL_ESPIDFDIR%\test %WOLFSSLLIB_TRG_DIR%\test\

echo;
echo Copying CMakeLists.txt to %WOLFSSLLIB_TRG_DIR%\
xcopy /F/Q   %WOLFSSL_ESPIDFDIR%\libs\CMakeLists.txt %WOLFSSLLIB_TRG_DIR%\

echo;
echo Copying component.mk to %WOLFSSLLIB_TRG_DIR%\"
xcopy /F/Q   %WOLFSSL_ESPIDFDIR%\libs\component.mk   %WOLFSSLLIB_TRG_DIR%\

rem Benchmark program
echo;
echo Removing %WOLFSSLEXP_TRG_DIR%\wolfssl_benchmark\
rmdir /S/Q %WOLFSSLEXP_TRG_DIR%\wolfssl_benchmark\

echo;
echo Copying %WOLFSSLEXP_TRG_DIR%\wolfssl_benchmark\main\
mkdir      %WOLFSSLEXP_TRG_DIR%\wolfssl_benchmark\main\
xcopy      %BASEDIR%\wolfcrypt\benchmark\benchmark.h  %BASEDIR%\IDE\Espressif\ESP-IDF\examples\wolfssl_benchmark\main\benchmark.h
xcopy      %BASEDIR%\wolfcrypt\benchmark\benchmark.c  %BASEDIR%\IDE\Espressif\ESP-IDF\examples\wolfssl_benchmark\main\benchmark.c
xcopy /F/Q %BASEDIR%\wolfcrypt\benchmark\benchmark.c %WOLFSSLEXP_TRG_DIR%\wolfssl_benchmark\main\

xcopy /E/F/Q %WOLFSSL_ESPIDFDIR%\examples\wolfssl_benchmark %WOLFSSLEXP_TRG_DIR%\wolfssl_benchmark\

:: Crypt Test program
rmdir /S/Q %WOLFSSLEXP_TRG_DIR%\wolfssl_test\
mkdir      %WOLFSSLEXP_TRG_DIR%\wolfssl_test\main\
xcopy /F/Q %BASEDIR%\wolfcrypt\test\test.c %WOLFSSLEXP_TRG_DIR%\wolfssl_test\main\
xcopy /E/F/Q %WOLFSSL_ESPIDFDIR%\examples\wolfssl_test %WOLFSSLEXP_TRG_DIR%\wolfssl_test\

:: TLS Client program
rmdir /S/Q %WOLFSSLEXP_TRG_DIR%\wolfssl_client\
mkdir      %WOLFSSLEXP_TRG_DIR%\wolfssl_client\main\
xcopy /E/F/Q %WOLFSSL_ESPIDFDIR%\examples\wolfssl_client %WOLFSSLEXP_TRG_DIR%\wolfssl_client\

:: TLS Server program
rmdir /S/Q %WOLFSSLEXP_TRG_DIR%\wolfssl_server\
mkdir      %WOLFSSLEXP_TRG_DIR%\wolfssl_server\main\
xcopy /E/F/Q %WOLFSSL_ESPIDFDIR%\examples\wolfssl_server %WOLFSSLEXP_TRG_DIR%\wolfssl_server\

goto :DONE

:: error during copy encountered
:COPYERR
echo Error during copy.
goto :ERR

:: abort at user request
:NOCOPY
echo Setup did not copy any files.
goto :ERR

:: ERROR 
:ERR
exit /B 1

:: Success
:DONE
echo;
echo Edit config file in  "%WOLFSSLLIB_TRG_DIR%\include\user_settings.h" before trying to compile.
echo;

echo completed


