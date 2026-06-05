#!/usr/bin/env python3

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate Arduino partition artifacts from config/<target>/auto_partitions.csv.")
    parser.add_argument("--sdk-path", required=True)
    parser.add_argument("--platform-path", required=True)
    parser.add_argument("--build-path", required=True)
    parser.add_argument("--project-name", required=True)
    parser.add_argument("--target", required=True)
    parser.add_argument("--partitions-csv", default="")
    return parser.parse_args()


def copy_if_exists(src: Path, dst: Path) -> None:
    if not src.exists():
        return
    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(src, dst)


def main() -> int:
    args = parse_args()
    sdk_path = Path(args.sdk_path).resolve()
    platform_path = Path(args.platform_path).resolve()
    build_path = Path(args.build_path).resolve()
    config_dir = platform_path / "config" / args.target
    partitions_csv = (
        (platform_path / args.partitions_csv).resolve()
        if args.partitions_csv
        else (config_dir / "auto_partitions.csv").resolve()
    )
    synthetic_project_root = build_path / "bk_partition_project"
    synthetic_config_dir = synthetic_project_root / "config" / args.target
    generator_root = sdk_path / "tools" / "partition_gen"
    build_process_root = generator_root / "build_process"
    auto_partition_script = build_process_root / "bk_build_auto_partition.py"

    if not config_dir.exists():
        raise SystemExit(f"Partition config directory not found: {config_dir}")
    if not partitions_csv.exists():
        raise SystemExit(f"Partition CSV not found: {partitions_csv}")
    if not auto_partition_script.exists():
        raise SystemExit(f"Partition generator not found in exported SDK: {auto_partition_script}")

    build_path.mkdir(parents=True, exist_ok=True)
    (build_path / "partitions").mkdir(parents=True, exist_ok=True)
    synthetic_config_dir.mkdir(parents=True, exist_ok=True)
    shutil.copy2(partitions_csv, synthetic_config_dir / "auto_partitions.csv")
    env = os.environ.copy()
    python_paths = [
        str(build_process_root),
        str(generator_root / "bk_py_libs"),
    ]
    if env.get("PYTHONPATH"):
        python_paths.append(env["PYTHONPATH"])

    env.update(
        {
            "PROJECT_DIR": str(synthetic_project_root),
            "PROJECT_NAME": args.project_name or "arduino-beken",
            "PROJECT_BUILD_DIR": str(build_path),
            "ARMINO_SOC_NAME": args.target,
            "BUILD_TARGETS": "app",
            "PYTHONPATH": os.pathsep.join(python_paths),
        }
    )

    subprocess.run([sys.executable, str(auto_partition_script)], check=True, env=env)

    partitions_dir = build_path / "partitions"
    generated_include_dir = build_path / "bk_generated" / "include"
    generated_include_dir.mkdir(parents=True, exist_ok=True)
    copy_if_exists(partitions_dir / "partitions.h", generated_include_dir / "partitions.h")
    copy_if_exists(partitions_dir / "partitions_gen.h", generated_include_dir / "partitions_gen.h")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
