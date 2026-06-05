#!/usr/bin/env python3

from __future__ import annotations

import argparse
import re
from pathlib import Path


MARKER = "/* arduino-beken: C++ constructor tables */"
INSERTION = """    /* arduino-beken: C++ constructor tables */
    .preinit_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } > FLASH

    .init_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        KEEP(*(.init_array))
        KEEP(*(SORT(.ctors.*)))
        KEEP(*(.ctors))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > FLASH

    .fini_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*)))
        KEEP(*(.fini_array))
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } > FLASH

"""


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Patch bk_idk generated linker script with init-array sections.")
    parser.add_argument("--script", required=True, help="Path to the generated <target>_out.ld file")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    script_path = Path(args.script).resolve()
    text = script_path.read_text(encoding="utf-8")

    if MARKER in text or "__init_array_start" in text:
        print(f"Linker script already carries init arrays: {script_path}")
        return 0

    patched, count = re.subn(r"^(\s*\.data\s*:\s*)$", INSERTION + r"\1", text, count=1, flags=re.MULTILINE)
    if count != 1:
        raise SystemExit(f"Unable to locate .data section in {script_path}")

    script_path.write_text(patched, encoding="utf-8")
    print(f"Patched linker script: {script_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
