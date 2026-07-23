# MQTT 认证示例

## 概述

连接 WiFi 后，使用用户名/密码登录 MQTT Broker，向 `outTopic` 发布消息并订阅 `inTopic`。

默认对接 [test.mosquitto.org](https://test.mosquitto.org/) 的认证端口 **1884**，使用官方公开测试账号，无需注册。

## 功能特性

- WiFi STA，等待 `WSS_GOT_IP`
- 带用户名密码的 MQTT CONNECT
- 连接成功后发布并订阅

## 硬件要求

- Beken BK7239N 开发板
- 能访问 `test.mosquitto.org:1884`（或自备认证 Broker）

## 配置说明

```cpp
WIFI_SSID / WIFI_PASS

const char* mqtt_server = "test.mosquitto.org";
const uint16_t mqtt_port = 1884;
const char* mqtt_user = "rw";
const char* mqtt_pass = "readwrite";
```

其他公开账号：`ro`/`readonly`、`wo`/`writeonly`。

## 工作原理

1. 连接 WiFi 并等待 IP
2. `client.connect(clientId, user, pass)`
3. 发布公告并订阅
4. `client.loop()` 维持会话

## 使用方法

1. 修改 WiFi
2. 上传后打开串口监视器（115200）
3. 确认出现 `MQTT connected (auth OK)`

## 预期输出

```
mqtt_auth starting
Broker test.mosquitto.org:1884 user=rw
WiFi connected
IP address: 192.168.x.x
MQTT connected (auth OK)
Published announcement to outTopic
Subscribed to inTopic
```

## 故障排除

- 认证失败：确认端口是 **1884**（不是 1883），账号为 `rw`/`readwrite`
- 凭据过长：调用 `client.setBufferSize(255)`
- 公开 Broker 后续可能断连（`rc=-4`）；本例程不含重连

## 相关示例

- mqtt_basic
- mqtt_bk7239
