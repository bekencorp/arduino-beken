**当前版本: 1.0.0**

语言：[English](README.md) | [简体中文](README_CN.md)

适用于 Beken 无线芯片组的 Arduino 核心库。

## 📋 目录

- [硬件](#-硬件)
- [快速开始](#-快速开始)
- [文档](#-文档)


## 🛠️ 支持的硬件

### 当前支持
- **BK7239N**: 双频 2.4/5 GHz  Wi-Fi 6 (802.11a/b/g/n/ac/ax)，蓝牙低功耗 (BLE) 6.0 和 IEEE 802.15.4 无线 MCU。


### 开发套件
- M39N 开发板 (基于 BK7239N)


## 💡 快速开始

通过安装包并上传第一个程序开始使用 Beken BK7239N Arduino 核心。详细说明可在文档中找到。

## 📖 文档

完整的文档可在我们的 [在线文档](https://docs.bekencorp.com/arminodoc/bk_arduino/bk7239n/zh_CN/index.html) 中获得：

- [平台概述](https://docs.bekencorp.com/arminodoc/bk_arduino/bk7239n/zh_CN/chapters/platform.html)
- [设置说明](https://docs.bekencorp.com/arminodoc/bk_arduino/bk7239n/zh_CN/chapters/setup.html)
- [外设指南](https://docs.bekencorp.com/arminodoc/bk_arduino/bk7239n/zh_CN/chapters/peripherals.html)
- [无线连接](https://docs.bekencorp.com/arminodoc/bk_arduino/bk7239n/zh_CN/chapters/wireless.html)
- [调试指南](https://docs.bekencorp.com/arminodoc/bk_arduino/bk7239n/zh_CN/chapters/debug.html)
- [常见问题](https://docs.bekencorp.com/arminodoc/bk_arduino/bk7239n/zh_CN/faq.html)



## 🤝 贡献

欢迎贡献！详情请参阅我们的 [贡献指南](https://docs.bekencorp.com/arminodoc/bk_arduino/bk7239n/zh_CN/contributing.html)。

## 🔧 验证方法

当用户想要改动 arduino-beken 的代码并进行验证时，有两种方式验证方法：

1. **直接修改方式**：直接找到 Arduino 安装的 bk7239n 的本地目录，在 `components/arduino-beken-core` 下直接改动代码，然后在 IDE 里直接验证。

2. **仓库工作流**：在 arduino-beken 的代码仓库下进行代码修改，参考 [release 文档](docs/release_CN.md) 的发布方式，生成 URL，在 Boards Manager 导入。这条路径需要用户本地部署一个 web 服务，存放发布物。