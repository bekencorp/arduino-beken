# MQTT 非阻塞重连示例

## 概述

在不阻塞主循环的前提下维持 MQTT 会话。断线后最多每 5 秒尝试重连一次。

## 功能特性

- WiFi STA，等待 `WSS_GOT_IP`
- 非阻塞重连定时器
- 连接成功后发布公告并订阅

## 硬件要求

- Beken BK7239N 开发板
- MQTT Broker（1883）

## 配置说明

```cpp
WIFI_SSID / WIFI_PASS
const char* mqtt_server = "broker.mqtt-dashboard.com";
```

## 工作原理

1. `setup()` 中连接 WiFi
2. `loop()` 中若断线且已过 5 秒，调用 `reconnect()`
3. 已连接时调用 `client.loop()`

## 使用方法

1. 修改 WiFi 与 Broker
2. 上传后，可重启 Broker 观察重连

## 预期输出

```
mqtt_reconnect_nonblocking starting
Connecting to your-ssid
.....
WiFi connected
IP address: 192.168.x.x
Entering loop: non-blocking reconnect every 5s when disconnected
MQTT reconnect attempt... broker=broker.mqtt-dashboard.com:1883
MQTT connected
Published announcement to outTopic
Subscribed to inTopic
```

Broker 不可达时会出现 `MQTT connect failed, rc=...`，随后 `Will retry MQTT reconnect in 5 seconds`。断线后会出现 `MQTT disconnected, rc=...` 并再次尝试重连。

## 故障排除

- 永不重连：检查 Broker 可达性与 clientId 冲突
- 主循环卡住：确认没用阻塞式重连例程

## 相关示例

- mqtt_basic
- mqtt_bk7239
