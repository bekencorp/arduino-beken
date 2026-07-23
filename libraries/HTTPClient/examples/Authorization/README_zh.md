# Authorization 示例

## 概述

这个示例演示如何在 `arduino-beken` 平台上使用 `HTTPClient` 完成 HTTP Basic Authorization。它通过 `WiFiMulti` 连接 WiFi 后，依次发起两次请求：

1. 使用 `http.setAuthorization(user, password)` 的成功路径
2. 不带 `Authorization` 头的失败路径

默认使用公开的 httpbin Basic Auth 端点验证：正确凭据返回 `200`，无凭据返回 `401`。

## 功能特性

- 使用 `WiFiMulti` 连接 WiFi
- 在发起 HTTP 前等待 `WSS_GOT_IP`
- 使用 `http.begin(WiFiClient&, url)`
- 演示 `setAuthorization(user, password)`
- 同时打印成功与失败两条路径

## 硬件要求

- Beken BK7239N 开发板
- 用于烧录和串口监视的 USB 线
- 可访问外网的 WiFi 网络（默认 httpbin 地址需要上网）

## 配置说明

先修改示例顶部的通用 WiFi 配置段：

```cpp
// -------- WiFi configuration (edit before upload) --------
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"  // 改成你的 WiFi 名称
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"  // 改成你的 WiFi 密码
#endif
// ----------------------------------------------------------
```

默认 Basic Authorization 端点已设为在线测试地址：

```cpp
#ifndef AUTH_HTTP_URL
// Online Basic Auth test endpoint (user/passwd must match URL path)
#define AUTH_HTTP_URL "http://httpbin.org/basic-auth/user/passwd"
#endif
#ifndef AUTH_USER
#define AUTH_USER "user"  // Basic Auth 用户名
#endif
#ifndef AUTH_PASS
#define AUTH_PASS "passwd"  // Basic Auth 密码
#endif
```

说明：

- `AUTH_HTTP_URL` 必须以 `http://` 开头
- 本示例不启用 HTTPS
- 用户名/密码必须与 httpbin URL 路径一致（`/basic-auth/<user>/<passwd>`）
- 目标端点必须是 HTTP Basic Auth，而不是网页表单登录页
- 如需也可改成局域网 Basic Auth 服务
- 也可以在编译时用 `-DWIFI_SSID=\"...\" -DWIFI_PASS=\"...\"` 覆盖 WiFi 凭据

## 工作原理

1. 串口初始化为 115200 波特率
2. 通过 `WiFiMulti` 连接 WiFi
3. 等待 `WiFi.status() == WSS_GOT_IP`
4. 调用 `http.begin(client, AUTH_HTTP_URL)`
5. 调用 `http.setAuthorization(AUTH_USER, AUTH_PASS)` 后发送 `GET()`
6. 打印状态码，并在可用时打印正文
7. 再发送一次不带凭据的 `GET()`，用于展示被拒绝路径
8. 10 秒后重复整轮流程

## 使用方法

1. 设置 `WIFI_SSID` 和 `WIFI_PASS`
2. 保持默认 httpbin 地址/凭据，或自行修改 `AUTH_HTTP_URL`、`AUTH_USER`、`AUTH_PASS`
3. 上传示例
4. 打开 115200 波特率串口监视器
5. 确认带鉴权请求返回 `200`，不带鉴权请求返回 `401`

## 预期输出

```text
[SETUP] WAIT 4...
[SETUP] WAIT 3...
[SETUP] WAIT 2...
[SETUP] WAIT 1...
[SETUP] URL: http://httpbin.org/basic-auth/user/passwd
[SETUP] user: user
[HTTP] begin with correct auth
[HTTP] setAuthorization(user, ...)
[HTTP] GET...
[HTTP] GET... code: 200
{
  "authenticated": true,
  "user": "user"
}
[HTTP] begin without auth
[HTTP] no Authorization header
[HTTP] GET...
[HTTP] GET... code: 401
```

## 故障排除

**`[HTTP] begin failed`**
- 检查 URL 是否以 `http://` 开头
- 确认开发板可以访问外网（或你的本地服务）

**带正确凭据仍返回 `401`**
- 重新核对用户名和密码
- 确认它们与 httpbin URL 路径一致
- 确认该端点真的是 HTTP Basic Auth，而不是网页表单登录页

**不带鉴权反而返回 `200`**
- 当前 URL 很可能是公开页面
- 请改成受 Basic Auth 保护的路径

## 注意事项

- 在 `arduino-beken` 上优先使用 `http.begin(client, url)` + `setAuthorization(user, password)`
- `http://user:passwd@host/...` 这类 URL 内嵌凭据仅作为注释参考保留

## 相关示例

- `BasicHttpClient` - 不带鉴权的基础 HTTP GET
- `BasicHttpsClient` - 带根证书校验的 HTTPS GET
- `StreamHttpClient` - 响应体流式读取示例
