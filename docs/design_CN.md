# arduino-beken 设计文档

语言： [English](design.md) | 简体中文

## 目标
`arduino-beken` 的定位是一个面向 `bk_idk` 的 Arduino 外部项目目录，目标如下：

1. 通过 `make` 调用 `bk_idk` 构建 Arduino sketch，并生成可烧录的 `all-app.bin`。
2. 维护一套可在没有本地 `bk_idk` checkout 的情况下直接消费的 Arduino 平台 SDK；只有在核心或平台输入变更时，才通过 `bk_idk` 刷新这份产物。
3. 生成可用于 Boards Manager 的发布物，并尽量把 target 差异收敛到自动生成阶段。
4. 尽量不修改 `bk_idk`，将适配逻辑留在 `arduino-beken` 内部。

## 总体结构
仓库内的主要目录职责如下：

| 路径 | 作用 |
| --- | --- |
| `components/arduino-beken-core` | Arduino Core 代码，参与 `bk_idk` 本地构建 |
| `components/arduino-beken-main` | Arduino 工程入口与初始化桩代码 |
| `components/arduino-beken-linker` | 链接器组件，用于修补链接脚本以支持 C++ 构造函数表 |
| `variants/<target>_*` | Arduino 板型变体定义，至少需要一个 `<target>` |
| `config/<target>` | 目标 SoC 的项目配置与分区配置 |
| `tools/` | sketch 预处理、平台 staging、SDK 导出、发布打包等脚本 |
| `build/platform/<target>/arduino-beken` | 面向发布的 target 专用 staged platform，供 SDK 导出与发布打包消费 |
| `build/platform-cli/<target>/arduino-beken` | 仅供本地 Arduino CLI 使用的 staged platform；默认把 `tools.bk-uploader.path` 改写到 staged 的 Linux uploader（在 macOS 上测 upload 可用 `stage_platform.py --cli-uploader-root` 指向 `tools/bk_loader/macos/...`） |
| `dist/sdk/<target>` | 导出的 Arduino 侧 SDK |
| `dist/release*` | 发布物与轻量校验产物 |

## 核心流程
### 1. `make build`
通过 `make` 构建 sketch 时，主要流程如下：

1. `tools/sketch_prep.py` 会先把主 `.ino` 和同目录其它 `.ino` 合并成 `build/generated/sketch.cpp`，再为同目录其它 `.cpp` / `.c` 生成额外的 wrapper 编译单元。
2. `make` 调用 `bk_idk`，并把 `PROJECT_DIR` 指向当前 `arduino-beken`。
3. `bk_idk` 以外部组件方式编译 `arduino-beken-core` / `arduino-beken-main`。
4. 打包阶段输出 `app.bin` 与 `all-app.bin`。
5. `user_app_main()` 会在 `bk_init()` 完成 BK 运行时初始化后调用真实的 `__libc_init_array()`，让 sketch / library 的全局构造发生在 `setup()` 前，但不会过早在 `main()` 阶段执行。

这条路径主要验证“直接经由 `bk_idk` 构建 Arduino sketch”。

### 2. `make export-sdk`
`export-sdk` 负责维护 `dist/sdk/<target>` 下的 Arduino CLI/IDE SDK 导出树。

1. 在没有传入 `SDK_DIR` 时，它只校验并复用这份维护中的导出树，因此常见的 CLI 和 release 流程不需要 `bk_idk` checkout。
2. 在传入 `SDK_DIR=/path/to/bk_idk` 时，它会根据当前上游构建结果刷新这份维护中的导出树。

导出树包含：

1. `armino_as_lib` 及其位于 `armino_as_lib/include` 下的公开头文件面。
2. `sdkconfig.h`、`partitions_gen.h`、`partitions.h` 等生成头文件。
3. 链接脚本、编译 flags、链接 flags。
4. 打包所需 bootloader、分区描述与 package 相关产物。
5. Arduino 库目录 `arduino_lib`，包含如 `libBLE.a`、`libWiFi.a` 等库文件。
6. 某些 SoC 额外需要的打包输入，例如 `sys_persist_config_4k.bin`。
7. 一份最小化的上游分区生成运行时，使 Arduino CLI / IDE 能在编译阶段根据 `config/<target>/auto_partitions*.csv` 重新生成分区产物。

维护中的 SDK 树就是 Arduino CLI/IDE 与 release 打包默认消费的内容，因此常见路径不再依赖本地 `bk_idk` build tree。
当这份产物需要刷新时，linker script 仍由 `bk_idk` 先生成，再由 `arduino-beken` 在 build tree 中补丁修正 `.preinit_array` / `.init_array` / `.fini_array`，避免维护一份分叉的 SoC linker script。
对 `bk7239n`，导出的 SDK 还会带上 `artifacts/package/sys_persist_config_4k.bin`；Arduino CLI 侧打包 `all-app.bin` 前会先把它插到 `app.bin` 头部，以保持与 `bk_idk` 打包路径一致。
对 Arduino CLI/IDE 构建，staged platform 会在 prebuild hook 中做两类 Arduino 侧生成动作：

1. 根据当前选中的 `auto_partitions*.csv` 重新生成分区头文件和打包元数据。
2. 为当前 sketch 生成一个带路径哈希的 `GeneratedSketch_<hash>.cpp` wrapper，放入当前 build 的 `core/` 目录。

这样既能把分区选择收敛到本次构建，又能保持 `setup()/loop()` 仍然通过 `core.a` 进入最终链接，并在 sketch 路径变化时强制 core archive 失效重建。

### 3. `make prepare-platform`
`prepare-platform` 负责生成 target-aware 的 Arduino 平台目录：

1. 基于源码树复制 `components`、`variants`、`config/<target>`、`tools` 等静态内容。
2. 通过 `tools/gen_boards_txt.py` 生成 `boards.txt`。
3. 从 `components/arduino-beken-core/` 和 `components/arduino-beken-main/` 自动生成 staged `cores/arduino/` core wrapper。
4. 按 target 改写 `platform.txt` 中的平台名、版本号和 SDK tool 名。
5. 先输出发布用平台到 `build/platform/<target>/arduino-beken/`。
6. 再复制一份本地 CLI 专用平台到 `build/platform-cli/<target>/arduino-beken/`。
7. 只在 CLI 专用副本里改写 `tools.bk-uploader.path`，默认让 `arduino-cli upload` 使用该副本内的 `tools/bk_loader/linux`（也可用 `--cli-uploader-root` 指向例如 `tools/bk_loader/macos/arm64`）。
8. 平台内还会携带一个 prebuild hook，在 Arduino CLI/IDE 开始编译前根据当前 `build.partitions_csv` 重新生成分区产物。
9. 平台内还会携带一个 prebuild hook，在 Arduino CLI/IDE 开始编译前向当前 build 的 `core/` 目录写入 sketch-specific wrapper。
10. 更新 `.arduino-cli-test/user/hardware/beken/<target>` symlink，使本地 CLI 始终指向 CLI 专用平台。

按 target 生成的 `boards.txt` 只存在于这些 staged platform 副本中，不再存放在仓库根目录。

### 4. `make release` / `make release-check`
发布流程直接复用 staged platform：

1. `release` / `release-check` 依赖 `export-sdk`，但在常见路径下 `export-sdk` 只会校验维护中的 SDK 树。
2. `tools/make_release.py` 读取 `build/platform/<target>/arduino-beken/boards.txt` 与 `platform.txt`。
3. 生成 target 维度的 platform archive、SDK archive，以及通过 URL 引用工具链的 package index metadata。
4. 输出 `package_beken_<target>_index.json`。

`release-check` 与正式发布的区别仅在于：

1. 使用轻量 archive 代替完整 SDK / toolchain / uploader payload。
2. 更适合快速验证 metadata、归档结构与 package index。

## target 与 variant 模型
### target
`TARGET` 代表 SoC 名称，例如 `bk7239n`。它会影响：

1. `config/<TARGET>` 的配置选择。
2. `variants/` 下的板型扫描范围。
3. staged platform 中生成的 `boards.txt` / `platform.txt`。
4. `dist/sdk/<TARGET>` 与 `package_beken_<TARGET>_index.json` 等发布物名称。

### variant
`VARIANT` 用于 make 驱动构建时显式指定某个变体目录。若未指定，则默认优先选中：

1. `variants/<target>_generic`
2. 其他 `variants/<target>_*`

variant 解析逻辑集中在 `cmake/arduino_variant.cmake`，一般新增 target 时无需改动该文件。

## 自动生成与固定内容的边界
当前设计刻意把“自动生成”和“源码常驻内容”分开：

1. 源码常驻内容保留在仓库根目录，如 `components/`、`variants/`、`tools/`、`docs/`。
2. 发布用 target 生成内容放到 `build/platform/<target>/arduino-beken/`。
3. 本地 CLI 专用生成内容放到 `build/platform-cli/<target>/arduino-beken/`。
4. 维护中的 SDK 导出树放到 `dist/sdk/<target>/`，默认直接复用。
5. 发布流程仅消费发布用 staged platform，不再从根目录重新拼装一份平台。

这样做的好处是：

1. 避免生成的 `boards.txt` 在仓库根目录随不同 target 来回覆盖。
2. `cores/arduino/` 下的薄 core wrapper 不会再与 `components/arduino-beken-core/` 漂移，因为每次都会重生成。
3. 本地 CLI 直接复用仓库内置 uploader，不再依赖 `bk_idk` 中的工具目录（默认 Linux 树；macOS 单文件构建放在 `tools/bk_loader/macos/` 下供发布与可选 CLI 覆盖）。
4. 常见的 CLI / release 路径现在直接消费维护中的 SDK 导出树，不需要 `bk_idk`。
5. 发布流程仍然消费稳定、可复现的 target 平台树。
6. 更容易比较两个 target 的实际平台输出差异。

## 新增一个 target 的方法
以下步骤适用于新增一个新的 SoC target。

### 1. 先确认 `bk_idk` 原生支持该 target
先验证上游 `bk_idk` 能直接构建该目标：

```bash
make -C /path/to/bk_idk <target> PROJECT_DIR=/path/to/arduino-beken BUILD_DIR=/tmp/arduino-beken-build
```

如果 `bk_idk` 本身没有对应 target 规则，`arduino-beken` 无法单独补齐。

### 2. 新增 `config/<target>`
至少补齐以下文件：

1. `config/<target>/config`
2. `config/<target>/auto_partitions.csv`
3. 可选的 `config/<target>/auto_partitions_<name>.csv` 或 `config/<target>/auto_partitions.<name>.csv`
4. `config/<target>/partitions.csv`

这些文件决定：

1. `bk_idk` 构建配置。
2. `boards.txt` 中的 `upload.maximum_size`。
3. `boards.txt` 中的 `build.flash_size`。
4. 可选的 Arduino `Partition Scheme` 菜单项。
5. 打包与 OTA 分区相关产物。

### 3. 新增 `variants/<target>_generic`
至少提供一个 generic variant：

1. `variants/<target>/pins_arduino.h`
2. 如有需要，补充该 variant 的其他板级定义文件。

如果未来有多个板型，可继续添加：

1. `variants/<target>_board_a`
2. `variants/<target>_board_b`

`boards.txt` 会在 `prepare-platform` 时自动按目录扫描生成。

### 4. 评估是否需要 target-specific Core 适配
如果新 SoC 与现有 BK 驱动常量、寄存器定义或能力模型不同，需要检查：

1. `components/arduino-beken-core/src/Arduino.cpp`
2. `components/arduino-beken-core/include/*`
3. 其他依赖 SoC 宏的代码路径

优先做法是增加条件编译分支，而不是复制整套 core。

### 5. 评估是否需要 target-specific 板级元数据
当前 `tools/gen_boards_txt.py` 会自动生成：

1. `upload.maximum_size`
2. `build.flash_size`
3. `build.partitions_csv`
4. 可选的 `menu.PartitionScheme.*`
5. `board name`
6. 默认 upload speed

如果新 target 需要不同的：

1. `upload.maximum_data_size`
2. `build.f_cpu`
3. 默认 upload 速率

可在 `tools/gen_boards_txt.py` 中增加 target 维度映射，而不是手改生成结果。

### 6. 跑完整验证矩阵
建议至少执行：

```bash
make TARGET=<target> prepare-platform
make SDK_DIR=/path/to/bk_idk TARGET=<target> build-base
make SDK_DIR=/path/to/bk_idk TARGET=<target> cli-compile \
  CLI_SKETCH=examples/Blink
make SDK_DIR=/path/to/bk_idk TARGET=<target> release-check
```

### 7. 同步更新中英文文档
新增 target 后建议同时更新英文与中文文档：

1. `README.md` 与 `README_CN.md`
2. `docs/design.md` 与 `docs/design_CN.md`
3. `docs/verification.md` 与 `docs/verification_CN.md`
4. `docs/release.md` 与 `docs/release_CN.md`

## 当前建议
对于未来扩展，建议继续遵守两条约束：

1. target 差异尽量收敛到 `config/<target>`、`variants/<target>_*` 和少量 SoC 条件编译。
2. 生成内容只落在 `build/` 与 `dist/`，不要回写源码根目录。