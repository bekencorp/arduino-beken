# 8. 调试指南

## SWD 系统调试

> **⚠️ 警告：功能开发中**<br>
> SWD 调试功能仍在积极开发中，目前无法使用。

### 接线说明

SWD（Serial Wire Debug）是一种用于 ARM 微控制器的调试接口。以下是 BK7239N 开发板的 SWD 接线说明：

| 引脚 | 信号名称 | 说明 |
|------|----------|------|
| SWDIO | SWD Data Input/Output | 串行数据线 |
| SWCLK | SWD Clock | 串行时钟线 |
| GND | Ground | 地线 |
| 3.3V | Power | 电源（连接到调试器 VTref） |

### 典型接线方式

**M39N DevKit 开发板：**

| 开发板引脚 | 调试器引脚 |
|------------|------------|
| SWDIO (GPIO1) | SWDIO |
| SWCLK (GPIO0) | SWCLK |
| 3.3V | VTref |
| GND | GND |


> **注意：**
> - 调试器需使用 3.3V 电平
> - 确保开发板和调试器共地
> - 建议使用高质量连接线，长度不宜过长

### 在 Arduino IDE 中开始调试会话

<a href="../_static/images/ide_debug.png" target="_blank"><img src="../_static/images/ide_debug.png" alt="Arduino IDE 调试按钮" style="max-width: 600px; height: auto;"></a>

完成接线后，按照以下步骤在 Arduino IDE 中开始调试：

1. 将调试器连接到电脑
2. 在 Arduino IDE 中打开您的代码
3. 点击工具栏中的 **调试** 按钮（▶️ 带虫子图标的按钮）
4. IDE 将编译代码并连接到目标设备
5. 连接成功后，您可以设置断点、查看变量并单步执行代码

> **注意：** 开始调试会话前，请确保调试器已被系统正确识别。

## 串口调试

### 串口调试输出

```cpp
Serial.begin(115200);
Serial.print("Value: ");
Serial.println(sensorValue);
Serial.printf("Temperature: %.2f C\n", temperature);
```



## 常见编译问题排查

| 问题 | 解决方案 |
|-------|----------|
| 缺少开发板包 | 通过 Boards Manager 安装 Beken BSP |
| 库冲突 | 删除重复的库 |
| 内存错误 | 减小代码大小 |
| 编译错误 | 检查语法和包含路径 |
| Python 未找到 (Windows) | 安装 Python 并添加到 PATH |

### Windows: Python 未找到错误

如果在 Windows 上编译时遇到 Python 相关错误：

<a href="../_static/images/python_compile_error.png" target="_blank"><img src="../_static/images/python_compile_error.png" alt="Windows Python 编译错误" style="max-width: 100%; height: auto;"></a>

**解决方案：**
1. 从 [官方网站](https://www.python.org/downloads/windows/) 下载 Python
2. 运行安装程序，确保勾选 **"Add Python to PATH"**
3. 重新启动 Arduino IDE 并重试编译

### macOS: zstd 库未找到错误

如果在 macOS 上编译时遇到 zstd 库错误：

```
dyld: Library not loaded: /usr/local/opt/zstd/lib/libzstd.1.dylib 
   Referenced from: /Users/beken/Library/Arduino15/packages/beken/tools/arm-none-eabi-gcc/13.3.1/bin/../libexec/gcc/arm-none-eabi/13.3.1/cc1plus 
   Reason: image not found 
```

**解决方案：**
按照**环境搭建**章节中的说明，使用 Homebrew 安装 zstd：

```bash
brew install zstd
```

## 下载问题排查

| 问题 | 解决方案 |
|-------|----------|
| 端口未找到 | 安装驱动，检查 USB 连接 |
| 上传失败 | 上传时重置设备，检查启动模式 |
| 连接超时 | 验证波特率和串口设置 |

## 硬件适配故障排查

| 症状 | 可能原因 | 修复方法 |
|---------|---------------|-----|
| 无电源 | 电压错误，线缆损坏 | 检查电源供应 |
| 无串口输出 | 波特率错误，端口错误 | 验证设置 |
| 外设不工作 | 引脚分配错误 | 检查引脚定义 |
| Wi-Fi 无法连接 | 凭证错误，信号问题 | 验证 SSID/密码 |