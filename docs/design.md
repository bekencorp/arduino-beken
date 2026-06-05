# arduino-beken Design

Language: English | [简体中文](design_CN.md)

## Goals
`arduino-beken` is an Arduino external project directory built around `bk_idk`. Its goals are:

1. Build Arduino sketches through `bk_idk` with `make`, and generate a flashable `all-app.bin`.
2. Maintain an Arduino-facing SDK export tree that can be consumed without a local `bk_idk` checkout, and refresh it from `bk_idk` only when core or platform inputs change.
3. Produce Boards Manager release artifacts while keeping target-specific differences inside the generation pipeline.
4. Keep adaptation logic inside `arduino-beken` and minimize changes to `bk_idk`.

## Repository layout
The main directories are:

| Path | Purpose |
| --- | --- |
| `components/arduino-beken-core` | Arduino core sources used by the `bk_idk` build |
| `components/arduino-beken-main` | Arduino entry and initialization stubs |
| `components/arduino-beken-linker` | Linker component that patches linker scripts to support C++ constructor tables |
| `variants/<target>_*` | Arduino board variants; at least one `<target>` is expected |
| `config/<target>` | SoC-specific project config and partition config |
| `tools/` | Sketch preprocessing, platform staging, SDK export, and release scripts |
| `build/platform/<target>/arduino-beken` | Release-facing target-specific staged platform consumed by SDK export and release packaging |
| `build/platform-cli/<target>/arduino-beken` | Local Arduino CLI-only staged platform; by default rewrites `tools.bk-uploader.path` to the staged Linux uploader (use `stage_platform.py --cli-uploader-root` to point at `tools/bk_loader/macos/...` when testing upload on macOS) |
| `dist/sdk/<target>` | Exported Arduino SDK |
| `dist/release*` | Release outputs and lightweight validation outputs |

## Core flows
### 1. `make build`
When building a sketch through `bk_idk`, the flow is:

1. `tools/sketch_prep.py` merges the primary `.ino` and sibling `.ino` files into `build/generated/sketch.cpp`, and emits wrapper translation units for sibling `.cpp` / `.c` files in the same sketch directory.
2. `make` invokes `bk_idk` with `PROJECT_DIR` pointing to the current `arduino-beken`.
3. `bk_idk` builds `arduino-beken-core` and `arduino-beken-main` as external components.
4. The packaging step emits `app.bin` and `all-app.bin`.
5. `user_app_main()` runs the real `__libc_init_array()` after `bk_init()` has brought up the BK runtime, so sketch and library global constructors execute before `setup()` without running too early in `main()`.

This path validates direct Arduino sketch builds through `bk_idk`.

### 2. `make export-sdk`
`export-sdk` maintains the Arduino CLI/IDE SDK export tree under `dist/sdk/<target>`.

1. When run without `SDK_DIR`, it verifies and reuses the maintained export tree so common CLI and release flows do not need a `bk_idk` checkout.
2. When run with `SDK_DIR=/path/to/bk_idk`, it refreshes the maintained export tree from the current upstream build outputs.

The exported tree includes:

1. `armino_as_lib` and its public include surface under `armino_as_lib/include`.
2. Generated headers such as `sdkconfig.h`, `partitions_gen.h`, and `partitions.h`.
3. Linker scripts, compile flags, and link flags.
4. Bootloader, partition metadata, and package-related artifacts needed by Arduino-side image generation.
5. Arduino library directory `arduino_lib` containing libraries such as `libBLE.a`, `libWiFi.a`, etc.
6. Target-specific packaging inputs such as `sys_persist_config_4k.bin` when the SoC requires them.
7. A minimal upstream partition-generation runtime so Arduino CLI / IDE can regenerate partition outputs from `config/<target>/auto_partitions*.csv` at compile time.

The maintained SDK tree is what Arduino CLI/IDE and release packaging consume by default, so the common path no longer depends on a local `bk_idk` tree at compile time.
When the tree is refreshed from `bk_idk`, the linker script is still generated upstream and then patched by `arduino-beken` to restore `.preinit_array` / `.init_array` / `.fini_array` without carrying a forked SoC linker script.
For `bk7239n`, the exported SDK also carries `artifacts/package/sys_persist_config_4k.bin`, and the Arduino CLI packer prepends it to `app.bin` before building `all-app.bin` so the flashed image matches the `bk_idk` packaging path.
For Arduino CLI/IDE builds, the staged platform uses prebuild hooks for two Arduino-specific generation steps:

1. Generate partition headers and package metadata from the selected `auto_partitions*.csv`.
2. Generate a sketch-specific `GeneratedSketch_<hash>.cpp` wrapper inside the build's `core/` directory.

This keeps partition selection build-local and keeps `setup()/loop()` inside `core.a` while still forcing the core archive to change when the sketch path changes.

### 3. `make prepare-platform`
`prepare-platform` builds a target-aware Arduino platform tree:

1. Copies source-controlled platform content such as `components`, `variants`, `config/<target>`, and `tools`.
2. Generates `boards.txt` through `tools/gen_boards_txt.py`.
3. Generates `cores/arduino/` core wrapper files inside the staged tree from `components/arduino-beken-core/` and `components/arduino-beken-main/`.
4. Rewrites `platform.txt` for the selected target, including display name, version, and SDK tool name.
5. Writes the release-facing staged result to `build/platform/<target>/arduino-beken/`.
6. Clones that staged result into `build/platform-cli/<target>/arduino-beken/` for local Arduino CLI use.
7. Rewrites only `tools.bk-uploader.path` in the CLI-only copy so `arduino-cli upload` uses its staged `tools/bk_loader/linux` tree by default (optional `--cli-uploader-root` for another checked-in tree such as `tools/bk_loader/macos/arm64`).
8. Carries a prebuild hook that regenerates partition artifacts from the active `build.partitions_csv` before Arduino CLI/IDE compilation starts.
9. Carries a prebuild hook that generates a sketch-specific wrapper under the build's `core/` directory before Arduino CLI/IDE compilation starts.
10. Updates the local `.arduino-cli-test/user/hardware/beken/<target>` symlink to point at the CLI-only staged platform.

Target-specific `boards.txt` exists only inside the staged platform copies and is not stored in the repository root.

### 4. `make release` / `make release-check`
The release flow directly consumes the staged platform:

1. `release` and `release-check` depend on `export-sdk`, but in the common path `export-sdk` only validates the maintained SDK tree.
2. `tools/make_release.py` reads `boards.txt` and `platform.txt` from `build/platform/<target>/arduino-beken/`.
3. It generates target-specific platform archives, SDK archives, and package index metadata with toolchain references via URLs.
4. It emits `package_beken_<target>_index.json`.

`release-check` differs from a full release in two ways:

1. It uses lightweight placeholder archives instead of full SDK / toolchain / uploader payloads.
2. It is intended for fast validation of metadata, archive naming, and package index structure.

## Target and variant model
### Target
`TARGET` is the SoC name, for example `bk7239n`. It affects:

1. The selected `config/<TARGET>` directory.
2. The variant scan scope under `variants/`.
3. Generated `boards.txt` and `platform.txt` in the staged platform.
4. Target-specific output names such as `dist/sdk/<TARGET>` and `package_beken_<TARGET>_index.json`.

### Variant
`VARIANT` optionally overrides the selected board variant for make-driven builds. If it is not provided, the default preference order is:

1. `variants/<target>_generic`
2. Other `variants/<target>_*`

Variant resolution is centralized in `cmake/arduino_variant.cmake`. In most cases, adding a new target does not require changing that file.

## Generated versus source-controlled content
The current design deliberately separates generated outputs from checked-in sources:

1. Source-controlled content remains in the repository root, including `components/`, `variants/`, `tools/`, and `docs/`.
2. Release-facing target-specific platform content lives under `build/platform/<target>/arduino-beken/`.
3. Local CLI-only platform content lives under `build/platform-cli/<target>/arduino-beken/`.
4. The maintained SDK export tree lives under `dist/sdk/<target>/` and is reused by default.
5. The release flow consumes the release-facing staged platform instead of reassembling another platform tree from the repository root.

Benefits of this split:

1. Generated `boards.txt` no longer lives in the repository root when the target changes.
2. Thin `cores/arduino/` core wrappers cannot drift from `components/arduino-beken-core/` because they are regenerated each time.
3. Local CLI upload keeps using the checked-in uploader tree without depending on `bk_idk` (defaults to the Linux tree; macOS trees live under `tools/bk_loader/macos/` for release and optional CLI overrides).
4. The default CLI/release path now consumes the maintained SDK export tree without needing `bk_idk`.
5. Release packaging still consumes a deterministic target-specific platform tree.
6. It is easier to diff actual target outputs.

## How to add a new target
The following steps apply when introducing a new SoC target.

### 1. Confirm upstream `bk_idk` support
First verify that the target already exists in upstream `bk_idk`:

```bash
make -C /path/to/bk_idk <target> PROJECT_DIR=/path/to/arduino-beken BUILD_DIR=/tmp/arduino-beken-build
```

If `bk_idk` itself does not expose that target, `arduino-beken` cannot add it on its own.

### 2. Add `config/<target>`
At minimum, provide:

1. `config/<target>/config`
2. `config/<target>/auto_partitions.csv`
3. Optional `config/<target>/auto_partitions_<name>.csv` or `config/<target>/auto_partitions.<name>.csv`
4. `config/<target>/partitions.csv`

These files drive:

1. `bk_idk` build configuration.
2. `upload.maximum_size` in generated `boards.txt`.
3. `build.flash_size` in generated `boards.txt`.
4. Optional Arduino `Partition Scheme` menu entries.
5. Packaging and OTA partition metadata.

### 3. Add `variants/<target>_generic`
At least one generic variant is required:

1. `variants/<target>/pins_arduino.h`
2. Any other board-level definition files needed by that variant

If you later support multiple boards, add more variant directories such as:

1. `variants/<target>_board_a`
2. `variants/<target>_board_b`

`boards.txt` will pick them up automatically during `prepare-platform`.

### 4. Check for target-specific core adaptation
If the new SoC differs in driver constants, register layout, or capability model, review:

1. `components/arduino-beken-core/src/Arduino.cpp`
2. `components/arduino-beken-core/include/*`
3. Other code paths guarded by SoC-specific macros

Prefer conditional compilation over copying the entire core.

### 5. Review target-specific board metadata
`tools/gen_boards_txt.py` currently auto-generates:

1. `upload.maximum_size`
2. `build.flash_size`
3. `build.partitions_csv`
4. Optional `menu.PartitionScheme.*`
5. Board display names
6. Default upload speed

If the new target needs target-specific values for:

1. `upload.maximum_data_size`
2. `build.f_cpu`
3. Default upload speed

add target-aware mappings in `tools/gen_boards_txt.py` instead of editing generated output by hand.

### 6. Run the validation matrix
At minimum, run:

```bash
make TARGET=<target> prepare-platform
make SDK_DIR=/path/to/bk_idk TARGET=<target> build-base
make SDK_DIR=/path/to/bk_idk TARGET=<target> cli-compile \
  CLI_SKETCH=examples/Blink
make SDK_DIR=/path/to/bk_idk TARGET=<target> release-check
```

### 7. Update both language tracks of the documentation
After adding a new target, update both the English and Chinese documents:

1. `README.md` and `README_CN.md`
2. `docs/design.md` and `docs/design_CN.md`
3. `docs/verification.md` and `docs/verification_CN.md`
4. `docs/release.md` and `docs/release_CN.md`

## Current recommendations
For future expansion, keep following these two rules:

1. Keep target-specific differences concentrated in `config/<target>`, `variants/<target>_*`, and small SoC-specific conditionals.
2. Keep generated content under `build/` and `dist/`, not in the source tree.