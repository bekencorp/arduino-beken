#!/usr/bin/env python3

from __future__ import annotations

import binascii
import argparse
import json
import shlex
import subprocess
import sys
from pathlib import Path


def run(cmd: list[str], **kwargs) -> None:
    subprocess.run(cmd, check=True, **kwargs)


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


def linker_group_balance(tokens: list[str]) -> int:
    balance = 0
    for token in tokens:
        linker_options = token[4:].split(",") if token.startswith("-Wl,") else [token]
        for option in linker_options:
            if option == "--start-group":
                balance += 1
            elif option == "--end-group":
                balance -= 1
    return balance


def write_manifest(build_path: Path, manifest: dict) -> None:
    manifest_path = build_path / "bk_arduino_manifest.json"
    manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")


def compile_build_version_object(build_path: Path, compiler: str, ld_flags: list[str]) -> Path:
    source_path = build_path / "bk_build_version.cpp"
    object_path = build_path / "bk_build_version.cpp.o"
    source_path.write_text(
        'extern "C" volatile const char build_version[] = __DATE__ " " __TIME__;\n',
        encoding="utf-8",
    )

    compile_flags = [
        token
        for token in ld_flags
        if not token.startswith("-Wl,") and token != "-nostdlib"
    ]
    run(compile_flags and [compiler, *compile_flags, "-c", str(source_path), "-o", str(object_path)] or [compiler, "-c", str(source_path), "-o", str(object_path)])
    return object_path


def filter_object_files(object_files: list[str]) -> list[str]:
    filtered: list[str] = []
    for path in object_files:
        normalized = path.replace("\\", "/")
        if "/sketch/" in normalized and normalized.endswith(".ino.cpp.o"):
            continue
        filtered.append(path)
    return filtered


def parse_size(size_str: str) -> int:
    value = size_str.strip().upper()
    if value.endswith("K"):
        return int(value[:-1]) * 1024
    if value.endswith("M"):
        return int(value[:-1]) * 1024 * 1024
    return int(value, 0)


def bk_crc16(data: bytes) -> bytes:
    crc_pack_len = 32
    if len(data) % crc_pack_len:
        data += b"\xFF" * (crc_pack_len - (len(data) % crc_pack_len))

    out = bytearray()
    for offset in range(0, len(data), crc_pack_len):
        chunk = data[offset : offset + crc_pack_len]
        crc = 0xFFFFFFFF
        for value in chunk:
            crc ^= value << 8
            for _ in range(8):
                crc = (crc << 1) ^ 0x8005 if (crc & 0x8000) else (crc << 1)
        out.extend(chunk)
        out.extend((crc & 0xFFFF).to_bytes(2, byteorder="big"))
    return bytes(out)


def format_string_to_bytes(value: str, length: int) -> bytes:
    raw = value.encode("utf-8")
    if len(raw) >= length:
        return raw[:length]
    return raw + bytes(length - len(raw))


def serialize_ota_partitions_table(path: Path) -> bytes:
    part_info = json.loads(path.read_text(encoding="utf-8"))
    out = bytearray()
    for part in part_info["part_table"]:
        entry = bytearray()
        entry.extend(int(part["magic"], 16).to_bytes(4, "little"))
        entry.extend(format_string_to_bytes(part["name"], 24))
        entry.extend(format_string_to_bytes(part["flash_name"], 24))
        entry.extend(int(part["offset"], 16).to_bytes(4, "little"))
        entry.extend(parse_size(part["len"]).to_bytes(4, "little"))
        entry.extend((binascii.crc32(entry) & 0xFFFFFFFF).to_bytes(4, "little"))
        out.extend(entry)
    return bytes(out)


def resolve_build_partition_artifact(build_path: Path, sdk_path: Path, name: str) -> Path:
    build_local = build_path / "partitions" / name
    if build_local.exists():
        return build_local
    return sdk_path / "partitions" / name


def build_bootloader_image(build_path: Path, sdk_path: Path, bootloader_bin: Path) -> bytes:
    data = bytearray(bootloader_bin.read_bytes())
    ota_json = resolve_build_partition_artifact(build_path, sdk_path, "bk_ota_partitions.json")
    if not ota_json.exists():
        return bytes(data)

    # Match bk_idk packaging: align bootloader, then append OTA partition table.
    if len(data) % 32:
        data.extend(bytes(32 - (len(data) % 32)))
    data.extend(serialize_ota_partitions_table(ota_json))
    return bytes(data)


def build_app_image(sdk_path: Path, app_bin: Path) -> bytes:
    data = bytearray(app_bin.read_bytes())
    sys_persist_4k_bin = sdk_path / "artifacts" / "package" / "sys_persist_config_4k.bin"
    if sys_persist_4k_bin.exists():
        data = bytearray(sys_persist_4k_bin.read_bytes()) + data
    return bytes(data)


def build_all_app_image(build_path: Path, sdk_path: Path, app_bin: Path, output_path: Path) -> Path | None:
    pack_json = resolve_build_partition_artifact(build_path, sdk_path, "bk_package.json")
    bootloader_bin = sdk_path / "artifacts" / "package" / "bootloader.bin"
    if not pack_json.exists() or not bootloader_bin.exists() or not app_bin.exists():
        return None

    pack_data = json.loads(pack_json.read_text(encoding="utf-8"))
    work_files = {
        "bootloader.bin": build_bootloader_image(build_path, sdk_path, bootloader_bin),
        "app.bin": build_app_image(sdk_path, app_bin),
    }

    packed_sections: list[tuple[int, bytes]] = []
    for section in sorted(pack_data["section"], key=lambda item: int(item["start_addr"], 16)):
        firmware = section["firmware"]
        raw_data = work_files[firmware]
        packed_data = bk_crc16(raw_data) if pack_data.get("crc_enable", False) else raw_data
        if len(packed_data) > parse_size(section["size"]):
            raise RuntimeError(f"{firmware} size is over partition size after packaging")
        packed_sections.append((int(section["start_addr"], 16), packed_data))

    out = bytearray()
    current = 0
    for address, data in packed_sections:
        if current > address:
            raise RuntimeError("partition layout overlap while building all-app.bin")
        if current < address:
            out.extend(b"\xFF" * (address - current))
            current = address
        out.extend(data)
        current += len(data)

    out.extend(b"\xFF" * 34)
    output_path.write_bytes(bytes(out))
    return output_path


def link(args: argparse.Namespace) -> int:
    sdk_path = Path(args.sdk).resolve()
    build_path = Path(args.build_path).resolve()
    build_path.mkdir(parents=True, exist_ok=True)

    elf_path = Path(args.elf).resolve()
    bin_path = Path(args.bin).resolve()
    archive_path = Path(args.archive).resolve()
    compiler = args.compiler

    substitutions = {
        "{compiler.sdk.path}": str(sdk_path),
        "{build.path}": str(build_path),
        "{build.project_name}": args.project_name,
        "{build.core_archive}": str(archive_path),
    }

    ld_flags = expand_tokens(read_flag_file(sdk_path / "flags" / "ld_flags"), substitutions)
    ld_libs = expand_tokens(read_flag_file(sdk_path / "flags" / "ld_libs"), substitutions)
    build_version_obj = compile_build_version_object(build_path, compiler, ld_flags)
    object_files = filter_object_files(args.object_files)
    # Old SDK exports omitted the core archive from ld_libs, so keep the legacy
    # fallback until every export preserves the native core position explicitly.
    core_archive_in_ld_libs = str(archive_path) in ld_libs
    trailing_group_closures = ["-Wl,--end-group"] * max(0, linker_group_balance(ld_flags + ld_libs))
    cmd = (
        [compiler]
        + ld_flags
        + ["-Wl,--allow-multiple-definition"]
        + [f"-L{sdk_path / 'lib'}", f"-L{sdk_path / 'ld'}"]
        + [str(build_version_obj)]
        + object_files
        + ([] if core_archive_in_ld_libs else [str(archive_path)])
        + ld_libs
        + ([] if core_archive_in_ld_libs else [str(archive_path)])
        + trailing_group_closures
        + ["-o", str(elf_path)]
    )

    run(cmd)
    write_manifest(
        build_path,
        {
            "elf": str(elf_path),
            "bin": str(bin_path),
            "sdk": str(sdk_path),
            "archive": str(archive_path),
        },
    )
    return 0


def objcopy(args: argparse.Namespace) -> int:
    build_path = Path(args.build_path).resolve()
    elf_path = Path(args.elf).resolve()
    bin_path = Path(args.bin).resolve()
    sdk_path = Path(args.sdk).resolve()
    bin_path.parent.mkdir(parents=True, exist_ok=True)
    run([args.objcopy, "-O", "binary", str(elf_path), str(bin_path)])
    all_app_path = build_all_app_image(build_path, sdk_path, bin_path, build_path / f"{bin_path.stem}.all-app.bin")
    if all_app_path is not None:
        # Keep the legacy fixed-name artifact for existing scripts, but upload from
        # the sketch-specific image so IDE builds cannot reuse another sketch's file.
        legacy_all_app_path = build_path / "all-app.bin"
        if legacy_all_app_path != all_app_path:
            legacy_all_app_path.write_bytes(all_app_path.read_bytes())

    manifest_path = build_path / "bk_arduino_manifest.json"
    if manifest_path.exists():
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        manifest["bin"] = str(bin_path)
        if all_app_path is not None:
            manifest["all_app_bin"] = str(all_app_path)
        write_manifest(build_path, manifest)
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description="Arduino BK SDK build wrapper")
    sub = parser.add_subparsers(dest="cmd", required=True)

    link_p = sub.add_parser("link")
    link_p.add_argument("--compiler", required=True)
    link_p.add_argument("--sdk", required=True)
    link_p.add_argument("--build-path", required=True)
    link_p.add_argument("--project-name", required=True)
    link_p.add_argument("--archive", required=True)
    link_p.add_argument("--elf", required=True)
    link_p.add_argument("--bin", required=True)
    link_p.add_argument("object_files", nargs="*")

    objcopy_p = sub.add_parser("objcopy")
    objcopy_p.add_argument("--objcopy", required=True)
    objcopy_p.add_argument("--sdk", required=True)
    objcopy_p.add_argument("--build-path", required=True)
    objcopy_p.add_argument("--elf", required=True)
    objcopy_p.add_argument("--bin", required=True)

    args = parser.parse_args()

    try:
        if args.cmd == "link":
            return link(args)
        if args.cmd == "objcopy":
            return objcopy(args)
    except Exception as exc:
        print(f"bk_builder error: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
