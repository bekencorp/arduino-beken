# MQTT 基础示例

## 概述

最小 MQTT 客户端：连接 Broker，向 `outTopic` 发布 `hello world`，订阅 `inTopic` 并打印载荷；断线时用阻塞式 `reconnect()` 恢复。

## 功能特性

- WiFi STA，等待 `WSS_GOT_IP`
- 阻塞式重连
- 发布/订阅与回调打印

## 硬件要求

- Beken BK7239N 开发板
- 可访问的 MQTT Broker（1883）

## 配置说明

```cpp
WIFI_SSID / WIFI_PASS
const char* mqtt_server = "broker.mqtt-dashboard.com";
```

## 工作原理

1. 连接 WiFi
2. 设置服务器与回调
3. 断线时在 `reconnect()` 中阻塞重试
4. 持续调用 `client.loop()`

## 使用方法

1. 修改 WiFi 与 Broker
2. 上传，串口 115200 监视

## 预期输出

```
WiFi connected
Attempting MQTT connection...connected
Message arrived [inTopic] ...
```

## 故障排除

- 一直重连：检查 Broker/DNS/防火墙
- 无回调：用其他客户端向 `inTopic` 发消息

## 相关示例

- mqtt_reconnect_nonblocking
- mqtt_auth
- mqtt_bk7239
