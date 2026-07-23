# MQTT 大消息示例

## 概述

演示用 `beginPublish` / `print` / `endPublish` 发布较长 MQTT 载荷。连上 Broker 后，板子会自动按 `DEMO_BOTTLE_COUNT` 发布歌词，**只需一块开发板即可验收**，不需要电脑上的 MQTT 客户端。若外部向 `greenBottles/<数字>` 发消息，仍会触发同样逻辑。

## 功能特性

- WiFi STA，等待 `WSS_GOT_IP`
- 流式发布长载荷
- 通配订阅 `greenBottles/#`
- 单板自测（无需外部 MQTT 客户端）

## 硬件要求

- Beken BK7239N 开发板
- 可访问 MQTT Broker（1883）

## 配置说明

```cpp
WIFI_SSID / WIFI_PASS
const char* mqtt_server = "broker.mqtt-dashboard.com";
#define DEMO_BOTTLE_COUNT 3   // 公开 Broker 建议保持较小
```

## 工作原理

1. 连接 WiFi 与 MQTT
2. 订阅 `greenBottles/#`
3. 连接成功后自动调用 `publishLyrics(DEMO_BOTTLE_COUNT)`
4. 若收到 `greenBottles/<数字>`，也会再次发布歌词

## 使用方法

1. 修改 WiFi
2. 上传程序
3. 打开串口监视器（115200）
4. 等待 `connected` 后的自测日志

## 预期输出

```
WiFi connected
Attempting MQTT connection...connected
Subscribed to greenBottles/#
Self-test: publishing lyrics for 3 bottles
beginPublish greenBottles/lyrics, expectedLen=...
endPublish OK (large message sent)
```

## 故障排除

- 大消息导致断连：减小 `DEMO_BOTTLE_COUNT`
- `endPublish failed`：检查网络后重试

## 相关示例

- mqtt_basic
- mqtt_publish_in_callback
