# Espressif Component Manager

This directory does not need to be included in any wolfSSL distributions. The files
contained herein are used only to publish wolfSSL as a managed component to the [ESP Registry](https://components.espressif.com/).

See the local [wolfssl_component_publish.sh script](./wolfssl_component_publish.sh) for the steps used to copy wolfSSL
source files locally.

The wolfSSL [README.md](https://github.com/wolfSSL/wolfssl/blob/master/README.md) is stripped of an embedded HTML
anchor tags that do not render well.

The local [README_REGISTRY_PREPEND.md](./README_REGISTRY_PREPEND.md) text is prepended to the 
wolfSSL [README.md](https://github.com/wolfSSL/wolfssl/blob/master/README.md) before being published to the registry.

Other README.md files for the examples are appended automatically by the ESP Registry at pubish time.
As such example REAME files must _not_ contain any relative links.

A working Internet connection is required to build the samples.

Any new examples should have a manifest file in the `[project]/main` directory:

```
## IDF Component Manager Manifest File
dependencies:
  wolfssl/wolfssl: "^1.0.13-dev"
  ## Required IDF version
  idf:
    version: ">=4.1.0"
  # # Put list of dependencies here
  # # For components maintained by Espressif:
  # component: "~1.0.0"
  # # For 3rd party components:
  # username/component: ">=1.0.0,<2.0.0"
  # username2/component2:
  #   version: "~1.0.0"
  #   # For transient dependencies `public` flag can be set.
  #   # `public` flag doesn't have an effect dependencies of the `main` component.
  #   # All dependencies of `main` are public by default.
  #   public: true
  ```
