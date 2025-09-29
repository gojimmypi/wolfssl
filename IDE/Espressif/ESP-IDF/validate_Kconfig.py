#!/usr/bin/env python3

# Copyright (C) 2006-2025 wolfSSL Inc.
#
# This file is part of wolfSSL.
#
# wolfSSL is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# wolfSSL is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA

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
