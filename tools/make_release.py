#!/usr/bin/env python3

from __future__ import annotations

import argparse
import hashlib
import json
import os
import shutil
import tarfile
import tempfile
import zipfile
from pathlib import Path
from urllib.parse import urlparse

from platform_tree import platform_display_name, sdk_tool_name


DEFAULT_TOOLCHAIN_LINUX_ROOT = Path("/opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi")
DEFAULT_TOOLCHAIN_MANIFEST = Path(__file__).resolve().parent / "toolchains.json"
DEFAULT_TOOLCHAIN_HOSTS = (
    "x86_64-linux-gnu",
    "x86_64-mingw32",
    "x86_64-apple-darwin",
    "arm64-apple-darwin",
)

DEFAULT_RELEASE_GZIP_LEVEL = 3


def release_gzip_compresslevel() -> int:
    raw = os.environ.get("ARDUINO_BEKEN_RELEASE_GZIP_LEVEL", str(DEFAULT_RELEASE_GZIP_LEVEL))
    try:
        level = int(raw.strip())
    except ValueError:
        return DEFAULT_RELEASE_GZIP_LEVEL
    return max(0, min(9, level))


def open_release_gzip_tar(path: Path) -> tarfile.TarFile:
    return tarfile.open(path, "w:gz", compresslevel=release_gzip_compresslevel())


def _tar_ignore_filter(ignore_names: frozenset[str]):
    def filt(tarinfo: tarfile.TarInfo) -> tarfile.TarInfo | None:
        parts = tarinfo.name.split("/")
        if any(part in ignore_names for part in parts):
            return None
        return tarinfo

    return filt


def sha256sum(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as fp:
        for chunk in iter(lambda: fp.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def load_sdk_manifest(sdk_root: Path) -> dict:
    manifest_path = sdk_root / "manifest.json"
    if not manifest_path.exists():
        raise SystemExit(f"SDK manifest not found: {manifest_path}")
    return json.loads(manifest_path.read_text(encoding="utf-8"))


def load_toolchain_manifest(path: Path) -> dict:
    if not path.exists():
        raise SystemExit(f"Toolchain manifest not found: {path}")
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise SystemExit(f"Invalid toolchain manifest JSON: {path}: {exc}") from exc
    if not isinstance(data, dict):
        raise SystemExit(f"Toolchain manifest root must be a JSON object: {path}")
    return data


def add_tree_to_tar(tar: tarfile.TarFile, src: Path, arcname: str, ignore_names: set[str] | None = None) -> None:
    ignore = frozenset(ignore_names or {".git", "__pycache__"})
    tar.add(src, arcname=arcname, filter=_tar_ignore_filter(ignore), recursive=True)


def create_platform_archive(output_dir: Path, platform_root: Path, version: str, target: str) -> Path:
    staging_root = output_dir / "staging-platform"
    if staging_root.exists():
        shutil.rmtree(staging_root)
    archived_platform_root = staging_root / "arduino-beken"
    shutil.copytree(platform_root, archived_platform_root)

    archive = output_dir / f"arduino-beken-{target}-{version}.tar.gz"
    with open_release_gzip_tar(archive) as tar:
        tar.add(
            archived_platform_root,
            arcname="arduino-beken",
            filter=_tar_ignore_filter(frozenset({".git", "__pycache__"})),
            recursive=True,
        )
    return archive


def create_sdk_archive(output_dir: Path, version: str, sdk_root: Path, target: str) -> Path:
    tool_name = sdk_tool_name(target)
    archive = output_dir / f"{tool_name}-{version}.tar.gz"
    with open_release_gzip_tar(archive) as tar:
        add_tree_to_tar(tar, sdk_root, tool_name, ignore_names={".git", "__pycache__"})
    return archive


def write_text_file(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def create_lightweight_archive(archive: Path, root_name: str, files: dict[str, str]) -> Path:
    if archive.suffix == ".zip":
        with zipfile.ZipFile(archive, "w", compression=zipfile.ZIP_DEFLATED) as zf:
            for relative_path, content in files.items():
                zf.writestr(f"{root_name}/{relative_path}", content)
        return archive

    with tempfile.TemporaryDirectory(prefix="arduino-beken-release-") as temp_dir:
        temp_root = Path(temp_dir) / root_name
        temp_root.mkdir(parents=True, exist_ok=True)
        for relative_path, content in files.items():
            write_text_file(temp_root / relative_path, content)
        with open_release_gzip_tar(archive) as tar:
            tar.add(temp_root, arcname=root_name)
    return archive


def create_sdk_archive_lightweight(output_dir: Path, version: str, sdk_root: Path, target: str) -> Path:
    tool_name = sdk_tool_name(target)
    archive = output_dir / f"{tool_name}-{version}.tar.gz"
    manifest_path = sdk_root / "manifest.json"
    files = {
        "VALIDATION.txt": (
            "Lightweight validation archive for release metadata checks.\n"
            f"source_sdk_root={sdk_root}\n"
            "This archive intentionally omits the full SDK payload.\n"
        ),
        "manifest.json": manifest_path.read_text(encoding="utf-8") if manifest_path.exists() else "{}\n",
    }
    return create_lightweight_archive(archive, tool_name, files)


def create_host_tool_archive(output_dir: Path, name: str, version: str, host_suffix: str, root: Path) -> Path:
    ext = "zip" if host_suffix == "win64" else "tar.gz"
    archive = output_dir / f"{name}-{version}-{host_suffix}.{ext}"
    root_name = f"{name}-{version}"

    if ext == "zip":
        temp_root = output_dir / f"{name}-{version}-{host_suffix}"
        if temp_root.exists():
            shutil.rmtree(temp_root)
        shutil.copytree(root, temp_root)
        shutil.make_archive(str(archive.with_suffix("")), "zip", root_dir=temp_root.parent, base_dir=temp_root.name)
        return archive

    with open_release_gzip_tar(archive) as tar:
        add_tree_to_tar(tar, root, root_name)
    return archive


def create_host_tool_archive_lightweight(output_dir: Path, name: str, version: str, host_suffix: str, root: Path) -> Path:
    ext = "zip" if host_suffix == "win64" else "tar.gz"
    archive = output_dir / f"{name}-{version}-{host_suffix}.{ext}"
    files = {
        "VALIDATION.txt": (
            "Lightweight validation archive for release metadata checks.\n"
            f"source_tool_root={root}\n"
            "This archive intentionally omits the full toolchain payload.\n"
        )
    }
    return create_lightweight_archive(archive, f"{name}-{version}", files)


def artifact_info(base_url: str, path: Path) -> dict[str, str]:
    return {
        "url": f"{base_url}/{path.name}",
        "archiveFileName": path.name,
        "checksum": f"SHA-256:{sha256sum(path)}",
        "size": str(path.stat().st_size),
    }


def remote_artifact_info(url: str, archive_name: str, checksum: str, size: str) -> dict[str, str]:
    normalized_checksum = checksum if checksum.startswith("SHA-256:") else f"SHA-256:{checksum}"
    return {
        "url": url,
        "archiveFileName": archive_name,
        "checksum": normalized_checksum,
        "size": size,
    }


def archive_name_from_url(url: str) -> str:
    parsed = urlparse(url)
    name = Path(parsed.path).name
    if not name:
        raise SystemExit(f"Could not infer archive file name from URL: {url}")
    return name


def read_toolchain_entry(manifest: dict, name: str) -> tuple[str, dict]:
    entry = manifest.get(name)
    if not isinstance(entry, dict):
        raise SystemExit(f"Toolchain manifest missing entry for {name}")
    version = entry.get("version")
    if not isinstance(version, str) or not version.strip():
        raise SystemExit(f"Toolchain manifest entry {name} must define a non-empty version")
    systems = entry.get("systems")
    if not isinstance(systems, dict):
        raise SystemExit(f"Toolchain manifest entry {name} must define a systems object")
    return version, systems


def host_suffix_for_archive(host: str) -> str:
    return {
        "x86_64-linux-gnu": "linux64",
        "x86_64-mingw32": "win64",
        "x86_64-apple-darwin": "macos",
        "arm64-apple-darwin": "macos-arm64",
    }.get(host, host.replace("/", "-"))


def resolve_tool_artifact(
    *,
    output_dir: Path,
    name: str,
    version: str,
    host: str,
    manifest_entry: dict | None,
    root_arg: str,
    url_arg: str,
    archive_name_arg: str,
    checksum_arg: str,
    size_arg: str,
    lightweight: bool,
) -> tuple[Path | None, dict[str, str] | None]:
    manifest_entry = manifest_entry or {}
    root_value = root_arg or str(manifest_entry.get("root", "") or "")
    url_value = url_arg or str(manifest_entry.get("url", "") or "")
    archive_name_value = archive_name_arg or str(manifest_entry.get("archiveFileName", "") or "")
    checksum_value = checksum_arg or str(manifest_entry.get("checksum", "") or "")
    size_value = size_arg or str(manifest_entry.get("size", "") or "")

    if root_value and url_value:
        raise SystemExit(
            f"Toolchain metadata for {name} {host} is ambiguous; define either root or url, not both"
        )

    if url_value:
        missing = [
            var_name
            for var_name, value in (
                ("checksum", checksum_value),
                ("size", size_value),
            )
            if not value
        ]
        if missing:
            raise SystemExit(
                f"External tool metadata for {name} {host} is incomplete; missing: {', '.join(missing)}"
            )
        archive_name = archive_name_value or archive_name_from_url(url_value)
        return None, remote_artifact_info(url_value, archive_name, checksum_value, size_value)

    if not root_value:
        return None, None

    root = Path(root_value).resolve()
    if not root.exists():
        raise SystemExit(f"Tool root not found: {root}")

    host_suffix = host_suffix_for_archive(host)
    if lightweight:
        archive = create_host_tool_archive_lightweight(output_dir, name, version, host_suffix, root)
    else:
        archive = create_host_tool_archive(output_dir, name, version, host_suffix, root)
    return archive, None


def read_board_names(boards_txt: Path) -> list[dict[str, str]]:
    boards: list[dict[str, str]] = []
    for line in boards_txt.read_text(encoding="utf-8").splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#") or "=" not in stripped:
            continue
        key, value = stripped.split("=", 1)
        if key.endswith(".name"):
            boards.append({"name": value.strip()})
    if not boards:
        raise SystemExit(f"No board names found in {boards_txt}")
    return boards


def read_boards_target(boards_txt: Path) -> str | None:
    for line in boards_txt.read_text(encoding="utf-8").splitlines():
        stripped = line.strip()
        if stripped.startswith("# TARGET="):
            return stripped.split("=", 1)[1].strip()
    return None


def generate_package_index(
    target: str,
    version: str,
    base_url: str,
    platform_archive: Path,
    sdk_archive: Path,
    toolchain_version: str,
    toolchain_systems: dict[str, tuple[Path | None, dict[str, str] | None]],
    boards: list[dict[str, str]],
) -> dict:
    tool_name = sdk_tool_name(target)
    linux_archive, linux_info = toolchain_systems.get("x86_64-linux-gnu", (None, None))
    if linux_archive is None and linux_info is None:
        raise SystemExit("Linux ARM GCC metadata is required for release packaging")

    toolchain_system_entries = []
    for host, (archive, info) in toolchain_systems.items():
        if info is not None:
            toolchain_system_entries.append({"host": host, **info})
        elif archive is not None:
            toolchain_system_entries.append({"host": host, **artifact_info(base_url, archive)})

    return {
        "packages": [
            {
                "name": "beken",
                "maintainer": "Beken",
                "websiteURL": "https://github.com/bekencorp/bk_idk",
                "email": "support@example.com",
                "help": {"online": "https://github.com/bekencorp/bk_idk"},
                "platforms": [
                    {
                        "name": platform_display_name(target),
                        "architecture": target,
                        "version": version,
                        "category": "Beken",
                        **artifact_info(base_url, platform_archive),
                        "boards": boards,
                        "toolsDependencies": [
                            {
                                "packager": "beken",
                                "name": tool_name,
                                "version": version,
                            },
                            {
                                "packager": "beken",
                                "name": "arm-none-eabi-gcc",
                                "version": toolchain_version,
                            },
                        ],
                    }
                ],
                "tools": [
                    {
                        "name": tool_name,
                        "version": version,
                        "systems": [
                            {"host": "x86_64-linux-gnu", **artifact_info(base_url, sdk_archive)},
                            {"host": "x86_64-mingw32", **artifact_info(base_url, sdk_archive)},
                            {"host": "x86_64-apple-darwin", **artifact_info(base_url, sdk_archive)},
                            {"host": "arm64-apple-darwin", **artifact_info(base_url, sdk_archive)},
                        ],
                    },
                    {
                        "name": "arm-none-eabi-gcc",
                        "version": toolchain_version,
                        "systems": toolchain_system_entries,
                    },
                ],
            }
        ]
    }


def main() -> int:
    parser = argparse.ArgumentParser(description="Create Arduino Boards Manager artifacts for arduino-beken.")
    parser.add_argument("--platform-root", required=True, help="Path to the staged arduino-beken platform root")
    parser.add_argument("--sdk-root", required=True, help="Path to the exported Arduino SDK")
    parser.add_argument("--target", required=True, help="Target SoC, for example bk7239n")
    parser.add_argument("--version", required=True, help="Release version")
    parser.add_argument("--base-url", required=True, help="Public download URL prefix")
    parser.add_argument("--output-dir", required=True, help="Output directory")
    parser.add_argument(
        "--toolchain-manifest",
        default=str(DEFAULT_TOOLCHAIN_MANIFEST),
        help="Toolchain manifest JSON, default tools/toolchains.json under this repository",
    )
    parser.add_argument(
        "--toolchain-linux-root",
        default="",
        help="Linux ARM GCC toolchain root override",
    )
    parser.add_argument("--toolchain-linux-url", default="", help="External Linux ARM GCC archive URL override")
    parser.add_argument("--toolchain-linux-archive-name", default="", help="External Linux ARM GCC archive file name override")
    parser.add_argument("--toolchain-linux-checksum", default="", help="SHA-256 checksum for the external Linux ARM GCC archive override")
    parser.add_argument("--toolchain-linux-size", default="", help="Byte size for the external Linux ARM GCC archive override")
    parser.add_argument(
        "--toolchain-win-root",
        default="",
        help="Windows ARM GCC toolchain root override",
    )
    parser.add_argument("--toolchain-win-url", default="", help="External Windows ARM GCC archive URL override")
    parser.add_argument("--toolchain-win-archive-name", default="", help="External Windows ARM GCC archive file name override")
    parser.add_argument("--toolchain-win-checksum", default="", help="SHA-256 checksum for the external Windows ARM GCC archive override")
    parser.add_argument("--toolchain-win-size", default="", help="Byte size for the external Windows ARM GCC archive override")
    parser.add_argument("--toolchain-macos-x64-root", default="", help="macOS x86_64 ARM GCC toolchain root override")
    parser.add_argument("--toolchain-macos-x64-url", default="", help="External macOS x86_64 ARM GCC archive URL override")
    parser.add_argument("--toolchain-macos-x64-archive-name", default="", help="External macOS x86_64 ARM GCC archive file name override")
    parser.add_argument("--toolchain-macos-x64-checksum", default="", help="SHA-256 checksum for the external macOS x86_64 ARM GCC archive override")
    parser.add_argument("--toolchain-macos-x64-size", default="", help="Byte size for the external macOS x86_64 ARM GCC archive override")
    parser.add_argument("--toolchain-macos-arm64-root", default="", help="macOS arm64 ARM GCC toolchain root override")
    parser.add_argument("--toolchain-macos-arm64-url", default="", help="External macOS arm64 ARM GCC archive URL override")
    parser.add_argument("--toolchain-macos-arm64-archive-name", default="", help="External macOS arm64 ARM GCC archive file name override")
    parser.add_argument("--toolchain-macos-arm64-checksum", default="", help="SHA-256 checksum for the external macOS arm64 ARM GCC archive override")
    parser.add_argument("--toolchain-macos-arm64-size", default="", help="Byte size for the external macOS arm64 ARM GCC archive override")
    parser.add_argument(
        "--lightweight",
        action="store_true",
        help="Create small validation archives instead of packaging full SDK/toolchain/uploader payloads",
    )
    args = parser.parse_args()

    platform_root = Path(args.platform_root).resolve()
    if not platform_root.exists():
        raise SystemExit(f"Platform root not found: {platform_root}")

    output_dir = Path(args.output_dir).resolve()
    output_dir.mkdir(parents=True, exist_ok=True)

    sdk_root = Path(args.sdk_root).resolve()
    sdk_manifest = load_sdk_manifest(sdk_root)
    manifest_target = sdk_manifest.get("target", {}).get("soc")
    if manifest_target != args.target:
        raise SystemExit(
            f"Release target mismatch: --target={args.target} but SDK manifest says {manifest_target!r}"
        )

    boards_target = read_boards_target(platform_root / "boards.txt")
    if boards_target and boards_target != args.target:
        raise SystemExit(
            f"Release target mismatch: generated boards.txt is for {boards_target}, not {args.target}"
        )

    toolchain_manifest = load_toolchain_manifest(Path(args.toolchain_manifest).resolve())
    toolchain_version, toolchain_systems = read_toolchain_entry(toolchain_manifest, "arm-none-eabi-gcc")

    platform_archive = create_platform_archive(output_dir, platform_root, args.version, args.target)
    if args.lightweight:
        sdk_archive = create_sdk_archive_lightweight(output_dir, args.version, sdk_root, args.target)
    else:
        sdk_archive = create_sdk_archive(output_dir, args.version, sdk_root, args.target)

    toolchain_overrides = {
        "x86_64-linux-gnu": {
            "root": args.toolchain_linux_root,
            "url": args.toolchain_linux_url,
            "archive_name": args.toolchain_linux_archive_name,
            "checksum": args.toolchain_linux_checksum,
            "size": args.toolchain_linux_size,
        },
        "x86_64-mingw32": {
            "root": args.toolchain_win_root,
            "url": args.toolchain_win_url,
            "archive_name": args.toolchain_win_archive_name,
            "checksum": args.toolchain_win_checksum,
            "size": args.toolchain_win_size,
        },
        "x86_64-apple-darwin": {
            "root": args.toolchain_macos_x64_root,
            "url": args.toolchain_macos_x64_url,
            "archive_name": args.toolchain_macos_x64_archive_name,
            "checksum": args.toolchain_macos_x64_checksum,
            "size": args.toolchain_macos_x64_size,
        },
        "arm64-apple-darwin": {
            "root": args.toolchain_macos_arm64_root,
            "url": args.toolchain_macos_arm64_url,
            "archive_name": args.toolchain_macos_arm64_archive_name,
            "checksum": args.toolchain_macos_arm64_checksum,
            "size": args.toolchain_macos_arm64_size,
        },
    }
    resolved_toolchain_systems = {}
    for host in DEFAULT_TOOLCHAIN_HOSTS:
        archive, info = resolve_tool_artifact(
            output_dir=output_dir,
            name="arm-none-eabi-gcc",
            version=toolchain_version,
            host=host,
            manifest_entry=toolchain_systems.get(host),
            root_arg=toolchain_overrides[host]["root"],
            url_arg=toolchain_overrides[host]["url"],
            archive_name_arg=toolchain_overrides[host]["archive_name"],
            checksum_arg=toolchain_overrides[host]["checksum"],
            size_arg=toolchain_overrides[host]["size"],
            lightweight=args.lightweight,
        )
        resolved_toolchain_systems[host] = (archive, info)

    boards = read_board_names(platform_root / "boards.txt")
    package_index = generate_package_index(
        target=args.target,
        version=args.version,
        base_url=args.base_url.rstrip("/"),
        platform_archive=platform_archive,
        sdk_archive=sdk_archive,
        toolchain_version=toolchain_version,
        toolchain_systems=resolved_toolchain_systems,
        boards=boards,
    )

    json_path = output_dir / f"package_beken_{args.target}_index.json"
    json_path.write_text(json.dumps(package_index, indent=2) + "\n", encoding="utf-8")

    print(f"platform:  {platform_archive}")
    print(f"sdk:       {sdk_archive}")
    for host in DEFAULT_TOOLCHAIN_HOSTS:
        archive, info = resolved_toolchain_systems[host]
        if archive:
            print(f"toolchain[{host}]: {archive}")
        elif info:
            print(f"toolchain[{host}]: {info['url']}")
    print(f"json:      {json_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())