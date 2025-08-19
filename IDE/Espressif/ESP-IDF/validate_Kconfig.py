#!/usr/bin/env python3

from kconfiglib import Kconfig
import os
import sys

def main():
    # Adjust this path if your top-level Kconfig is elsewhere
    kconfig_path = "Kconfig"

    if not os.path.exists(kconfig_path):
        print(f"❌ Kconfig not found: {kconfig_path}")
        sys.exit(1)

    # Set target if needed for conditional logic in ESP-IDF Kconfig files
    os.environ["IDF_TARGET"] = os.getenv("IDF_TARGET", "esp32")

    try:
        kconf = Kconfig(kconfig_path)
        print("✅ Kconfig parsed successfully.")
    except Exception as e:
        print(f"❌ Kconfig parsing failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
