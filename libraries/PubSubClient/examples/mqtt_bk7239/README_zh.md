# MQTT BK7239 示例

## 概述

连接 WiFi 后接入 MQTT Broker：每 2 秒向 `outTopic` 发布计数消息，订阅 `inTopic`，打印收到的载荷；若载荷首字符为 `1` 则点亮 LED。

## 功能特性

- WiFi STA，等待 `WSS_GOT_IP`
- 阻塞式 MQTT 重连
- 周期发布与订阅回调
- 可选 GPIO LED 控制

## 硬件要求

- Beken BK7239N 开发板（arduino-beken）
- USB 线用于烧录与串口监视
- 可访问 MQTT Broker（默认 `broker.mqtt-dashboard.com:1883`）

## 配置说明

```cpp
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif

const char* mqtt_server = "broker.mqtt-dashboard.com";
// LED 使用 LED_BUILTIN（BK7239N 上为 GPIO 24）
```

## 工作原理

1. 连接 WiFi，等待 `WSS_GOT_IP`
2. 设置 MQTT 服务器与回调
3. 连接成功后发布公告并订阅 `inTopic`
4. 每 2 秒发布 `hello world #N`
5. 回调打印消息并控制 LED

## 使用方法

1. 修改 WiFi 与 Broker
2. 上传程序
3. 打开串口监视器（115200）
4. 用其他 MQTT 客户端向 `inTopic` 发消息验证回调（发 `1` / 其他控制 LED）

## 预期输出

```
Connecting to your-ssid
.....
WiFi connected
IP address: 192.168.x.x
Attempting MQTT connection...connected
Publish message: hello world #1
```

## 故障排除

- 一直打印点：检查 SSID/密码与 2.4 GHz 网络
- MQTT `rc` 失败：检查 Broker、1883 端口与 DNS
- LED 无反应：确认板载灯接在 `LED_BUILTIN`（GPIO 24），且为低电平点亮

## 注意事项

- 必须等到 `WSS_GOT_IP` 再建立 MQTT TCP
- 非阻塞重连见 `mqtt_reconnect_nonblocking`

## 相关示例

- mqtt_basic
- mqtt_auth
- mqtt_reconnect_nonblocking
