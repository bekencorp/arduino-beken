#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path

from core_wrappers import generate_core_wrappers


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate Arduino core wrapper sources from component implementations.")
    parser.add_argument("--project-root", required=True, help="Path to the arduino-beken project root")
    parser.add_argument(
        "--output-dir",
        default="",
        help="Optional output core directory. Defaults to <project-root>/cores/arduino",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    project_root = Path(args.project_root).resolve()
    output_dir = Path(args.output_dir).resolve() if args.output_dir else project_root / "cores" / "arduino"

    written = generate_core_wrappers(project_root, output_dir)
    print(f"Generated {len(written)} core wrapper source file(s) under {output_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
