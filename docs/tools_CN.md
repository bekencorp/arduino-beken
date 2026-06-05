# arduino-beken 脚本说明

语言： [English](tools.md) | 简体中文

本文档逐一介绍 `tools/` 下的脚本，重点说明：

1. 它的职责是什么
2. 它通常由谁调用

本文不覆盖 `tools/bk_loader/`（`linux/`、`windows/`、可选的 `macos/arm64/`、`macos/x86_64/`）下的二进制 uploader 和 `tools/toolchains.json` 这类数据文件。

## 如何阅读

可把这些脚本分成四类：

1. 平台 staging 与 core wrapper 生成
2. `bk_idk` 直构辅助
3. Arduino CLI / IDE 构建上传链路
4. SDK 导出与发布打包

其中真正的主链入口通常不是脚本本身，而是 `Makefile` 或 `platform.txt`。

## 术语说明

### staging

在这个仓库里，`staging` 指的是把仓库中的源码内容复制并按 Arduino 平台要求改写到 `build/` 下的生成目录里，使 Arduino CLI / IDE 能把它当成一套自包含的 hardware package 直接消费。

典型的 staged 输出包括：

- `build/platform/<target>/arduino-beken/`
- `build/platform-cli/<target>/arduino-beken/`

### core wrapper（核心 wrapper）

Arduino CLI 与平台布局要求在 `cores/arduino/` 下出现约定俗成的核心文件名（如 `Arduino.cpp`、`HardwareSerial.h`）。本移植的真实实现位于 `components/arduino-beken-core/`、`components/arduino-beken-main/`，按 `bk_idk` / CMake 组织。

**core wrapper** 指自动生成的薄文件（通常只有一行 `#include` 指向 `components/...`），用于：

- 满足 Arduino 的目录与文件名约定，而不把逻辑复制第二份
- 以 `components/` 为唯一事实来源
- 在每次 `prepare-platform` 时随 staged 平台一起重生成

### sketch 链路里的其它 wrapper

与 core wrapper 不同，以下脚本为 sketch 侧生成小型胶水库：

- `bk_prepare_core.py` 生成的 sketch wrapper
- `sketch_prep.py` 为 sketch 同级其它源文件生成的额外编译单元

## 一、平台 staging 与 core wrapper 生成

### `tools/stage_platform.py`

作用：

- `prepare-platform` 的直接入口
- 生成 `build/platform/<target>/arduino-beken/`
- 生成 `build/platform-cli/<target>/arduino-beken/`
- 更新本地 `.arduino-cli-test/user/hardware/beken/<target>` symlink

调用方：

- [Makefile](../Makefile)

### `tools/platform_tree.py`

作用：

- `stage_platform.py` 的底层实现
- 复制 `components/`、`variants/`、`libraries/`、`examples/`、`tools/`
- 生成 staged `boards.txt`
- 改写 staged `platform.txt`
- 生成 staged `cores/arduino/` core wrapper

调用方：

- [tools/stage_platform.py](../tools/stage_platform.py)
- [tools/make_release.py](../tools/make_release.py)

### `tools/gen_boards_txt.py`

作用：

- 按 `TARGET` 扫描 `config/<target>` 与 `variants/`
- 计算 flash size、app partition size、默认 upload speed，以及分区方案相关板级属性
- 生成 staged `boards.txt`
- 当存在多份 `auto_partitions*.csv` 时，自动生成 `menu.PartitionScheme`

调用方：

- [tools/platform_tree.py](../tools/platform_tree.py)

### `tools/ide/bk_generate_partitions.py`

作用：

- 在 `platform.txt` 的 prebuild hook 中、普通编译开始前执行
- 根据 `build.partitions_csv` 选择当前生效的分区 CSV
- 为当前 Arduino build 创建一份 synthetic project root
- 调用导出 SDK 中内置的上游分区生成器
- 生成 build-local 的 `partitions_gen.h`、`partitions.h`、`bk_package.json` 和 `bk_ota_partitions.json`

调用方：

- [platform.txt](../platform.txt)

### `tools/core_wrappers.py`

作用：

- 定义 staged `cores/arduino/` 的 core wrapper 映射（`CORE_SOURCE_MAP`、`CORE_HEADER_MAP`）
- 让每个 wrapper 指向 `components/arduino-beken-core/` 或 `components/arduino-beken-main/`
- 定义 staged 平台中要省略的 wrapper（`STAGED_CORE_PRUNED_FILES`）

调用方：

- [tools/platform_tree.py](../tools/platform_tree.py)
- [tools/generate_core_wrappers.py](../tools/generate_core_wrappers.py)
- [tools/check_core_wrappers.py](../tools/check_core_wrappers.py)

### `tools/generate_core_wrappers.py`

作用：

- 根据 `tools/core_wrappers.py` 把薄 `#include` core wrapper 写入指定的 `cores/arduino/` 目录
- [Makefile](../Makefile) 目标 `sync-core-wrappers` 写入的是**仓库根目录**下的 `cores/arduino/`（完整清单，包含 staged 流程会省略的 `GeneratedSketch.cpp`、`main.c`、`init_stub.c` 等）
- 在修改 `CORE_SOURCE_MAP` / `CORE_HEADER_MAP` 后可用于同步本地/IDE 仍打开的 repo 根 `cores/arduino/`；正式发布与 `arduino-cli` 实际消费的是 `prepare-platform` 产出的 **staged** 树

调用方：

- [Makefile](../Makefile) 的 `sync-core-wrappers`

### `tools/check_core_wrappers.py`

作用：

- 校验 staged `cores/arduino/` 是否完整
- 校验每个 wrapper 是否与 `core_wrappers.py` 清单一致
- 在临时目录重新生成并比对文件集合是否与预期一致
- 可作为 CI 或回归检查项

调用方：

- [Makefile](../Makefile) 的 `check-core-wrappers`（该目标依赖 `prepare-platform`，因此每次都会先刷新 staged 平台再校验）

## 二、`bk_idk` 直构辅助

### `tools/sketch_prep.py`

作用：

- 服务 `make build`
- 把 Arduino sketch 目录整理成 `bk_idk` 可直接消费的编译单元
- 合并主 `.ino` 与同目录其它 `.ino`
- 为同目录其它 `.c` / `.cpp` 生成额外 wrapper

调用方：

- [Makefile](../Makefile)

### `tools/patch_linker_script.py`

作用：

- 给 `bk_idk` 生成的 linker script 打补丁
- 补回 `.preinit_array` / `.init_array` / `.fini_array`
- 保证 C++ 全局构造和析构表可用

调用方：

- [components/arduino-beken-linker/CMakeLists.txt](../components/arduino-beken-linker/CMakeLists.txt)

## 三、Arduino CLI / IDE 构建上传链路

### `tools/ide/bk_prepare_core.py`

作用：

- 由 `platform.txt` 的 prebuild hook 调用
- 在当前 build 的 `core/` 目录里生成 `GeneratedSketch_<hash>.cpp`
- 让 `setup()/loop()` 仍然通过 `core.a` 进入最终链接
- 同时在 sketch 路径变化时强制 core archive 失效重建，避免 IDE 复用旧 sketch

调用方：

- [platform.txt](../platform.txt)

### `tools/ide/bk_compile.py`

作用：

- Arduino 平台 compile recipe 的轻包装
- 读取 SDK 导出的 flags 文件
- 拼出真实编译命令并执行

调用方：

- [platform.txt](../platform.txt)

### `tools/ide/bk_builder.py`

作用：

- 承接 Arduino 平台的 link / objcopy / 最终镜像打包
- 负责链接 ELF
- 负责生成 `.bin`
- 负责生成 sketch-specific 的 `*.all-app.bin`
- 同时保留兼容副本 `all-app.bin`
- 优先消费当前 build 目录下的分区 JSON，而不是 SDK 中固化的静态副本
- 负责过滤独立的 `sketch/*.ino.cpp.o`，保持当前验证过的链接语义

调用方：

- [platform.txt](../platform.txt)

### `tools/ide/bk_uploader.py`

作用：

- Arduino 平台 upload recipe 的包装器
- 把串口、波特率、输入镜像参数转成 `bk_loader download`
- 统一传递 `--reboot`

调用方：

- [platform.txt](../platform.txt)

## 四、SDK 导出与发布打包

### `tools/export_sdk.py`

作用：

- 从一次 `bk_idk` baseline build 中抽取 Arduino 可复用 SDK
- 导出 include、flags、libs、linker script、bootloader、partitions、package 物料
- 同时导出 Arduino CLI / IDE 编译阶段所需的最小分区生成运行时
- 生成 `dist/sdk/<target>/manifest.json`

调用方：

- [Makefile](../Makefile)

### `tools/make_release.py`

作用：

- 基于 staged platform 和 exported SDK 生成发布物
- 生成 platform archive、SDK archive；工具链引用通过 package index 中的 URL 处理
- 生成 `package_beken_<target>_index.json`
- 也负责 `release-check` 的 lightweight archive 流程

调用方：

- [Makefile](../Makefile)