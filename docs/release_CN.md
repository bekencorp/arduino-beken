# arduino-beken 发布文档

语言： [English](release.md) | 简体中文

## 目的
本文档说明如何为 `arduino-beken` 生成 Boards Manager 发布物，以及如何在正式发布前完成轻量校验。

## 发布前准备
开始前请确认：

1. `bk_idk` checkout 可本地构建目标 target。
2. `arduino-beken/VERSION` 已更新为目标版本号。
3. 平台归档已包含所有平台的 uploader 可执行文件，无需额外准备。
4. 已检查 `tools/toolchains.json`，确认其内容符合本次发布策略。
5. 如果 manifest 对 Linux 采用本地打包模式，则其中配置的 Linux ARM GCC `root` 必须可用。
6. 如果要支持 Windows Boards Manager 编译且 manifest 对 Windows 也采用本地打包模式，则其中配置的 Windows ARM GCC `root` 也必须可用。
7. 用于托管发布物的 HTTP/HTTPS 地址已确定。

## 发布模型
当前发布模型按 target 拆分：

1. 每个 target 生成一份独立的 platform archive（包含平台文件、libraries、uploader可执行文件等）。
2. 每个 target 生成一份独立的 SDK archive。
3. 每个 target 生成一份独立的 package index：
   - `package_beken_bk7239n_index.json`

其中 platform archive 包含了所有平台的 uploader 可执行文件。uploader 不作为独立工具发布，而是作为 platform 的一部分。

这意味着当前更适合“每个 target 一个 package index URL”的发布方式。  
如果未来需要“一个 index 同时承载多个 target”，需要扩展 `tools/make_release.py`。

## Toolchain 元数据模型
当前 toolchain 元数据已改为仓库内自管，并通过本仓库维护的模板化 manifest 驱动。

默认 manifest 位于：

```text
tools/toolchains.json
```

其中按宿主机维护 `arm-none-eabi-gcc` 的 Boards Manager 元数据，这些元数据包含在 package index 中。每个宿主机条目支持两种模式：

1. `root`：在 `make release` 时从本地目录获取 toolchain 信息
2. `url` + `checksum` + `size`：直接引用已发布的外部 archive，并写入 package index 供 Arduino IDE/CLI 下载

因此，正常发布流程默认只需要维护 manifest，不需要在命令行里手工拼很长的 toolchain 参数。

## 推荐发布流程
### 1. 先做轻量校验
对每个 target 先执行：

```bash
make SDK_DIR=/path/to/bk_idk TARGET=<target> release-check
```

这一步会：

1. 重新生成 staged platform。
2. 导出 `dist/sdk/<target>`。
3. 使用轻量 archive 生成 `dist/release-check/<target>/` 下的完整发布结构。
4. 快速验证 package index、文件命名和 metadata，而不必打大包。

建议优先对当前支持的两个 target 都跑一遍：

```bash
make SDK_DIR=/path/to/bk_idk TARGET=bk7239n release-check
```

### 2. 再执行正式打包
轻量校验通过后，对每个 target 分别执行正式发布：

```bash
make SDK_DIR=/path/to/bk_idk TARGET=<target> release \
  BASE_URL=https://example.com/arduino-beken/<target>
```

示例：

```bash
make SDK_DIR=/path/to/bk_idk TARGET=bk7239n release \
  BASE_URL=https://downloads.example.com/arduino-beken/bk7239n
```

## 产物说明
对单个 target 执行 `release` 后，默认会在 `dist/release/<target>/` 下输出以下文件：

1. `arduino-beken-<target>-<version>.tar.gz`
2. `arduino-beken-sdk-<target>-<version>.tar.gz`
3. `package_beken_<target>_index.json`

这些文件的含义如下：

| 文件 | 作用 |
| --- | --- |
| `arduino-beken-<target>-<version>.tar.gz` | Arduino 平台本体，包含 cores、variants、libraries、uploader 可执行文件（所有平台）、platform.txt、boards.txt |
| `arduino-beken-sdk-<target>-<version>.tar.gz` | 供 CLI/IDE 编译使用的静态库、头文件、flags、linker script、打包材料，以及编译期分区生成运行时 |
| `package_beken_<target>_index.json` | Boards Manager package index，其中不包含独立的 uploader 工具条目，uploader 作为 platform 的一部分分发。工具链通过 JSON 中的 URL 由 Arduino IDE/CLI 下载 |

补充说明：

1. 默认 `RELEASE_DIR` 是 `dist/release/<target>`。
2. 对 `bk7239n`，SDK archive 里必须包含 `artifacts/package/sys_persist_config_4k.bin`，Arduino CLI 才能重新拼出可启动的 `all-app.bin`。
3. staged platform archive 中还必须包含 `config/<target>/auto_partitions*.csv`，Arduino CLI / Arduino IDE 才能在编译阶段重新生成分区产物。
4. `tools/toolchains.json` 中已预定义了所有平台的工具链条目（包括 Linux、macOS、Windows），如果需要自定义特定平台的工具链配置，可以在 `tools/toolchains.json` 中修改相应的条目。
5. uploader 不作为独立工具发布，而是作为 platform 的一部分。所有平台的 uploader 可执行文件都嵌入在 platform 归档中。
6. 若想让不同 target 的正式发布过程分别落到独立目录，可在执行时覆盖：

```bash
make SDK_DIR=/path/to/bk_idk TARGET=bk7239n \
  RELEASE_DIR="$(pwd)/dist/release/bk7239n" \
  release BASE_URL=https://downloads.example.com/arduino-beken/bk7239n
```

manifest 示例：

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

配置好 manifest 后，正式发布命令可以保持简短：

```bash
make SDK_DIR=/path/to/bk_idk TARGET=bk7239n release \
  BASE_URL=https://downloads.example.com/arduino-beken/bk7239n
```

命令行 override 仍然保留，便于一次性调试，但不再作为主流程。

## URL 与托管布局建议
建议每个 target 使用独立 URL 前缀，保证 package index 中的链接稳定且清晰。

推荐布局：

```text
https://downloads.example.com/arduino-beken/bk7239n/
  arduino-beken-bk7239n-<version>.tar.gz
  arduino-beken-sdk-bk7239n-<version>.tar.gz
  package_beken_bk7239n_index.json
```

## 发布时的关键检查项
上传前建议至少检查以下内容：

1. `package_beken_<target>_index.json` 中 `platforms[0].architecture` 是否等于 target 名。
2. `platforms[0].boards` 是否来自当前 target 的 staged `boards.txt`。
3. `toolsDependencies` 中的 SDK tool 名是否为 `arduino-beken-sdk-<target>`。
4. `platform.txt` 中的 `name=` 与 SDK tool 路径是否已按 target 改写。
5. SDK manifest 中的 `target.soc` 是否等于当前 `TARGET`。
6. `build/platform/<target>/arduino-beken/platform.txt` 中的配置正确无误。
7. 如果 Linux、macOS 或 Windows GCC 元数据来自 `tools/toolchains.json`，则生成出的 package index 应准确反映该 manifest，这些元数据用于指导 Arduino IDE/CLI 下载相应平台的工具链。
8. 对 `bk7239n`，`dist/sdk/bk7239n/artifacts/package/sys_persist_config_4k.bin` 必须存在，并且重新生成的 Arduino CLI `all-app.bin` 需要能在硬件上启动。

## staged platform 与发布的一致性
当前发布实现有一个重要约束：

1. 正式 `release` 与 `release-check` 都直接消费 `build/platform/<target>/arduino-beken/`。
2. 本地 Arduino CLI 验证会指向 `build/platform-cli/<target>/arduino-beken/`。
3. CLI 专用平台由 `build/platform/<target>/arduino-beken/` 复制而来，进行适当的本地验证调整。

这意味着：

1. 正式发布始终只消费 `build/platform/<target>/arduino-beken/` 这份发布用平台。
2. 本地 CLI 看到的 boards、tools 与 metadata 与发布平台保持一致。
3. release archive 与 package metadata 不再依赖 `bk_idk` 中的 uploader 目录。
4. 脚本和板型配置依然只需要在源码树维护一份。

## 建议的正式发布清单
正式对外发布前，建议按以下顺序执行：

1. 更新 `VERSION`。
2. 对所有目标执行 `release-check`。
3. 对所有目标执行 `arduino-cli compile`。
4. 如有硬件条件，执行至少一轮 `arduino-cli upload` 与上电启动验证。
5. 对所有目标执行正式 `release`。
6. 上传所有归档与对应的 package index。
7. 用一个干净的 Arduino CLI 数据目录再次验证 package index 可安装、可编译。

## 发布后回溯建议
如果某个发布目标出现问题，优先从以下几处排查：

1. `build/platform/<target>/arduino-beken/boards.txt`
2. `build/platform/<target>/arduino-beken/platform.txt`
3. `dist/sdk/<target>/manifest.json`
4. `dist/release/<target>/` 中的目标归档，或自定义 `RELEASE_DIR` 下的归档
5. `tools/gen_boards_txt.py`
6. `tools/export_sdk.py`
7. `tools/make_release.py`