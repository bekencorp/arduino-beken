# arduino-beken Release Guide

Language: English | [简体中文](release_CN.md)

## Purpose
This document explains how to generate Boards Manager release artifacts for `arduino-beken` and how to validate them before a formal release.

## Release prerequisites
Before starting, confirm that:

1. Your `bk_idk` checkout can build the target locally
2. `arduino-beken/VERSION` has been updated to the intended release version
3. The platform archive already includes the uploader executables for all platforms, no additional preparation needed.
4. `tools/toolchains.json` has been reviewed and matches the intended release policy
5. If the manifest uses local packaging for Linux, the Linux ARM GCC toolchain is available at the configured `root`
6. If the release must support Windows Boards Manager compilation and the manifest uses local packaging there too, the configured Windows ARM GCC root is also available
7. The final HTTP/HTTPS hosting location for release artifacts is known

## Release model
The current release model is target-split:

1. One platform archive per target (containing platform files, libraries, uploader executables, etc.)
2. One SDK archive per target
3. One package index per target, for example:
   - `package_beken_bk7239n_index.json`

The platform archive includes the uploader executables for all platforms. The uploader is distributed as part of the platform, not as a separate tool.

This means the current implementation is best suited for one package-index URL per target.  
If a future release needs a single index containing multiple targets, `tools/make_release.py` must be extended.

## Toolchain metadata model
The toolchain metadata is now repository-managed through a template-driven manifest owned by this repository.

The default manifest is:

```text
tools/toolchains.json
```

It defines the Arduino Boards Manager metadata for `arm-none-eabi-gcc` per host, which is included in the package index. Each host entry can use one of two modes:

1. `root`: get toolchain information from a local directory during `make release`
2. `url` + `checksum` + `size`: directly reference an already published archive in the generated package index for Arduino IDE/CLI to download

The release command reads that manifest by default, so normal releases do not need long toolchain-related command lines.

## Recommended release flow
### 1. Run lightweight validation first
For each target, run:

```bash
make SDK_DIR=/path/to/bk_idk TARGET=<target> release-check
```

This step:

1. Regenerates the staged platform
2. Exports `dist/sdk/<target>`
3. Produces a full release layout under `dist/release-check/<target>/` using lightweight archives
4. Quickly validates package index metadata, naming, and archive structure without packaging large payloads

For the currently supported targets, the recommended baseline is:

```bash
make SDK_DIR=/path/to/bk_idk TARGET=bk7239n release-check
```

### 2. Run the full release packaging step
After lightweight validation passes, package the formal release for each target:

```bash
make SDK_DIR=/path/to/bk_idk TARGET=<target> release \
  BASE_URL=https://example.com/arduino-beken/<target>
```

Example:

```bash
make SDK_DIR=/path/to/bk_idk TARGET=bk7239n release \
  BASE_URL=https://downloads.example.com/arduino-beken/bk7239n
```

## Output artifacts
For a single target, the default `release` output directory is `dist/release/<target>/`, and it contains:

1. `arduino-beken-<target>-<version>.tar.gz`
2. `arduino-beken-sdk-<target>-<version>.tar.gz`
3. `package_beken_<target>_index.json`

Artifact meanings:

| File | Purpose |
| --- | --- |
| `arduino-beken-<target>-<version>.tar.gz` | The Arduino platform package containing cores, variants, libraries, uploader executables (for all platforms), `platform.txt`, and `boards.txt` |
| `arduino-beken-sdk-<target>-<version>.tar.gz` | The Arduino-side SDK with headers, static libraries, flags, linker scripts, packaging materials, and compile-time partition-generation runtime |
| `package_beken_<target>_index.json` | The Boards Manager package index, which does not include separate uploader tool entries, as the uploader is distributed as part of the platform. The toolchain is downloaded by Arduino IDE/CLI via URLs in the JSON file |

Additional notes:

1. The default `RELEASE_DIR` is `dist/release/<target>`
2. For `bk7239n`, the SDK archive must include `artifacts/package/sys_persist_config_4k.bin` so Arduino CLI can rebuild a bootable `all-app.bin`
3. The staged platform archive must include `config/<target>/auto_partitions*.csv` so Arduino CLI / Arduino IDE can regenerate partition outputs at compile time
4. `tools/toolchains.json` already pre-defines entries for all platforms (including Linux, macOS, and Windows); if custom platform-specific toolchain configuration is needed, modify the corresponding entries in `tools/toolchains.json`
5. The uploader is not released as a separate tool, but as part of the platform. All platform uploader executables are embedded in the platform archive.
6. If you want each target to land in its own output directory, override it explicitly:

```bash
make SDK_DIR=/path/to/bk_idk TARGET=bk7239n \
  RELEASE_DIR="$(pwd)/dist/release/bk7239n" \
  release BASE_URL=https://downloads.example.com/arduino-beken/bk7239n
```

Example manifest:

```json
{
  "arm-none-eabi-gcc": {
    "version": "13.3.1",
    "systems": {
      "linux64": {
        "url": "https://downloads.example.com/tools/arm-none-eabi-gcc-13.3.1-linux64.tar.gz",
        "checksum": "SHA-256:<sha256>",
        "size": "<bytes>"
      },
      "win64": {
        "url": "https://downloads.example.com/tools/arm-none-eabi-gcc-13.3.1-win64.zip",
        "checksum": "SHA-256:<sha256>",
        "size": "<bytes>"
      }
    }
  }
}
```

With that manifest in place, the normal release command stays short:

```bash
make SDK_DIR=/path/to/bk_idk TARGET=bk7239n release \
  BASE_URL=https://downloads.example.com/arduino-beken/bk7239n
```

Optional command-line overrides still exist for one-off testing, but they are no longer the primary workflow.

## Recommended hosting layout
It is recommended to use one URL prefix per target so that package-index URLs remain stable and easy to reason about.

Suggested layout:

```text
https://downloads.example.com/arduino-beken/bk7239n/
  arduino-beken-bk7239n-<version>.tar.gz
  arduino-beken-sdk-bk7239n-<version>.tar.gz
  package_beken_bk7239n_index.json
```

## Key release checks
Before uploading artifacts, check at least the following:

1. `platforms[0].architecture` in `package_beken_<target>_index.json` matches the target name
2. `platforms[0].boards` comes from the staged `boards.txt` of that target
3. The SDK tool dependency name is `arduino-beken-sdk-<target>`
4. `platform.txt` has already been rewritten with the correct target display name and SDK tool path
5. `manifest.json` in the exported SDK reports the same SoC as `TARGET`
6. `build/platform/<target>/arduino-beken/platform.txt` configurations are correct
7. If Linux, macOS, or Windows GCC metadata is sourced from `tools/toolchains.json`, the generated package index reflects that manifest exactly, which guides Arduino IDE/CLI to download the corresponding platform toolchains
8. For `bk7239n`, `dist/sdk/bk7239n/artifacts/package/sys_persist_config_4k.bin` exists and the regenerated Arduino CLI `all-app.bin` boots on hardware

## Staged platform and release consistency
There is an important constraint in the current implementation:

1. Both `release` and `release-check` consume `build/platform/<target>/arduino-beken/` directly
2. Local Arduino CLI validation points to `build/platform-cli/<target>/arduino-beken/`
3. The CLI-only staged platform is cloned from `build/platform/<target>/arduino-beken/` with appropriate adjustments for local validation

This means:

1. Release packaging always consumes the release-facing staged platform under `build/platform/<target>/arduino-beken/`
2. Local CLI validation still sees the same boards, tools, and metadata
3. Release archives and package metadata stay independent from any `bk_idk` uploader tree
4. Scripts and board metadata only need to be maintained once in the source tree

## Recommended formal release checklist
Before publishing a public release, the recommended order is:

1. Update `VERSION`
2. Run `release-check` for all supported targets
3. Run `arduino-cli compile` for all supported targets
4. If hardware is available, run at least one real `arduino-cli upload` and boot validation cycle
5. Run the full `release` step for all supported targets
6. Upload all archives together with their package indexes
7. Validate installation and compilation once more using a clean Arduino CLI data directory

## Post-release debugging suggestions
If a target-specific release fails, start by checking:

1. `build/platform/<target>/arduino-beken/boards.txt`
2. `build/platform/<target>/arduino-beken/platform.txt`
3. `dist/sdk/<target>/manifest.json`
4. The target archives under `dist/release/<target>/` or a custom `RELEASE_DIR`
5. `tools/gen_boards_txt.py`
6. `tools/export_sdk.py`
7. `tools/make_release.py`