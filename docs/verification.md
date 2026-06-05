# arduino-beken Verification Guide

Language: English | [简体中文](verification_CN.md)

## Purpose
This document defines the local validation workflow for `arduino-beken`. It covers three main paths:

1. Direct `bk_idk` builds
2. Local `arduino-cli` hardware-folder builds
3. Lightweight Boards Manager release validation

## Prerequisites
Before running the checks, make sure that:

1. `bk_idk` can already build the target SoC locally
2. `arduino-cli` is installed
3. The ARM GCC toolchain is available at `/opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi`, or you are ready to override `compiler.path`
4. `.arduino-cli.yaml` and `examples/Blink/Blink.ino` exist in the repository
5. If you want to validate upload, a real board and serial device are available

The local Arduino CLI environment is fully scoped under `.arduino-cli-test/`, including `user/`, `data/`, `downloads/`, and `cache/`.
On the first run, Arduino CLI may download builtin indexes and builtin tools into that local directory.

The examples below assume:

```bash
export SDK_DIR=/path/to/bk_idk
cd /path/to/arduino-beken
```

## Validation matrix
The current minimum matrix is:

- `bk7239n`

When adding a new target, extend the same matrix to that target.

## 1. Validate the staged platform
Run this explicitly when you want to inspect or validate the staged Arduino platform itself:

```bash
make TARGET=<target> prepare-platform
```

After a fresh checkout, this is also the command that generates the staged `cores/arduino/` core wrappers. The repository no longer needs a checked-in `cores/` tree for CLI or release packaging.

Expected results:

1. `build/platform/<target>/arduino-beken/` is generated
2. `build/platform-cli/<target>/arduino-beken/` is generated for local Arduino CLI use
3. `build/platform/<target>/arduino-beken/boards.txt` contains `# TARGET=<target>`
4. `build/platform/<target>/arduino-beken/config/<target>/` is staged together with the target platform
5. `.arduino-cli-test/user/hardware/beken/<target>` points to the current target's CLI-only staged platform
6. `build/platform/<target>/arduino-beken/cores/arduino/` is generated entirely from `components/arduino-beken-core/` and `components/arduino-beken-main/`

If multiple `auto_partitions*.csv` files exist under `config/<target>/`, also check that staged `boards.txt` contains `menu.PartitionScheme`.

Notes:

1. Local `arduino-cli` tests depend on that symlink
2. When switching between targets, rerun `prepare-platform` if you are validating the staged platform directly
3. `build`, `build-base`, `export-sdk`, `release`, and `release-check` all run through the same core-wrapper generation step automatically
4. `cli-compile` and `cli-upload` also chain through `prepare-platform` automatically

Optional explicit core-wrapper check:

```bash
make TARGET=<target> check-core-wrappers
```

Why core wrappers exist: Arduino CLI expects standard core filenames under `cores/arduino/`, while implementations live under `components/`. Thin `#include` wrappers bridge the two without duplicating sources; see `docs/tools.md` § Core wrappers.

This Makefile target depends on `prepare-platform`, so it always validates the freshly staged tree under `build/platform/<target>/arduino-beken/cores/arduino/`.

To refresh an optional **repository-root** `cores/arduino/` tree after editing `tools/core_wrappers.py` (for local browsing or legacy tooling), run `make sync-core-wrappers`; that path is separate from the staged platform used by CLI and release packaging.

Expected result:

1. The command exits successfully
2. The staged `cores/arduino/` directory contains the full generated wrapper set
3. Each generated wrapper matches the manifest in `tools/core_wrappers.py`

## 2. Validate direct `bk_idk` builds
If you only want to validate the external component integration, run:

```bash
make SDK_DIR="$SDK_DIR" TARGET=<target> build-base
```

If you also want to validate sketch injection and final packaging, run:

```bash
make SDK_DIR="$SDK_DIR" TARGET=<target> build \
  SKETCH="$(pwd)/libraries/Blink/examples/Blink"
```

Expected results:

1. The build exits successfully
2. `build/<target>/arduino-beken/package/all-app.bin` is generated
3. `build/<target>/arduino-beken/<target>/app.elf` and `app.map` are generated

If the change touches linker-script or startup behavior, additionally check that `app.elf` exposes `.preinit_array` / `.init_array` / `.fini_array` and the real `__libc_init_array()`.

## 3. Validate SDK export
Run:

```bash
make SDK_DIR="$SDK_DIR" TARGET=<target> export-sdk
```

If you already have a maintained `dist/sdk/<target>` tree, you can also run:

```bash
make TARGET=<target> export-sdk
```

Expected results:

1. `dist/sdk/<target>/manifest.json` is generated
2. `manifest.json` reports the same SoC as `TARGET`
3. `dist/sdk/<target>/flags/` contains `defines`, `includes`, `c_flags`, `cpp_flags`, `ld_flags`, and `ld_libs`
4. `dist/sdk/<target>/artifacts/package/bootloader.bin` and the `partitions/` directory are exported
5. `dist/sdk/<target>/tools/partition_gen/` is exported for compile-time partition regeneration
6. For `bk7239n`, `dist/sdk/bk7239n/artifacts/package/sys_persist_config_4k.bin` is also exported

## 4. Validate `arduino-cli compile`
### `bk7239n`
```bash
make SDK_DIR="$SDK_DIR" TARGET=bk7239n cli-compile \
  CLI_SKETCH=libraries/Blink/examples/Blink
```

Expected results:

1. The compile step exits successfully
2. The build path contains `Blink.ino.elf`, `Blink.ino.bin`, `Blink.ino.all-app.bin`, and the legacy compatibility copy `all-app.bin`
3. The build path also contains `partitions/` and `bk_generated/include/` populated by the prebuild partition-generation step
4. Arduino CLI prints program size and global variable usage correctly
5. The old warnings below should no longer appear:
   - `missing --end-group; added as last command line option`
   - `Couldn't determine program size`

Additional notes:

1. `Low memory available` may still appear. That is now a real memory-usage warning, not a parser failure.
2. `cli-compile` already runs through `prepare-platform` and reuses the maintained SDK export tree through `export-sdk`, so users normally do not need to invoke either one manually first.
3. If you changed linker/export/startup code, inspect `Blink.ino.elf` and confirm `.init_array` exists together with `__libc_init_array`.
4. For `bk7239n`, `all-app.bin` must be regenerated after platform or SDK updates so the Arduino CLI packer can prepend `sys_persist_config_4k.bin` before upload.
5. If you need a stable custom output directory, override `CLI_BUILD_PATH=...`.
6. If you need the full compile, link, and packaging command stream, append `CLI_COMPILE_EXTRA_ARGS=--verbose`.
7. If multiple partition schemes exist, validate at least one non-default scheme with `--board-options PartitionScheme=<name>`.

Low-level reference command:

```bash
arduino-cli compile --clean \
  --config-file .arduino-cli.yaml \
  --fqbn beken:<target>:<target> \
  --board-options PartitionScheme=<name> \
  --build-path .arduino-cli-test/build-<target>-blink \
  --build-property compiler.sdk.path="$(pwd)/dist/sdk/<target>" \
  --build-property compiler.path=/opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/ \
  libraries/Blink/examples/Blink
```

Omit `--board-options PartitionScheme=<name>` when you want the default `auto_partitions.csv`.

## 5. Validate `arduino-cli upload`
This step requires real hardware and a serial device.

Preferred commands:

### `bk7239n`
```bash
make SDK_DIR="$SDK_DIR" TARGET=bk7239n cli-upload \
  CLI_SKETCH=libraries/Blink/examples/Blink \
  CLI_PORT=/dev/ttyUSB0
```

Expected results:

1. The wrapper eventually invokes `bk_loader download`
2. `Blink.ino.all-app.bin` (or the current sketch-specific `*.all-app.bin`) is used as the input image
3. The default baud rate is `2000000`
4. Upload completion passes `--reboot`
5. On local Linux x86_64 validation, no extra `--upload-property` override is needed once the CLI-only staged platform has been prepared
6. Since the uploader executables for all platforms are now embedded in the platform archive, the correct uploader for each platform is used automatically (e.g., `bk_loader.exe` on Windows, Linux binary on Linux, and native binaries on macOS).

Replace `/dev/ttyUSB0` with the actual device name on your machine.

Low-level reference command:

```bash
arduino-cli upload \
  --config-file .arduino-cli.yaml \
  --fqbn beken:<target>:<target> \
  --build-path .arduino-cli-test/build-<target>-blink \
  --port /dev/ttyUSB0 \
  libraries/Blink/examples/Blink
```

## 6. Validate `release-check`
Run for each target:

```bash
make SDK_DIR="$SDK_DIR" TARGET=<target> release-check
```

Expected results:

1. `dist/release-check/<target>/package_beken_<target>_index.json` is generated
2. `arduino-beken-<target>-<version>.tar.gz` is generated
3. The lightweight SDK archive is generated; toolchain entries reference external URLs from official sources rather than packaging the toolchain directly
4. `dist/release-check/<target>/staging-platform/arduino-beken/boards.txt` and `platform.txt` match the staged platform content
5. Since `tools/toolchains.json` already pre-defines GCC entries for all platforms (including Linux, macOS, and Windows), the package index also advertises `arm-none-eabi-gcc` for the corresponding platforms
6. The package index correctly lists only the platform archive, SDK archive, and toolchain dependencies (no separate uploader tool entries since uploader is now distributed as part of the platform)

## Recommended regression triggers
Run at least the full validation matrix again when changing:

1. `components/arduino-beken-core` or `components/arduino-beken-main`
2. `tools/export_sdk.py`
3. `tools/make_release.py`
4. `tools/gen_boards_txt.py`
5. `config/<target>` or `variants/<target>_*`
6. `platform.txt`

## Current baseline coverage
The current repository has already covered these baseline checks:

1. `bk7239n`: `prepare-platform`, `arduino-cli compile`, `arduino-cli upload`, and `release-check`
2. `bk7239n`: generated ELF inspection for `.preinit_array` / `.init_array` / `.fini_array` and `__libc_init_array`
3. `bk7239n`: Arduino CLI `all-app.bin` packaging verified with exported `sys_persist_config_4k.bin`

Before a formal release, it is still recommended to run at least one real `arduino-cli upload` and board boot validation pass.