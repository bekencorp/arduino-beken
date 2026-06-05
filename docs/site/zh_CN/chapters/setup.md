# 4. 环境搭建

本章介绍如何搭建 Beken Arduino 的开发环境，包括 Arduino IDE 安装、平台准备和验证。

## 支持的操作系统

Beken Arduino 开发环境支持以下操作系统：

| 操作系统 | 版本 | 状态 |
|-----------------|---------|--------|
| Windows | 10/11 | ✅ 支持 |
| Linux | Ubuntu 20.04+ LTS, Debian 11+ | ✅ 支持 |
| macOS | 11.5+ | ✅ 支持 |


## 1. 安装 Python 环境

<a href="../_static/images/python_icon.png" target="_blank"><img src="../_static/images/python_icon.png" alt="Python Logo" style="max-width: 200px; height: auto;"></a>

编译 Beken Arduino 代码需要 Python 环境。

### Windows

1. 从 [官方网站](https://www.python.org/downloads/windows/) 下载 Python
2. 运行安装程序，确保勾选 **"Add Python to PATH"**
3. 打开命令提示符验证安装：
   ```shell
   python --version
   ```



### Linux

Python 3 通常已预装。验证版本：
```bash
python3 --version
```

Debian/Ubuntu 系统：
```bash
sudo apt update
sudo apt install python3 python3-pip
```

### macOS

我们建议通过 Homebrew 安装 Python，因为后续我们需要使用 Homebrew 安装 zstd：

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
brew install python3
```

验证安装：
```bash
python3 --version
```

## 2. 安装 Arduino IDE

<a href="../_static/images/arduino_icon.png" target="_blank"><img src="../_static/images/arduino_icon.png" alt="Arduino IDE Logo" style="max-width: 150px; height: auto;"></a>

### Windows

1. 从 [官方网站](https://www.arduino.cc/en/software) 下载 Arduino IDE

<a href="../_static/images/arduino_windows.png" target="_blank"><img src="../_static/images/arduino_windows.png" alt="Arduino IDE Windows Download" style="max-width: 100%; height: auto;"></a>

2. 运行安装程序并按照向导操作
3. 安装完成后启动 Arduino IDE

<a href="../_static/images/en_arduino_ide_windows.png" target="_blank"><img src="../_static/images/en_arduino_ide_windows.png" alt="Arduino IDE Windows Installer" style="max-width: 100%; height: auto;"></a>



### Linux

1. 从 [官方网站](https://www.arduino.cc/en/software) 下载 AppImage

<a href="../_static/images/arduino_ide_linux.png" target="_blank"><img src="../_static/images/arduino_ide_linux.png" alt="Arduino IDE Linux AppImage" style="max-width: 100%; height: auto;"></a>

2. 使 AppImage 可执行：
```bash
chmod +x arduino-ide_*.AppImage
```
3. 直接运行 AppImage 或集成到系统中
   - 如果 AppImage 报告缺少依赖，请按照提示安装

### macOS

**重要：需要手动安装 bk_uploader 和 zstd**

在 macOS 上使用 Arduino IDE 配合 Beken 开发板之前，您需要先手动安装 bk_uploader：

- 适用于 Intel 芯片 Mac (x86_64): [下载适用于 Intel Mac 的 bk_uploader](https://dl.bekencorp.com/tools/bkfil/v4/cli/macos/bk_loader_macos_x86_64_4.1.2.260525141.pkg)
- 适用于 Apple Silicon 芯片 Mac (arm64): [下载适用于 Apple Silicon 的 bk_uploader](https://dl.bekencorp.com/tools/bkfil/v4/cli/macos/bk_loader_macos_arm64_4.1.2.260525141.pkg)

<a href="../_static/images/install_guide.png" target="_blank"><img src="../_static/images/install_guide.png" alt="安装指南" style="max-width: 100%; height: auto;"></a>

在安装过程中，请选择默认配置选项。

安装完 bk_uploader 后，工具链需要通过 Homebrew 安装 zstd：

```bash
brew install zstd
```

然后：

1. 从 [官方网站](https://www.arduino.cc/en/software) 下载 Arduino IDE
2. 解压下载的文件并将 Arduino IDE 移动到应用程序文件夹
3. 启动 Arduino IDE

## 3. 安装 Beken 开发板支持包 (BSP)

### 使用 Boards Manager（推荐）

1. **打开首选项**：
   - 点击 **File > Preferences**
   
     <a href="../_static/images/file-preference.png" target="_blank"><img src="../_static/images/file-preference.png" alt="打开首选项" style="max-width: 100%; height: auto;"></a>

2. **添加 Beken 开发板 URL**：
   
   <a href="../_static/images/url_github.png" target="_blank"><img src="../_static/images/url_github.png" alt="Add Beken Board URL" style="max-width: 100%; height: auto;"></a>
   - 在 **Preferences** 窗口中，找到 **Additional Boards Manager URLs** 字段
   - 点击 **+** 按钮添加新 URL
   - 粘贴以下 URL：
     ```
     https://github.com/bekencorp/arduino-beken/releases/download/V1.0.0/package_beken_bk7239n_index.json
     ```
   - 点击 **OK** 保存

3. **安装 Beken 开发板包**：
   - 点击 **Tools > Board > Boards Manager**
   - 在搜索栏中搜索 "Beken"
   - 点击 **Install** 安装 "BK7239N" 包
   
     <a href="../_static/images/board_import.png" target="_blank"><img src="../_static/images/board_import.png" alt="Install Beken Board Package" style="max-width: 100%; height: auto;"></a>

4. **选择开发板**：
   - 安装完成后，点击 **Tools > Board > Beken**
   - 从列表中选择 BK7239N

## 4. 验证安装

1. 通过 USB 将 Beken 开发板连接到电脑
2. 创建新的代码文件，输入以下代码：
   ```cpp
   // setup 函数在按下复位按钮或给开发板供电时运行一次
   void setup() {
     // 将数字引脚 LED_BUILTIN 初始化为输出
     pinMode(LED_BUILTIN, OUTPUT);
   }

   // loop 函数会一遍又一遍地永远运行
   void loop() {
     digitalWrite(LED_BUILTIN, HIGH);   // 点亮 LED（HIGH 是电压级别）
     delay(1000);                       // 等待一秒
     digitalWrite(LED_BUILTIN, LOW);    // 通过将电压设为低电平关闭 LED
     delay(1000);                       // 等待一秒
   }
   ```
3. 从 **Tools > Board > Beken** 选择开发板
4. 从 **Tools > Port** 选择正确的端口
5. 点击上传按钮 (▶️)
6. 您应该能看到开发板上的板载 LED 在闪烁

## 示例程序

示例程序集是了解当前 Arduino 层在硬件上实际功能的最快方式。

### 完整示例清单

- `ADCReadSerial`
- `BLEAdvertise`
- `BLEWriteTest`
- `Blink`
- `GPIOInterrupt`
- `HardwareTimer`
- `MultiInoBlink`
- `MultiSourceBlink`
- `PWMFade`
- `SPILoopback`
- `SerialEcho`
- `TickerBlink`
- `WiFiConnect`
- `WiFiScan`
- `WireMaster`
- `WireScanner`
- `WireSlave`

### 将示例作为验证层阅读

示例不仅是演示，也是分层验证集：

- 启动示例：`Blink`, `SerialEcho`
- 连接示例：`WiFiScan`, `WiFiConnect`, `BLEAdvertise`
- 总线示例：`WireScanner`, `SPILoopback`
- 外设示例：`ADCReadSerial`, `PWMFade`, `HardwareTimer`, `DACSineWave`
- 构建形态示例：`MultiInoBlink`, `MultiSourceBlink`

### 建议的验证顺序

对于新平台验证运行：

1. `Blink`
2. `SerialEcho`
3. `WiFiScan`
4. `WireScanner`

然后运行外设特定示例，如 `ADCReadSerial`、`PWMFade` 或 `HardwareTimer`。

### 示例特定注意事项

- 大多数示例以 `115200` 波特率打印状态。
- `SPILoopback` 需要将 `MOSI` 连接到 `MISO`。
- `ADCReadSerial` 依赖于变体模拟引脚映射。