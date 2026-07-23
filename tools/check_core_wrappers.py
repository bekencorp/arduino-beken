#!/usr/bin/env python3

from __future__ import annotations

import argparse
import os
import shutil
from pathlib import Path

from core_wrappers import CORE_HEADER_MAP, CORE_SOURCE_MAP, STAGED_CORE_PRUNED_FILES, generate_core_wrappers


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Validate that staged/generated Arduino core wrapper files are complete.")
    parser.add_argument("--project-root", required=True, help="Path to the arduino-beken project root")
    parser.add_argument("--core-dir", required=True, help="Path to the generated cores/arduino directory to validate")
    return parser.parse_args()


def list_wrapper_files(core_dir: Path) -> list[Path]:
    return sorted(
        path
        for path in core_dir.rglob("*")
        if path.is_file() and path.suffix in {".h", ".c", ".cpp"}
    )


def main() -> int:
    args = parse_args()
    project_root = Path(args.project_root).resolve()
    core_dir = Path(args.core_dir).resolve()
    try:
        core_dir.relative_to(project_root)
    except ValueError:
        raise SystemExit(f"--core-dir must be inside --project-root: {core_dir}") from None
    # Wrappers are generated with this tree as the root (same as `platform_tree.build_platform_tree`).
    platform_root = core_dir.parent.parent

    expected_from_map = sorted((set(CORE_SOURCE_MAP) | set(CORE_HEADER_MAP)) - STAGED_CORE_PRUNED_FILES)
    missing = [name for name in expected_from_map if not (core_dir / name).exists()]
    if missing:
        raise SystemExit(f"Missing generated core wrapper file(s): {', '.join(missing)}")

    mapping = {**CORE_SOURCE_MAP, **CORE_HEADER_MAP}
    for name in expected_from_map:
        source_path = platform_root / mapping[name]
        wrapper_path = core_dir / name
        relative_include = os.path.relpath(source_path, wrapper_path.parent).replace(os.sep, "/")
        expected_line = f'#include "{relative_include}"\n'
        actual_text = wrapper_path.read_text(encoding="utf-8")
        if expected_line not in actual_text:
            raise SystemExit(f"Unexpected wrapper content in {wrapper_path}")

    # Scratch output must live under `platform_root` at the same depth as `cores/arduino/` so `#include` paths match.
    scratch_dir = platform_root / "cores" / ".wrapper_check_scratch"
    if scratch_dir.exists():
        shutil.rmtree(scratch_dir)
    scratch_dir.mkdir(parents=True, exist_ok=True)
    try:
        generate_core_wrappers(platform_root, scratch_dir, excluded=STAGED_CORE_PRUNED_FILES)

        staged_files = list_wrapper_files(core_dir)
        scratch_files = list_wrapper_files(scratch_dir)
        staged_rels = [p.relative_to(core_dir).as_posix() for p in staged_files]
        scratch_rels = [p.relative_to(scratch_dir).as_posix() for p in scratch_files]
        if staged_rels != scratch_rels:
            raise SystemExit(
                "Staged core wrapper file set does not match regenerated reference.\n"
                f"  staged only: {sorted(set(staged_rels) - set(scratch_rels))}\n"
                f"  scratch only: {sorted(set(scratch_rels) - set(staged_rels))}"
            )

        for rel in staged_rels:
            a = (core_dir / rel).read_text(encoding="utf-8")
            b = (scratch_dir / rel).read_text(encoding="utf-8")
            if a != b:
                raise SystemExit(f"Staged wrapper differs from regenerated reference: {rel}")

        print(f"Validated {len(staged_rels)} generated core wrapper file(s) in {core_dir}")
        return 0
    finally:
        if scratch_dir.exists():
            shutil.rmtree(scratch_dir)


if __name__ == "__main__":
    raise SystemExit(main())
