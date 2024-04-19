# wolfSSL Espressif Component

This is the wolfSSL directory for a an Espressif ESP-IDF component.

## Directory Contents

This directory must contain, at a minimum:

- CMakeLists.txt
- ./include/user_settings.h

The directory should also contain:
- Kconfig
- component.mk

The directory may contain wolfSSL source, for example with a [Managed Component](https://components.espressif.com/components/wolfssl/wolfssl).


If the wolfSSL source is not included here, the `CMakeLists.txt` will search for it in this order:

- A hard-coded `WOLFSSL_ROOT` cmake variable.
- `WOLFSSL_ROOT` Environment Variable
- The `CONFIG_CUSTOM_SETTING_WOLFSSL_ROOT` value in the `sdkconfig` file, from the `Kconfig` option.
- Any parent directories, up to the root (if this directory is in the ESP-IDF components)
- Any parent directories, up to the root (if this directory is a project component)

While recursing up the directory tree, the following names of wolfSSL directories will be considered:

- `wolfssl-[current user name]`
- `wolfssl-master`
- `wolfssl`

## Contact

Have a specific request or questions? We'd love to hear from you! Please contact us at support@wolfssl.com or open an issue on GitHub.

## Licensing and Support

wolfSSL (formerly known as CyaSSL) and wolfCrypt are either licensed for use under the GPLv2 (or at your option any later version) or a standard commercial license. For our users who cannot use wolfSSL under GPLv2 (or any later version), a commercial license to wolfSSL and wolfCrypt is available.

See the LICENSE.txt, visit wolfssl.com/license, contact us at licensing@wolfssl.com or call +1 425 245 8247

View Commercial Support Options: wolfssl.com/products/support-and-maintenance
