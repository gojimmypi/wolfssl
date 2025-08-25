# VisualGDB ESP-IDF v6.0 notes

Manual install:

## Environment Variables:

```
IDF_COMPONENT_STORAGE_URL
```

old value: `file:///C:/SysGCC/esp32-13.2.0_20240530/registry;default`

new value: `file:///C:/SysGCC/esp32-master/registry;default`


```
IDF_PYTHON_ENV_PATH
```

old value: `C:\SysGCC\esp32-13.2.0_20240530\python_env\`

new value: `C:\SysGCC\esp32-master\python_env\`

```
IDF_TOOLS_PATH
```

old value: `C:\SysGCC\esp32-13.2.0_20240530`

new value: `C:\SysGCC\esp32-master`

## Registry

```
Computer\HKEY_CURRENT_USER\Software\Free Software Foundation\SysGCC-xtensa-esp32-elf-15.1.0
```

Add strings:

```
BINUTILS  C:\SysGCC\esp32-master
G++       C:\SysGCC\esp32-master
GCC       C:\SysGCC\esp32-master
```

```
Computer\HKEY_CURRENT_USER\Software\Sysprogs\GNUToolchains
```

Add string:

```
SysGCC-xtensa-esp32-elf-15.1.0  C:\SysGCC\esp32-master
```

See:
Computer\HKEY_CURRENT_USER\Software\Sysprogs\VisualGDB\Toolchains\ESPIDF

was:
Version 14.2.0/14.2/r1

update to:
Version 15.1.0/15.1/r1  (did not fix startup error)
