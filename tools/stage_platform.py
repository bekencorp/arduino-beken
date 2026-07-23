#!/usr/bin/env python3

from __future__ import annotations

import argparse
import shutil
import sys
from pathlib import Path

from platform_tree import build_platform_tree, rewrite_platform_txt


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Stage an Arduino Beken platform tree under build/.")
    parser.add_argument("--project-root", required=True, help="Path to the arduino-beken project root")
    parser.add_argument("--target", required=True, help="Target SoC, for example bk7239n")
    parser.add_argument("--version", required=True, help="Platform version written into platform.txt")
    parser.add_argument("--output-dir", required=True, help="Output platform directory, usually build/platform/<target>/arduino-beken")
    parser.add_argument(
        "--cli-output-dir",
        default="",
        help="Optional local Arduino CLI-only platform directory with tool path overrides",
    )
    parser.add_argument(
        "--cli-link",
        default="",
        help="Optional hardware symlink path for the local Arduino CLI test environment",
    )
    parser.add_argument(
        "--cli-uploader-root",
        default="",
        help="Optional absolute uploader root injected only into the CLI-only staged platform",
    )
    parser.add_argument(
        "--cli-uploader-os",
        choices=("linux", "windows", "macosx"),
        default="",
        help="platform.txt uploader path key to override (default: macosx on macOS, else linux)",
    )
    return parser.parse_args()


def replace_path_with_symlink(link_path: Path, target_path: Path) -> None:
    link_path.parent.mkdir(parents=True, exist_ok=True)
    if link_path.is_symlink() or link_path.is_file():
        link_path.unlink()
    elif link_path.exists():
        shutil.rmtree(link_path)
    link_path.symlink_to(target_path)


def remove_legacy_cli_link(cli_link: Path) -> None:
    for legacy_link in (cli_link.parent / "bk", cli_link.parent.parent / "bekencorp"):
        if legacy_link == cli_link:
            continue
        if legacy_link.is_symlink() or legacy_link.is_file():
            legacy_link.unlink()
        elif legacy_link.exists():
            shutil.rmtree(legacy_link)


def main() -> int:
    args = parse_args()
    project_root = Path(args.project_root).resolve()
    output_dir = Path(args.output_dir).resolve()
    build_platform_tree(project_root, output_dir, args.version, args.target)
    print(f"Staged platform: {output_dir}")

    cli_platform_dir = output_dir
    if args.cli_output_dir:
        cli_platform_dir = Path(args.cli_output_dir).resolve()
        if cli_platform_dir.exists():
            shutil.rmtree(cli_platform_dir)
        shutil.copytree(output_dir, cli_platform_dir)

        uploader_root: Path | None = None
        uploader_os = args.cli_uploader_os or ("macosx" if sys.platform == "darwin" else "linux")
        if args.cli_uploader_root:
            uploader_root = Path(args.cli_uploader_root).resolve()
        else:
            default_name = "linux" if uploader_os == "linux" else "windows" if uploader_os == "windows" else ""
            if default_name:
                default_cli_uploader_root = cli_platform_dir / "tools" / "bk_loader" / default_name
                if default_cli_uploader_root.exists():
                    uploader_root = default_cli_uploader_root

        if uploader_root is not None:
            if not uploader_root.exists():
                raise SystemExit(f"CLI uploader root not found: {uploader_root}")

        # Local CLI tests do not install Boards Manager tools (env-python / bk_uploader).
        # Keep the release platform.txt for packaging, but override paths for compile/upload.
        cli_overrides: dict[str, str] = {
            "tools.python.cmd.linux=": "python3",
        }
        rewrite_platform_txt(
            cli_platform_dir / "platform.txt",
            args.version,
            args.target,
            uploader_root=uploader_root,
            uploader_os=uploader_os,
            line_overrides=cli_overrides,
        )

        print(f"Staged CLI platform: {cli_platform_dir}")

    if args.cli_link:
        cli_link = Path(args.cli_link)
        if not cli_link.is_absolute():
            cli_link = (Path.cwd() / cli_link).resolve()
        replace_path_with_symlink(cli_link, cli_platform_dir)
        remove_legacy_cli_link(cli_link)
        print(f"Updated CLI hardware link: {cli_link} -> {cli_platform_dir}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())