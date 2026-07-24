**Current Version: 1.0.1**

Language: [English](README.md) | [简体中文](README_CN.md)

Arduino Core for Beken Wi-Fi Chipsets.

## 📋 Contents

- [Hardware](#-hardware)
- [What's New in 1.0.1](#-whats-new-in-101)
- [Quick Start](#-quick-start)
- [Documentation](#-documentation)


## 🛠️ Supported Hardware

### Current Supported

- **BK7239N**: Dual-band 2.4/5 GHz Wi-Fi 6 (802.11a/b/g/n/ac/ax), Bluetooth Low Energy (BLE) 6.0, and IEEE 802.15.4 wireless MCU

### Development Kits

- M39N Development Board (BK7239N-based)

## ✨ What's New in 1.0.1

Expanded wireless/networking libraries and examples:

- **WiFi**: more STA/AP/UDP/server examples (13 sketches)
- **HTTPClient**: HTTP/HTTPS client examples (6 sketches)
- **PubSubClient**: MQTT examples (7 sketches)
- **Websocket**: WebSocket example (1 sketch)
- **DNSServer**: Captive portal example

## 💡 Quick Start

Get started with the Beken BK7239N Arduino Core by installing the package and uploading your first sketch. Detailed instructions are available in the documentation.

## 📖 Documentation

Complete documentation is available in our [online documentation](https://docs.bekencorp.com/arminodoc/bk_arduino/bk7239n/en/index.html):

- [Platform Overview](https://docs.bekencorp.com/arminodoc/bk_arduino/bk7239n/en/chapters/platform.html)
- [Setup Instructions](https://docs.bekencorp.com/arminodoc/bk_arduino/bk7239n/en/chapters/setup.html)
- [Peripheral Guides](https://docs.bekencorp.com/arminodoc/bk_arduino/bk7239n/en/chapters/peripherals.html)
- [Wireless Connectivity](https://docs.bekencorp.com/arminodoc/bk_arduino/bk7239n/en/chapters/wireless.html)
- [Debugging Guide](https://docs.bekencorp.com/arminodoc/bk_arduino/bk7239n/en/chapters/debug.html)
- [FAQ](https://docs.bekencorp.com/arminodoc/bk_arduino/bk7239n/en/faq.html)



## 🤝 Contributing

We welcome contributions! Please see our [contributing guide](https://docs.bekencorp.com/arminodoc/bk_arduino/bk7239n/en/contributing.html) for details.

## 🔧 Validation Methods

When modifying the arduino-beken code and validating changes, there are two approaches:

1. **Direct Modification**: Locate the local bk7239n directory in your Arduino installation, directly modify code under `components/arduino-beken-core`, then validate directly in the IDE.

2. **Repository-based Workflow**: Modify code in the arduino-beken repository, follow the [release documentation](docs/release.md) to generate a URL, and import in Boards Manager. This approach requires deploying a web service locally to store the release artifacts.
