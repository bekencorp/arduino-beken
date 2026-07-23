# MQTT 回调内发布示例

## 概述

演示在回调中转发收到的 MQTT 载荷。必须先复制载荷，因为构造出站 PUBLISH 时会覆盖原始缓冲区。

## 功能特性

- WiFi STA，等待 `WSS_GOT_IP`
- 订阅 `inTopic`
- 回调复制载荷并发布到 `outTopic`

## 硬件要求

- Beken BK7239N 开发板
- MQTT Broker（1883）

## 配置说明

```cpp
WIFI_SSID / WIFI_PASS
const char* mqtt_server = "broker.mqtt-dashboard.com";
```

## 工作原理

1. 在构造 `PubSubClient` 前声明回调
2. 收到消息后 `malloc` + `memcpy`
3. `client.publish("outTopic", p, length)` 后 `free`

## 使用方法

1. 修改 WiFi 与 Broker
2. 上传后向 `inTopic` 发消息，观察 `outTopic`

## 预期输出

```
mqtt_publish_in_callback starting
WiFi connected
MQTT connected
Published announcement to outTopic
Subscribed to inTopic
Message arrived [inTopic] len=5 payload=hello
Republished payload to outTopic
```

用其他 MQTT 客户端向 `inTopic` 发消息；也可同时订阅 `outTopic` 查看回显。

## 故障排除

- 无转发：检查回调声明顺序与堆内存
- 断连：保持回调尽量短

## 相关示例

- mqtt_basic
- mqtt_large_message
