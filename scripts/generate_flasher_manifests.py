#!/usr/bin/env python3
"""Generates ESP Web Tools manifests for the browser flasher, see docs/flash.md.

Run after `pio run` + `pio run -t buildfs`. Offsets must match partitions_custom.csv.
"""

import argparse
import json
import os

BOOTLOADER_OFFSET = 0x1000
PARTITION_TABLE_OFFSET = 0x8000
APP_OFFSET = 0x10000
LITTLEFS_OFFSET = 0x250000  # must match the "littlefs" row in partitions_custom.csv


def build(parts):
    return {"chipFamily": "ESP32", "parts": parts}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", required=True, help="output directory for the manifest JSON files")
    parser.add_argument("--version", required=True, help="version string to embed (e.g. a short git SHA)")
    args = parser.parse_args()

    base_parts = [
        {"path": "bootloader.bin", "offset": BOOTLOADER_OFFSET},
        {"path": "partitions.bin", "offset": PARTITION_TABLE_OFFSET},
        {"path": "firmware.bin", "offset": APP_OFFSET},
    ]
    full_parts = base_parts + [{"path": "littlefs.bin", "offset": LITTLEFS_OFFSET}]

    # new_install_prompt_erase controls what happens on click, see
    # esp-web-tools' install-dialog: true shows an interactive "erase
    # device?" prompt (user picks yes/no each time); false skips the
    # prompt and erases automatically. There is no third option to skip
    # the prompt AND skip erasing - see docs/flash.md.
    manifests = {
        "manifest-erase.json": {
            "name": "Paintball Taster-Sound - Komplette Neuinstallation",
            "version": args.version,
            "new_install_prompt_erase": False,
            "builds": [build(full_parts)],
        },
        "manifest-reset.json": {
            "name": "Paintball Taster-Sound - Firmware reset",
            "version": args.version,
            "new_install_prompt_erase": True,
            "builds": [build(full_parts)],
        },
        "manifest-update.json": {
            "name": "Paintball Taster-Sound - Firmware-Update",
            "version": args.version,
            "new_install_prompt_erase": True,
            "builds": [build(base_parts)],
        },
    }

    os.makedirs(args.out, exist_ok=True)
    for filename, manifest in manifests.items():
        path = os.path.join(args.out, filename)
        with open(path, "w") as f:
            json.dump(manifest, f, indent=2)
            f.write("\n")
        print(f"wrote {path}")


if __name__ == "__main__":
    main()
