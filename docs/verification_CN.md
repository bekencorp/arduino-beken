# arduino-beken 测试验证文档

语言： [English](verification.md) | 简体中文

## 目的
本文档用于统一 `arduino-beken` 的本地验证方式，覆盖以下三类路径：

1. `bk_idk` 直构路径。
2. `arduino-cli` 本地硬件目录路径。
3. Boards Manager 发布物的轻量校验路径。

## 前置条件
执行验证前，建议确认：

1. `bk_idk` 已可本地构建目标 SoC。
2. `arduino-cli` 已安装。
3. ARM GCC 工具链位于 `/opt/gcc-arm-none-eabi-10.3-2021.10/bin/`（与 SDK 预编译库一致），或在命令里按实际路径覆盖 `compiler.path`。
4. 本仓库根目录下存在 `.arduino-cli.yaml` 与 `examples/Blink/Blink.ino`。
5. 如需验证 upload，准备好可访问的串口设备与开发板。

本地 Arduino CLI 环境已经完全收敛到 `.arduino-cli-test/`，包括 `user/`、`data/`、`downloads/` 和 `cache/`。
首次运行时，Arduino CLI 可能会把 builtin index 和 builtin tools 下载到这个本地目录中。

下面示例默认：

```bash
export SDK_DIR=/path/to/bk_idk
cd /path/to/arduino-beken
```

## 验证矩阵
当前建议至少覆盖：

- `bk7239n`

如果新增 target，请把同样的矩阵补到该 target 上。

## 1. 验证 staged platform
只有在你想显式检查 staged Arduino platform 本身时，才需要单独执行：

```bash
make TARGET=<target> prepare-platform
```

在全新 checkout 后，这也是生成 staged `cores/arduino/` core wrapper 的入口命令。仓库本身不再依赖已提交的 `cores/` 目录参与 CLI 和 release 打包。

期望结果：

1. 生成 `build/platform/<target>/arduino-beken/`。
2. 生成供本地 Arduino CLI 使用的 `build/platform-cli/<target>/arduino-beken/`。
3. `build/platform/<target>/arduino-beken/boards.txt` 中带有 `# TARGET=<target>`。
4. `build/platform/<target>/arduino-beken/config/<target>/` 会随平台一起被 stage。
5. `.arduino-cli-test/user/hardware/beken/<target>` symlink 指向当前 target 的 CLI 专用平台。
6. `build/platform/<target>/arduino-beken/cores/arduino/` 完全由 `components/arduino-beken-core/` 和 `components/arduino-beken-main/` 自动生成。

如果 `config/<target>/` 下存在多份 `auto_partitions*.csv`，还应确认 staged `boards.txt` 中出现 `menu.PartitionScheme`。

注意：

1. 本地 `arduino-cli` 测试依赖该 symlink。
2. 如果你是在直接验证 staged platform，本地切换 target 后要重新执行一次 `prepare-platform`。
3. `build`、`build-base`、`export-sdk`、`release`、`release-check` 都会自动经过同一套 core wrapper 生成步骤。
4. 更高层的 `cli-compile` / `cli-upload` 也会自动串上 `prepare-platform`。
5. `build/platform-cli/<target>/arduino-beken/platform.txt` 会在本地 CLI 测试时自动把 `tools.python.cmd.linux` 覆盖为 `python3`（Boards Manager 安装 env-python 后则使用 release 平台里的 `{runtime.tools.env-python.path}` 配置）。

如果需要显式校验 core wrapper 完整性，可执行：

```bash
make TARGET=<target> check-core-wrappers
```

为何需要 core wrapper：Arduino CLI 要求在 `cores/arduino/` 下出现约定俗成的核心文件名，而实现放在 `components/`；薄 `#include` wrapper 在两边对齐且不重复源码，详见 `docs/tools_CN.md`「core wrapper」小节。

该 Makefile 目标依赖 `prepare-platform`，因此每次都会先刷新 staged 平台，再校验 `build/platform/<target>/arduino-beken/cores/arduino/`。

若你修改了 `tools/core_wrappers.py` 并希望同步**仓库根目录**下可选的 `cores/arduino/`（供本地浏览或旧工具链），可执行 `make sync-core-wrappers`；该目录与 CLI / release 实际消费的 staged 树相互独立。

期望结果：

1. 命令成功退出。
2. staged `cores/arduino/` 中生成的 wrapper 文件集完整。
3. 每个 wrapper 文件都与 `tools/core_wrappers.py` 中的清单一致。

## 2. 验证 `bk_idk` 直构
如果只验证外部组件集成是否正常，可先跑：

```bash
make SDK_DIR="$SDK_DIR" TARGET=<target> build-base
```

如果还要验证 sketch 注入与最终打包，可执行：

```bash
make SDK_DIR="$SDK_DIR" TARGET=<target> build \
  SKETCH="$(pwd)/libraries/Blink/examples/Blink"
```

期望结果：

1. 构建成功退出。
2. 生成 `build/<target>/arduino-beken/package/all-app.bin`。
3. 生成 `build/<target>/arduino-beken/<target>/app.elf` 与 `app.map`。

如果本次修改涉及 linker script 或启动时序，建议额外检查 `app.elf` 中已经出现 `.preinit_array` / `.init_array` / `.fini_array` 和真实 `__libc_init_array()`。

## 3. 验证 SDK 导出
执行：

```bash
make SDK_DIR="$SDK_DIR" TARGET=<target> export-sdk
```

如果已经有一份维护中的 `dist/sdk/<target>`，也可以直接执行：

```bash
make TARGET=<target> export-sdk
```

期望结果：

1. 生成 `dist/sdk/<target>/manifest.json`。
2. `manifest.json` 中 `target.soc` 与 `TARGET` 一致。
3. `dist/sdk/<target>/flags/` 下存在 `defines`、`includes`、`c_flags`、`cpp_flags`、`ld_flags`、`ld_libs`。
4. `dist/sdk/<target>/artifacts/package/bootloader.bin` 与 `partitions/` 目录已导出。
5. `dist/sdk/<target>/tools/partition_gen/` 已导出，供编译阶段重新生成分区产物。
6. 对 `bk7239n`，还应导出 `dist/sdk/bk7239n/artifacts/package/sys_persist_config_4k.bin`。

## 4. 验证 `arduino-cli compile`
### `bk7239n`
```bash
make SDK_DIR="$SDK_DIR" TARGET=bk7239n cli-compile \
  CLI_SKETCH=libraries/Blink/examples/Blink
```

期望结果：

1. 编译成功退出。
2. `build path` 下生成 `Blink.ino.elf`、`Blink.ino.bin`、`Blink.ino.all-app.bin`，以及兼容副本 `all-app.bin`。
3. `build path` 下还会出现由 prebuild 分区生成阶段写出的 `partitions/` 与 `bk_generated/include/`。
4. CLI 能正确打印 program size / global variables。
5. 不再出现以下旧告警：
   - `missing --end-group; added as last command line option`
   - `Couldn't determine program size`

补充说明：

1. `Low memory available` 仍可能出现，这是当前静态内存占用较高时的正常提示，不等同于构建失败。
2. `cli-compile` 已经自动串上 `prepare-platform`，并通过 `export-sdk` 复用维护中的 SDK 导出树，通常不需要手工先执行这两个目标。
3. 如果这次改动涉及 linker/export/startup 逻辑，建议顺手检查 `Blink.ino.elf` 中是否同时存在 `.init_array` 与 `__libc_init_array`。
4. 对 `bk7239n`，在平台或 SDK 更新后，需重新生成 `all-app.bin`，让 Arduino CLI 打包阶段先把 `sys_persist_config_4k.bin` 插入 `app.bin` 头部。
5. 如果需要稳定的自定义输出目录，可覆盖 `CLI_BUILD_PATH=...`。
6. 如果需要查看完整的 compile、link 与打包命令流，可追加 `CLI_COMPILE_EXTRA_ARGS=--verbose`。
7. 如果存在多套分区方案，建议至少用 `--board-options PartitionScheme=<name>` 再验证一套非默认方案。

底层参考命令：

```bash
arduino-cli compile --clean \
  --config-file .arduino-cli.yaml \
  --fqbn beken:<target>:<target> \
  --board-options PartitionScheme=<name> \
  --build-path .arduino-cli-test/build-<target>-blink \
  --build-property compiler.sdk.path="$(pwd)/dist/sdk/<target>" \
  --build-property compiler.path=/opt/gcc-arm-none-eabi-10.3-2021.10/bin/ \
  libraries/Blink/examples/Blink
```

如果你要验证默认 `auto_partitions.csv`，可以去掉 `--board-options PartitionScheme=<name>`。

## 5. 验证 `arduino-cli upload`
该步骤需要真实硬件与串口设备。

推荐命令：

### `bk7239n`
```bash
make SDK_DIR="$SDK_DIR" TARGET=bk7239n cli-upload \
  CLI_SKETCH=libraries/Blink/examples/Blink \
  CLI_PORT=/dev/ttyUSB0
```

期望结果：

1. 底层会调用 `bk_loader download`。
2. 使用 `Blink.ino.all-app.bin`，或当前 sketch 对应的 `*.all-app.bin` 作为输入镜像。
3. 默认波特率为 `2000000`。
4. 烧录结束后会传递 `--reboot`。
5. 在本地 Linux x86_64 验证环境下，只要先执行过 `prepare-platform`，CLI 专用平台会把 `tools.bk-uploader.path.linux` 覆盖为 staged 树内的 `tools/bk_loader/linux`（若存在），不再依赖 Boards Manager 安装的 `bk_uploader` 工具。
6. Boards Manager 发布路径下，`bk_uploader` 与 `env-python` 作为独立 tool 从 GitHub Release 下载；platform 包本身不再内嵌 `tools/bk_loader/`。

如果本机串口命名不是 `/dev/ttyUSB0`，请替换为实际设备名。

底层参考命令：

```bash
arduino-cli upload \
  --config-file .arduino-cli.yaml \
  --fqbn beken:<target>:<target> \
  --build-path .arduino-cli-test/build-<target>-blink \
  --port /dev/ttyUSB0 \
  libraries/Blink/examples/Blink
```

## 6. 验证 `release-check`
对每个 target 执行：

```bash
make SDK_DIR="$SDK_DIR" TARGET=<target> release-check
```

期望结果：

1. 生成 `dist/release-check/<target>/package_beken_<target>_index.json`。
2. 生成 `arduino-beken-<target>-<version>.tar.gz`。
3. 生成轻量版 `arduino-beken-sdk-<target>-<version>.tar.gz`；`arm-none-eabi-gcc`、`bk_uploader`、`env-python` 等 tool 条目引用 GitHub Release 上的外部 URL，而不是直接打包进 index 对应的 archive。
4. `dist/release-check/<target>/staging-platform/arduino-beken/` 中的 `boards.txt` 与 `platform.txt` 与 staged platform 一致；staged platform 的 `platform.txt` 版本应与 `VERSION` 文件一致（当前 `1.0.1`）。
5. package index 里应包含 `arm-none-eabi-gcc`（Linux/Windows）、`bk_uploader`（Linux/Windows）、`env-python`（Linux/Windows）条目；所有 GitHub 托管 tool 的 URL 前缀为 `https://github.com/bekencorp/arduino-beken/releases/download/V1.0.1/`。
6. platform 归档不再包含 `tools/bk_loader/`；`bk_uploader` 通过独立 tool 分发。

## 回归建议
发生以下改动时，建议至少跑一遍完整验证矩阵：

1. 修改 `components/arduino-beken-core` / `components/arduino-beken-main`
2. 修改 `tools/export_sdk.py`
3. 修改 `tools/make_release.py`
4. 修改 `tools/gen_boards_txt.py`
5. 新增或修改 `config/<target>`、`variants/<target>_*`
6. 修改 `platform.txt`

## 当前已验证路径
当前仓库已完成的基础验证包括：

1. `bk7239n` 的 `prepare-platform`、`arduino-cli compile`、`arduino-cli upload`、`release-check`。
2. `bk7239n` 生成 ELF 中 `.preinit_array` / `.init_array` / `.fini_array` 与 `__libc_init_array` 的检查。
3. `bk7239n` 的 Arduino CLI `all-app.bin` 已验证会使用导出的 `sys_persist_config_4k.bin` 参与打包。

如需做正式发布前验收，建议再补一次真实开发板上的 `arduino-cli upload` 与上电启动验证。