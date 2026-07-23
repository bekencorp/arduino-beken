#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import shlex
import shutil
from pathlib import Path


CORE_ARCHIVE_TOKEN = "{build.core_archive}"
ARDUINO_LIB_DIR = "arduino_lib"
ARDUINO_LIBRARY_ARTIFACTS = (
    "libBLE.a",
    "libWiFi.a",
    "libArduinoBLE.a",
    "libWiFiClientSecure.a",
    "libHTTPClient.a",
    "libPubSubClient.a",
    "libDNSServer.a",
    "libWebsocket.a",
)
ARDUINO_LIBRARY_NAMES = frozenset(
    {
        "libBLE.a",
        "libWiFi.a",
        "libWiFiClientSecure.a",
        "libHTTPClient.a",
        "libPubSubClient.a",
        "libDNSServer.a",
        "libWebsocket.a",
    }
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Export an Arduino-facing SDK from a bk_idk build.")
    parser.add_argument("--sdk-dir", help="Path to bk_idk when refreshing the maintained SDK")
    parser.add_argument("--project-root", required=True, help="Path to the arduino-beken project")
    parser.add_argument("--build-root", help="Build root passed to bk_idk when refreshing the maintained SDK")
    parser.add_argument("--soc", required=True, help="Target SoC")
    parser.add_argument("--version", required=True, help="SDK version")
    parser.add_argument("--output", required=True, help="Export output directory")
    return parser.parse_args()


def is_relative_to(path: Path, parent: Path) -> bool:
    try:
        path.relative_to(parent)
        return True
    except ValueError:
        return False


def ensure_clean_dir(path: Path) -> None:
    if path.exists():
        shutil.rmtree(path)
    path.mkdir(parents=True, exist_ok=True)


def verify_exported_sdk(output_root: Path, soc: str) -> None:
    manifest_path = output_root / "manifest.json"
    required_paths = [
        manifest_path,
        output_root / "armino_as_lib" / "include",
        output_root / "flags" / "defines",
        output_root / "flags" / "includes",
        output_root / "include" / "generated" / "sdkconfig.h",
        output_root / "include" / "generated" / "partitions_gen.h",
    ]

    missing = [str(path) for path in required_paths if not path.exists()]
    if missing:
        raise SystemExit(
            "Maintained SDK artifacts are missing. "
            "Run export-sdk with SDK_DIR=/path/to/bk_idk to refresh them.\n"
            + "\n".join(f"- {path}" for path in missing)
        )

    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    manifest_soc = manifest.get("target", {}).get("soc")
    if manifest_soc != soc:
        raise SystemExit(
            f"Maintained SDK at {output_root} targets {manifest_soc!r}, not {soc!r}. "
            "Refresh it with export-sdk and the matching SDK_DIR."
        )


def read_build_vars(build_ninja: Path) -> tuple[str, str, str]:
    build_line = ""
    flags_line = ""
    link_libs_line = ""

    for line in build_ninja.read_text(encoding="utf-8").splitlines():
        if line.startswith("build app.elf:"):
            build_line = line
            continue
        if build_line and not flags_line and line.startswith("  FLAGS = "):
            flags_line = line[len("  FLAGS = "):]
            continue
        if build_line and not link_libs_line and line.startswith("  LINK_LIBRARIES = "):
            link_libs_line = line[len("  LINK_LIBRARIES = "):]
            break

    if not build_line or not flags_line or not link_libs_line:
        raise SystemExit(f"Unable to locate app.elf link metadata in {build_ninja}")

    return build_line, flags_line, link_libs_line


def load_compile_commands(path: Path) -> list[dict]:
    return json.loads(path.read_text(encoding="utf-8"))


def select_command(entries: list[dict], suffixes: tuple[str, ...], fallback_suffix: str) -> str:
    for suffix in suffixes:
        for entry in entries:
            if entry["file"].endswith(suffix):
                return entry["command"]
    for entry in entries:
        if entry["file"].endswith(fallback_suffix):
            return entry["command"]
    raise SystemExit(f"Unable to find a representative {fallback_suffix} compile command")


def normalize_define(token: str) -> str | None:
    if token.startswith("-D__FILE__="):
        return None
    if "mbedtls_psa_crypto_config.h" in token:
        return '-DMBEDTLS_CONFIG_FILE=\\"mbedtls_psa_crypto_config.h\\"'
    return token


def extract_compile_parts(command: str) -> tuple[list[str], list[Path], list[str]]:
    tokens = shlex.split(command)
    defines: list[str] = []
    includes: list[Path] = []
    flags: list[str] = []

    i = 1
    while i < len(tokens):
        token = tokens[i]

        if token.startswith("-D"):
            define = normalize_define(token)
            if define:
                defines.append(define)
            i += 1
            continue

        if token == "-I" and i + 1 < len(tokens):
            includes.append(Path(tokens[i + 1]).resolve())
            i += 2
            continue

        if token.startswith("-I"):
            includes.append(Path(token[2:]).resolve())
            i += 1
            continue

        if token in {"-c", "-o", "-MF", "-MT", "-MQ", "-x"} and i + 1 < len(tokens):
            i += 2
            continue

        if token in {"-MMD", "-MD", "-MP"}:
            i += 1
            continue

        # compile_commands.json from bk_idk carries some linker-only flags;
        # drop them here so Arduino compile recipes stay compiler-clean.
        if token.startswith("-Wl"):
            i += 1
            continue

        if token.startswith("-include"):
            i += 1
            continue

        if not token.startswith("-"):
            i += 1
            continue

        flags.append(token)
        i += 1

    return defines, includes, flags


def dedupe(items: list[str]) -> list[str]:
    result: list[str] = []
    seen: set[str] = set()
    for item in items:
        if item not in seen:
            seen.add(item)
            result.append(item)
    return result


def copy_tree(src: Path, dst: Path) -> None:
    if not src.exists():
        return
    shutil.copytree(src, dst, dirs_exist_ok=True)


def copy_file(src: Path, dst: Path) -> None:
    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(src, dst)


# Headers needed by WiFiClientSecure / Websocket under Arduino CLI/IDE.
# make/CMake gets these via component REQUIRES; export_sdk must package them.
TLS_INCLUDE_RELS = (
    "components/psa_mbedtls/mbedtls_port/configs",
    "components/psa_mbedtls/mbedtls_port/inc",
    "components/psa_mbedtls/mbedtls_port/mbedtls/include",
    "components/psa_mbedtls/mbedtls_ui",
    "components/psa_mbedtls/mbedtls/include",
    "components/os_source/freertos_v10/include",
    "components/os_source/freertos_v10/portable/GCC/ARM_CM33_NTZ/non_secure",
    "components/bk_rtos/freertos",
    "components/bk_rtos/include",
    "components/bk_startup/freertos",
    "middleware/arch/cm33/os/freertos",
)


def export_tls_headers(sdk_dir: Path, output_root: Path) -> list[str]:
    """Copy mbedtls + FreeRTOS public headers from bk_idk into the Arduino SDK."""
    include_flags: list[str] = []
    sdk_include_root = output_root / "include" / "sdk"
    ensure_clean_dir(sdk_include_root)
    for rel in TLS_INCLUDE_RELS:
        src = sdk_dir / rel
        if not src.exists():
            continue
        dst = sdk_include_root / rel
        dst.parent.mkdir(parents=True, exist_ok=True)
        copy_tree(src, dst)
        include_flags.append(f"-I{{compiler.sdk.path}}/include/sdk/{rel}")
    return include_flags


def export_generated_headers(project_build_dir: Path, sdk_dir: Path, soc: str, output_root: Path) -> list[dict[str, str]]:
    exported: list[dict[str, str]] = []
    generated_dir = output_root / "include" / "generated"
    generated_dir.mkdir(parents=True, exist_ok=True)

    candidates = [
        (project_build_dir / "partitions" / "partitions_gen.h", generated_dir / "partitions_gen.h"),
        (project_build_dir / "partitions" / "partitions.h", generated_dir / "partitions.h"),
        (project_build_dir / "partitions" / "bk_package.json", output_root / "partitions" / "bk_package.json"),
        (project_build_dir / "partitions" / "bk_ota_partitions.json", output_root / "partitions" / "bk_ota_partitions.json"),
        (project_build_dir / "partitions" / "partitions.csv", output_root / "partitions" / "partitions.csv"),
        (project_build_dir / "partitions" / "partitions.json", output_root / "partitions" / "partitions.json"),
        (project_build_dir / "partitions" / "partitions.txt", output_root / "partitions" / "partitions.txt"),
        # `artifacts/package/` only carries inputs used by `tools/ide/bk_builder.py` when assembling `all-app.bin`.
        (
            project_build_dir / "sys_persist_config" / "sys_persist_config_4k.bin",
            output_root / "artifacts" / "package" / "sys_persist_config_4k.bin",
        ),
        (project_build_dir / "security" / "bl2.bin", output_root / "security" / "bl2.bin"),
        (project_build_dir / "security" / "provision.bin", output_root / "security" / "provision.bin"),
        (project_build_dir / "security" / "tfm_s.bin", output_root / "security" / "tfm_s.bin"),
        (project_build_dir / "security" / "ppc_config.bin", output_root / "security" / "ppc_config.bin"),
        (
            sdk_dir / "components" / "bk_libs" / soc / "bootloader" / "normal_bootloader" / "bootloader.bin",
            output_root / "artifacts" / "package" / "bootloader.bin",
        ),
    ]

    config_sdkconfig = project_build_dir / soc / "config" / "sdkconfig.h"
    if config_sdkconfig.exists():
        candidates.insert(0, (config_sdkconfig, generated_dir / "sdkconfig.h"))

    for src, dst in candidates:
        if src.exists():
            copy_file(src, dst)
            exported.append({"source": str(src), "path": str(dst.relative_to(output_root))})

    return exported


def export_partition_tools(sdk_dir: Path, output_root: Path) -> list[dict[str, str]]:
    exported: list[dict[str, str]] = []
    generator_root = output_root / "tools" / "partition_gen"

    candidates = [
        (
            sdk_dir / "tools" / "build_tools" / "build_process" / "bk_build_auto_partition.py",
            generator_root / "build_process" / "bk_build_auto_partition.py",
        ),
        (
            sdk_dir / "tools" / "build_tools" / "build_process" / "bk_project.py",
            generator_root / "build_process" / "bk_project.py",
        ),
        (
            sdk_dir / "tools" / "build_tools" / "build_process" / "bk_sdk",
            generator_root / "build_process" / "bk_sdk",
        ),
        (
            sdk_dir / "tools" / "env_tools" / "bk_py_libs" / "bk_auto_partition",
            generator_root / "bk_py_libs" / "bk_auto_partition",
        ),
        (
            sdk_dir / "tools" / "env_tools" / "bk_py_libs" / "bk_flash_partiton",
            generator_root / "bk_py_libs" / "bk_flash_partiton",
        ),
        (
            sdk_dir / "tools" / "env_tools" / "bk_py_libs" / "bk_ota_partition",
            generator_root / "bk_py_libs" / "bk_ota_partition",
        ),
        (
            sdk_dir / "tools" / "env_tools" / "bk_py_libs" / "bk_packager",
            generator_root / "bk_py_libs" / "bk_packager",
        ),
        (
            sdk_dir / "tools" / "env_tools" / "bk_py_libs" / "bk_crc",
            generator_root / "bk_py_libs" / "bk_crc",
        ),
        (
            sdk_dir / "tools" / "env_tools" / "bk_py_libs" / "bk_misc",
            generator_root / "bk_py_libs" / "bk_misc",
        ),
    ]

    for src, dst in candidates:
        if not src.exists():
            continue
        if src.is_dir():
            copy_tree(src, dst)
        else:
            copy_file(src, dst)
        exported.append({"source": str(src), "path": str(dst.relative_to(output_root))})

    return exported


def is_arduino_adaptation_library(source: Path, project_lib: str | None) -> bool:
    if project_lib == "library":
        return True
    return source.name in ARDUINO_LIBRARY_NAMES


def classify_export_path(
    source: Path,
    cmake_build_dir: Path,
    sdk_dir: Path,
    output_root: Path,
    project_lib: str | None = None,
) -> Path:
    if is_arduino_adaptation_library(source, project_lib):
        return output_root / ARDUINO_LIB_DIR / source.name
    if is_relative_to(source, cmake_build_dir / "armino"):
        return output_root / "armino_as_lib" / cmake_build_dir.name / "libs" / source.name
    if is_relative_to(source, sdk_dir / "components" / "bk_libs"):
        return output_root / "bk_libs" / source.name
    if is_relative_to(source, sdk_dir):
        rel = source.relative_to(sdk_dir)
        return output_root / "lib" / "bk_idk" / rel
    return output_root / "lib" / "external" / source.name


def resolve_bk_lib_export_source(
    source: Path,
    sdk_dir: Path,
    project_root: Path,
    soc: str,
    cmake_build_dir: Path | None = None,
) -> Path:
    """Prefer build armino_as_lib output, then third_party_patches, then vendor bk_libs."""
    bk_libs_soc = sdk_dir / "components" / "bk_libs" / soc
    if not is_relative_to(source, bk_libs_soc):
        return source
    if cmake_build_dir is not None:
        built = cmake_build_dir / "armino_as_lib" / soc / "libs" / source.name
        if built.is_file():
            return built
    patch_root = project_root / "third_party_patches" / "bk_idk" / "bk_libs" / soc
    if not patch_root.is_dir():
        return source
    rel = source.relative_to(bk_libs_soc)
    mirrored = patch_root / rel
    if mirrored.is_file():
        return mirrored
    flat = patch_root / source.name
    if flat.is_file():
        return flat
    return source


def prune_copied_arduino_libs(output_root: Path, soc: str) -> None:
    """Remove Arduino adaptation libraries from the bulk armino_as_lib copy."""
    libs_dir = output_root / "armino_as_lib" / soc / "libs"
    if not libs_dir.is_dir():
        return
    for name in ARDUINO_LIBRARY_ARTIFACTS:
        path = libs_dir / name
        if path.is_file():
            path.unlink()


def classify_project_library(source: Path) -> str | None:
    if "arduino-beken-core" in source.parts:
        return "core"
    if "arduino-beken-main" in source.parts:
        return "main"
    if "arduino-beken-linker" in source.parts:
        return "linker"
    if "libraries" in source.parts:
        return "library"
    return None


def export_link_assets(
    build_line: str,
    flags_line: str,
    link_libs_line: str,
    cmake_build_dir: Path,
    sdk_dir: Path,
    project_root: Path,
    soc: str,
    output_root: Path,
) -> tuple[list[str], list[str], list[dict[str, str]], list[dict[str, str]]]:
    ld_flags = shlex.split(flags_line)
    link_tokens = shlex.split(link_libs_line)
    build_tokens = shlex.split(build_line)
    extra_lib_tokens = [token for token in build_tokens if token.endswith(".a") and token not in link_tokens]

    if extra_lib_tokens:
        insert_at = link_tokens.index("-T") if "-T" in link_tokens else len(link_tokens)
        link_tokens[insert_at:insert_at] = extra_lib_tokens

    ld_libs: list[str] = []
    exported_libs: list[dict[str, str]] = []
    exported_ld: list[dict[str, str]] = []

    script_lookup: dict[str, Path] = {}
    for token in build_tokens:
        if token.endswith(".ld"):
            if token.startswith("/"):
                script_lookup[Path(token).name] = Path(token)
            else:
                script_lookup[Path(token).name] = (cmake_build_dir / token).resolve()

    last_search_dir: Path | None = None
    i = 0
    while i < len(link_tokens):
        token = link_tokens[i]

        if token == "-L" and i + 1 < len(link_tokens):
            path = Path(link_tokens[i + 1]).resolve()
            if is_relative_to(path, cmake_build_dir):
                last_search_dir = path
            i += 2
            continue

        if token == "-T" and i + 1 < len(link_tokens):
            script_name = link_tokens[i + 1]
            script_src = Path(script_name)
            if not script_src.is_absolute():
                if last_search_dir is not None:
                    script_src = (last_search_dir / script_name).resolve()
                else:
                    script_src = script_lookup.get(script_name, cmake_build_dir / script_name)
            script_dst = output_root / "ld" / script_src.name
            copy_file(script_src, script_dst)
            ld_libs.extend(["-T", f"{{compiler.sdk.path}}/{script_dst.relative_to(output_root)}"])
            exported_ld.append({"source": str(script_src), "path": str(script_dst.relative_to(output_root))})
            i += 2
            continue

        if token.endswith(".a"):
            source = Path(token)
            if not source.is_absolute():
                source = (cmake_build_dir / token).resolve()
            project_lib = classify_project_library(source)
            if project_lib == "core":
                ld_libs.append(CORE_ARCHIVE_TOKEN)
                i += 1
                continue
            dst = classify_export_path(source, cmake_build_dir, sdk_dir, output_root, project_lib)
            copy_src = resolve_bk_lib_export_source(source, sdk_dir, project_root, soc, cmake_build_dir)
            copy_file(copy_src, dst)
            ld_libs.append(f"{{compiler.sdk.path}}/{dst.relative_to(output_root)}")
            exported_libs.append({"source": str(copy_src), "path": str(dst.relative_to(output_root))})
            i += 1
            continue

        if token.startswith("-Wl,--print-memory-usage,--Map="):
            ld_libs.append("-Wl,--print-memory-usage,--Map={build.path}/{build.project_name}.map")
            i += 1
            continue

        ld_libs.append(token)
        i += 1

    return ld_flags, ld_libs, exported_libs, exported_ld


def write_flag_file(path: Path, values: list[str]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(values) + "\n", encoding="utf-8")


def main() -> int:
    args = parse_args()

    sdk_dir = Path(args.sdk_dir).resolve() if args.sdk_dir else None
    project_root = Path(args.project_root).resolve()
    output_root = Path(args.output).resolve()

    if sdk_dir is None:
        verify_exported_sdk(output_root, args.soc)
        print(f"Using maintained SDK: {output_root}")
        return 0

    if not args.build_root:
        raise SystemExit("--build-root is required when --sdk-dir is provided")

    build_root = Path(args.build_root).resolve()
    project_name = project_root.name
    project_build_dir = build_root / args.soc / project_name
    cmake_build_dir = project_build_dir / args.soc
    compile_commands_path = cmake_build_dir / "compile_commands.json"
    build_ninja_path = cmake_build_dir / "build.ninja"
    armino_as_lib_dir = cmake_build_dir / "armino_as_lib"

    if not compile_commands_path.exists():
        raise SystemExit(f"compile_commands.json not found: {compile_commands_path}")
    if not build_ninja_path.exists():
        raise SystemExit(f"build.ninja not found: {build_ninja_path}")

    ensure_clean_dir(output_root)

    compile_commands = load_compile_commands(compile_commands_path)
    c_command = select_command(
        compile_commands,
        ("/components/arduino-beken-main/main.c", "/components/arduino-beken-main/init_stub.c"),
        ".c",
    )
    cpp_command = select_command(
        compile_commands,
        ("/components/arduino-beken-core/src/Arduino.cpp", "/components/arduino-beken-core/src/SketchDefaults.cpp"),
        ".cpp",
    )

    c_defines, _, c_flags = extract_compile_parts(c_command)
    cpp_defines, _, cpp_flags = extract_compile_parts(cpp_command)

    if armino_as_lib_dir.exists():
        copy_tree(armino_as_lib_dir, output_root / "armino_as_lib")
        prune_copied_arduino_libs(output_root, args.soc)

    generated = export_generated_headers(project_build_dir, sdk_dir, args.soc, output_root)
    partition_tools = export_partition_tools(sdk_dir, output_root)

    build_line, flags_line, link_libs_line = read_build_vars(build_ninja_path)
    ld_flags, ld_libs, exported_libs, exported_ld = export_link_assets(
        build_line=build_line,
        flags_line=flags_line,
        link_libs_line=link_libs_line,
        cmake_build_dir=cmake_build_dir,
        sdk_dir=sdk_dir,
        project_root=project_root,
        soc=args.soc,
        output_root=output_root,
    )

    write_flag_file(output_root / "flags" / "defines", dedupe(c_defines + cpp_defines))
    write_flag_file(output_root / "flags" / "c_flags", dedupe(c_flags))
    write_flag_file(output_root / "flags" / "cpp_flags", dedupe(cpp_flags))
    write_flag_file(output_root / "flags" / "S_flags", dedupe(c_flags))
    tls_include_flags = export_tls_headers(sdk_dir, output_root)
    include_flags = [
        *tls_include_flags,
        "-I{compiler.sdk.path}/armino_as_lib/include",
        "-I{compiler.sdk.path}/include/generated",
    ]
    write_flag_file(output_root / "flags" / "includes", include_flags)
    write_flag_file(output_root / "flags" / "ld_flags", ld_flags)
    write_flag_file(output_root / "flags" / "ld_libs", ld_libs)

    def to_relative_path(abs_path: str, base_path: Path) -> str:
        try:
            # Convert to absolute paths first to ensure proper comparison
            abs_path_obj = Path(abs_path).resolve()
            base_path_abs = base_path.resolve()
            
            # Try to make path relative to base
            relative_path = abs_path_obj.relative_to(base_path_abs)
            return str(relative_path)
        except ValueError:
            # If abs_path is not within base_path, try to create a relative path from base to abs_path
            # Calculate the relative path between two absolute paths
            abs_parts = abs_path_obj.parts
            base_parts = base_path_abs.parts
            
            # Find common prefix
            common_len = 0
            for part1, part2 in zip(abs_parts, base_parts):
                if part1 == part2:
                    common_len += 1
                else:
                    break
            
            # Calculate how many levels up we need to go from base_path
            up_levels = len(base_parts) - common_len
            # Add the remaining parts of the target path
            down_parts = abs_parts[common_len:]
            
            # Create relative path: go up then down
            relative_parts = ['..'] * up_levels + list(down_parts)
            return str(Path(*relative_parts))

    # Convert absolute paths to relative paths with respect to output_root
    manifest = {
        "name": project_name,
        "version": args.version,
        "target": {"soc": args.soc},
        "paths": {
            "sdk_dir": to_relative_path(str(sdk_dir), output_root),
            "project_root": to_relative_path(str(project_root), output_root),
            "build_root": to_relative_path(str(build_root), output_root),
            "project_build_dir": to_relative_path(str(project_build_dir), output_root),
            "cmake_build_dir": to_relative_path(str(cmake_build_dir), output_root),
        },
        "exports": {
            "root": ".",  # Root is the current directory of the manifest
            "generated": [{"source": to_relative_path(item["source"], output_root), "path": item["path"]} for item in generated],
            "partition_tools": [{"source": to_relative_path(item["source"], output_root), "path": item["path"]} for item in partition_tools],
            "libraries": [{"source": to_relative_path(item["source"], output_root), "path": item["path"]} for item in exported_libs],
            "ld": [{"source": to_relative_path(item["source"], output_root), "path": item["path"]} for item in exported_ld],
            "armino_as_lib": "armino_as_lib" if armino_as_lib_dir.exists() else None,
            "arduino_lib": ARDUINO_LIB_DIR if (output_root / ARDUINO_LIB_DIR).exists() else None,
        },
        "sources": {
            "compile_commands": to_relative_path(str(compile_commands_path), output_root),
            "build_ninja": to_relative_path(str(build_ninja_path), output_root),
        },
    }
    (output_root / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")

    print(f"Exported SDK: {output_root}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())