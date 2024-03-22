# PlatformIO

Follow the [instructions](https://docs.platformio.org/en/latest/core/installation/methods/index.html) to install PlatformIO.

Note there are two options:

- [Core CLI](https://docs.platformio.org/en/latest/core/index.html)
- [VSCode IDE](https://docs.platformio.org/en/latest/integration/ide/vscode.html#ide-vscode)


## Publishing

The PlatformIO Core CLI is needed to publish wolfSSL:

See the [Arduino](../ARDUINO/README.md) publishing notes.


### Publish PlatformIO Arduino Library with Windows

Setup the PlatformIO CLI:

```dos
set PATH=%PATH%;C:\Users\%USERNAME%\.platformio\penv\Scripts\
pio --help
pio account show
```

Publish

```dos
pio pkg publish --owner wolfSSL C:\backup\PlatformIO\wolfssl
```

### Publish with Linux

```bash
set PATH=%PATH%;C:\Users\%USERNAME%\.platformio\penv\Scripts\
pio --help
pio account show
```

```bash
pio pkg publish --owner wolfSSL ~\workspace\
```
