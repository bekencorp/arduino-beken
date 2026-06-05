#!/usr/bin/env python3

from __future__ import annotations

import argparse
import shlex
import subprocess
import sys
from pathlib import Path


def read_flag_file(path: Path) -> list[str]:
    if not path.exists():
        return []

    tokens: list[str] = []
    for line in path.read_text(encoding="utf-8").splitlines():
        stripped = line.strip()
        if not stripped:
            continue
        tokens.extend(shlex.split(stripped))
    return tokens


def expand_tokens(tokens: list[str], substitutions: dict[str, str]) -> list[str]:
    expanded: list[str] = []
    for token in tokens:
        value = token
        for key, replacement in substitutions.items():
            value = value.replace(key, replacement)
        expanded.append(value)
    return expanded


def main() -> int:
    parser = argparse.ArgumentParser(description="Compile wrapper for arduino-beken SDK exports.")
    parser.add_argument("--compiler", required=True, help="Compiler executable")
    parser.add_argument("--sdk", required=True, help="Exported SDK root")
    parser.add_argument(
        "--flag-file",
        action="append",
        default=[],
        help="Flag file name under <sdk>/flags to expand before the remaining compiler args",
    )
    args, rest = parser.parse_known_args()

    sdk_path = Path(args.sdk).resolve()
    substitutions = {"{compiler.sdk.path}": str(sdk_path)}

    cmd = [args.compiler]
    for flag_file in args.flag_file:
        cmd.extend(expand_tokens(read_flag_file(sdk_path / "flags" / flag_file), substitutions))
    cmd.extend(rest)

    try:
        subprocess.run(cmd, check=True)
    except subprocess.CalledProcessError as exc:
        return exc.returncode
    except Exception as exc:
        print(f"bk_compile error: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
