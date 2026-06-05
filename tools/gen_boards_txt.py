#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import re
from dataclasses import dataclass
from pathlib import Path


APP_PARTITION_NAMES = {"primary_cpu0_app", "primary_app", "app"}
DEFAULT_UPLOAD_MAXIMUM_DATA_SIZE = 262144
DEFAULT_UPLOAD_SPEEDS = (2000000, 1500000, 115200)
DEFAULT_CPU_FREQ = "120000000L"


@dataclass(frozen=True)
class PartitionScheme:
    scheme_id: str
    label: str
    csv_path: Path
    flash_size: str
    upload_maximum_size: int


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate boards.txt for the selected Arduino Beken TARGET.")
    parser.add_argument("--project-root", required=True, help="Path to the arduino-beken project root")
    parser.add_argument("--target", required=True, help="Target SoC, for example bk7239n")
    parser.add_argument("--output", required=True, help="Output boards.txt path")
    return parser.parse_args()


def parse_size_to_bytes(value: str) -> int:
    raw = value.strip().upper()
    if raw.endswith("KB"):
        raw = f"{raw[:-2]}K"
    if raw.endswith("MB"):
        raw = f"{raw[:-2]}M"
    if raw.endswith("K"):
        return int(raw[:-1]) * 1024
    if raw.endswith("M"):
        return int(raw[:-1]) * 1024 * 1024
    return int(raw, 0)


def normalize_flash_size_label(value: str) -> str:
    raw = value.strip().upper()
    if raw.endswith("MB") or raw.endswith("KB"):
        return raw
    if raw.endswith("M") or raw.endswith("K"):
        return f"{raw}B"
    return raw


def read_auto_partition_values(auto_partitions: Path) -> tuple[str, int]:
    flash_size = "4MB"
    upload_maximum_size: int | None = None

    for raw_line in auto_partitions.read_text(encoding="utf-8").splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#"):
            continue
        if "=" in line:
            key, value = [item.strip() for item in line.split("=", 1)]
            if key == "FLASH_CAPACITY":
                flash_size = normalize_flash_size_label(value)
            continue

        row = next(csv.reader([line]))
        if not row or row[0] == "Name":
            continue
        if row[0] in APP_PARTITION_NAMES and len(row) >= 3:
            upload_maximum_size = parse_size_to_bytes(row[2])
            break

    if upload_maximum_size is None:
        raise SystemExit(f"Unable to determine app partition size from {auto_partitions}")

    return flash_size, upload_maximum_size


def partition_scheme_suffix(path: Path) -> str:
    name = path.name
    if name == "auto_partitions.csv":
        return ""
    if name.startswith("auto_partitions_") and name.endswith(".csv"):
        return name[len("auto_partitions_") : -len(".csv")]
    if name.startswith("auto_partitions.") and name.endswith(".csv"):
        return name[len("auto_partitions.") : -len(".csv")]
    raise SystemExit(
        "Partition scheme files must be named auto_partitions.csv, "
        "auto_partitions_<name>.csv, or auto_partitions.<name>.csv: "
        f"{path}"
    )


def normalize_scheme_id(value: str) -> str:
    normalized = re.sub(r"[^A-Za-z0-9]+", "_", value.strip().lower()).strip("_")
    if not normalized:
        return "default"
    if normalized[0].isdigit():
        normalized = f"scheme_{normalized}"
    return normalized


def partition_scheme_label(path: Path) -> str:
    suffix = partition_scheme_suffix(path)
    if not suffix:
        return "Default"
    return " ".join(part.capitalize() for part in re.split(r"[_\-.]+", suffix) if part)


def find_partition_schemes(config_dir: Path) -> list[PartitionScheme]:
    scheme_files = sorted(config_dir.glob("auto_partitions*.csv"))
    if not scheme_files:
        raise SystemExit(f"No auto_partitions CSV files found under {config_dir}")

    schemes: list[PartitionScheme] = []
    seen_ids: set[str] = set()
    for path in sorted(
        scheme_files,
        key=lambda item: (0 if item.name == "auto_partitions.csv" else 1, item.name),
    ):
        flash_size, upload_maximum_size = read_auto_partition_values(path)
        scheme_id = normalize_scheme_id(partition_scheme_suffix(path))
        if scheme_id in seen_ids:
            raise SystemExit(f"Duplicate partition scheme id '{scheme_id}' under {config_dir}")
        seen_ids.add(scheme_id)
        schemes.append(
            PartitionScheme(
                scheme_id=scheme_id,
                label=partition_scheme_label(path),
                csv_path=path,
                flash_size=flash_size,
                upload_maximum_size=upload_maximum_size,
            )
        )
    return schemes


def variant_sort_key(path: Path) -> tuple[int, str]:
    name = path.name
    return (0 if name.endswith("_generic") else 1, name)


def select_variants(variants_dir: Path, target: str) -> list[Path]:
    if not variants_dir.exists():
        raise SystemExit(f"Variants directory not found: {variants_dir}")

    matches = [
        path
        for path in variants_dir.iterdir()
        if path.is_dir() and (path.name == target or path.name.startswith(f"{target}_"))
    ]
    if not matches:
        raise SystemExit(f"No variants found for TARGET={target} under {variants_dir}")

    return sorted(matches, key=variant_sort_key)


def board_macro(board_id: str) -> str:
    return re.sub(r"[^A-Za-z0-9]+", "_", board_id).upper()


def board_name(target: str, variant: str) -> str:
    if variant == target:
        return target.upper()

    suffix = variant[len(target) :].lstrip("_-")
    if not suffix:
        return target.upper()

    words = [part.capitalize() for part in re.split(r"[_-]+", suffix) if part]
    return f"{target.upper()} {' '.join(words)}"


def build_board_block(
    *,
    board_id: str,
    target: str,
    schemes: list[PartitionScheme],
) -> list[str]:
    default_scheme = schemes[0]
    lines = [
        f"{board_id}.name={board_name(target, board_id)}",
        f"{board_id}.upload.tool=bk_uploader",
        f"{board_id}.upload.tool.default=bk_uploader",
        f"{board_id}.upload.tool.serial=bk_uploader",
        f"{board_id}.upload.protocol=serial",
        f"{board_id}.upload.maximum_size={default_scheme.upload_maximum_size}",
        f"{board_id}.upload.maximum_data_size={DEFAULT_UPLOAD_MAXIMUM_DATA_SIZE}",
        f"{board_id}.build.board={board_macro(board_id)}",
        f"{board_id}.build.core=arduino",
        f"{board_id}.build.variant={board_id}",
        f"{board_id}.build.mcu={target}",
        f"{board_id}.build.f_cpu={DEFAULT_CPU_FREQ}",
        f"{board_id}.build.arch={target}",
        f"{board_id}.build.flash_size={default_scheme.flash_size}",
        f"{board_id}.build.partitions_csv=config/{target}/{default_scheme.csv_path.name}",
        f"{board_id}.upload.speed={DEFAULT_UPLOAD_SPEEDS[0]}",
        "",
    ]

    for speed in DEFAULT_UPLOAD_SPEEDS:
        lines.append(f"{board_id}.menu.UploadSpeed.{speed}={speed}")
        lines.append(f"{board_id}.menu.UploadSpeed.{speed}.upload.speed={speed}")

    if len(schemes) > 1:
        lines.append("")
        for scheme in schemes:
            lines.append(
                f"{board_id}.menu.PartitionScheme.{scheme.scheme_id}={scheme.label}"
            )
            lines.append(
                f"{board_id}.menu.PartitionScheme.{scheme.scheme_id}.build.partitions_csv=config/{target}/{scheme.csv_path.name}"
            )
            lines.append(
                f"{board_id}.menu.PartitionScheme.{scheme.scheme_id}.build.flash_size={scheme.flash_size}"
            )
            lines.append(
                f"{board_id}.menu.PartitionScheme.{scheme.scheme_id}.upload.maximum_size={scheme.upload_maximum_size}"
            )

    return lines


def generate_boards_txt(project_root: Path, target: str) -> str:
    config_dir = project_root / "config" / target
    variants_dir = project_root / "variants"
    schemes = find_partition_schemes(config_dir)
    variants = select_variants(variants_dir, target)

    lines = [
        "# Auto-generated by tools/gen_boards_txt.py.",
        f"# TARGET={target}",
        "menu.UploadSpeed=Upload Speed",
        *(
            ["menu.PartitionScheme=Partition Scheme"]
            if len(schemes) > 1
            else []
        ),
        "",
    ]
    for index, variant_dir in enumerate(variants):
        if index:
            lines.append("")
        lines.extend(
            build_board_block(
                board_id=variant_dir.name,
                target=target,
                schemes=schemes,
            )
        )

    return "\n".join(lines).rstrip() + "\n"


def main() -> int:
    args = parse_args()
    project_root = Path(args.project_root).resolve()
    output_path = Path(args.output).resolve()
    output_path.write_text(generate_boards_txt(project_root, args.target), encoding="utf-8")
    print(f"Generated {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())