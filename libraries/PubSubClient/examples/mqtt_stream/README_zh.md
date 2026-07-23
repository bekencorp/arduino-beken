# MQTT Stream 示例

## 概述

演示把收到的 MQTT 载荷存入 `Stream`。适合 BK7239N 的内存版 `MemoryStream`。

## 功能特性

- WiFi STA，等待 `WSS_GOT_IP`
- 通过 `PubSubClient::setStream()` 捕获载荷
- 在回调中从 Stream 打印载荷字节

## 硬件要求

- Beken BK7239N 开发板
- MQTT Broker（1883）

## 配置说明

```cpp
WIFI_SSID / WIFI_PASS
const char* mqtt_server = "broker.mqtt-dashboard.com";
MemoryStream payloadStream(1024);  // 可按需扩大容量
```

## 工作原理

1. 分配内存 Stream 并 `setStream`
2. 订阅消息到达时，库把载荷写入 Stream
3. 回调 `seek(0)` 后打印 Stream 内容

## 使用方法

1. 修改 WiFi 与 Broker
2. 上传后向 `inTopic` 发消息，观察串口输出

## 预期输出

```
WiFi connected
MQTT connected
Message arrived [inTopic] hello
```

## 故障排除

- 无输出：确认在 subscribe 前调用了 `setStream`
- 载荷被截断：增大 `MemoryStream` 容量

## 注意事项

- 本平台无需外部 SRAM 库

## 相关示例

- mqtt_basic
- mqtt_publish_in_callback
