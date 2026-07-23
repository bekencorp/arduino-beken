#!/usr/bin/env python3

from __future__ import annotations

import re
import shutil
from pathlib import Path

from core_wrappers import STAGED_CORE_PRUNED_FILES, generate_core_wrappers
from gen_boards_txt import generate_boards_txt


def sdk_tool_name(target: str) -> str:
    return f"arduino-beken-sdk-{target}"


def platform_display_name(target: str) -> str:
    return f"Arduino Beken {target.upper()}"


def copy_if_exists(src: Path, dst: Path) -> None:
    if not src.exists():
        return
    if src.is_dir():
        shutil.copytree(src, dst, dirs_exist_ok=True, ignore=shutil.ignore_patterns("__pycache__", "*.pyc"))
    else:
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(src, dst)


def copy_tools_tree(src: Path, dst: Path) -> None:
    if not src.exists():
        return
    shutil.copytree(
        src,
        dst,
        dirs_exist_ok=True,
        ignore=shutil.ignore_patterns("__pycache__", "*.pyc"),
    )
    macos_uploader = dst / "bk_loader" / "macos"
    if macos_uploader.exists():
        shutil.rmtree(macos_uploader)


def rewrite_platform_txt(
    platform_txt: Path,
    version: str,
    target: str,
    uploader_root: Path | None = None,
    uploader_os: str = "linux",
    line_overrides: dict[str, str] | None = None,
) -> None:
    tool_name = sdk_tool_name(target)
    tool_pattern = re.compile(r"arduino-beken-sdk(?:-[A-Za-z0-9_]+)?")
    uploader_path_key = f"tools.bk-uploader.path.{uploader_os}="
    overrides = dict(line_overrides or {})
    if uploader_root is not None:
        overrides[uploader_path_key] = str(uploader_root)
    lines = platform_txt.read_text(encoding="utf-8").splitlines()
    platform_txt.write_text(
        "\n".join(
            (
                f"name={platform_display_name(target)}"
                if line.startswith("name=")
                else f"version={version}"
                if line.startswith("version=")
                else next(
                    (f"{prefix}{value}" for prefix, value in overrides.items() if line.startswith(prefix)),
                    tool_pattern.sub(tool_name, line),
                )
            )
            for line in lines
        )
        + "\n",
        encoding="utf-8",
    )


def build_platform_tree(project_root: Path, platform_root: Path, version: str, target: str) -> Path:
    if platform_root.exists():
        shutil.rmtree(platform_root)
    platform_root.mkdir(parents=True, exist_ok=True)

    for name in ["platform.txt", "programmers.txt", "README.md"]:
        copy_if_exists(project_root / name, platform_root / name)

    copy_if_exists(project_root / "components", platform_root / "components")
    copy_if_exists(project_root / "config" / target, platform_root / "config" / target)
    copy_if_exists(project_root / "variants", platform_root / "variants")
    copy_if_exists(project_root / "libraries", platform_root / "libraries")
    copy_tools_tree(project_root / "tools", platform_root / "tools")

    # Native bk_idk builds link arduino-beken-main as its own component library.
    # Do not compile duplicate core wrappers under Arduino CLI.
    generate_core_wrappers(
        platform_root,
        platform_root / "cores" / "arduino",
        excluded=STAGED_CORE_PRUNED_FILES,
    )

    (platform_root / "boards.txt").write_text(generate_boards_txt(project_root, target), encoding="utf-8")
    rewrite_platform_txt(platform_root / "platform.txt", version, target)
    return platform_root
