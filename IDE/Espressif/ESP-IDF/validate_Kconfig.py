#!/usr/bin/env python3

# Usage: validate_Kconfig.py <path/to/Kconfig
#
# Validates a given Kconfig file

from kconfiglib import Kconfig
import os
import sys

def main():
    if len(sys.argv) < 2:
        print("Usage: validate_Kconfig.py <path/to/Kconfig>")
        sys.exit(1)

    # Adjust this path if your top-level Kconfig is elsewhere
    kconfig_path = sys.argv[1]

    if not os.path.exists(kconfig_path):
        print(f"❌ Kconfig file not found: {kconfig_path}")
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
