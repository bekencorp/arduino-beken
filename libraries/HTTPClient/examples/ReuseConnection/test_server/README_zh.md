# keep-alive 测试服务端

给 `ReuseConnection.ino` 用的短响应 keep-alive 测试页。

## 文件

- `keepalive.php`：返回纯文本，并设置 `Connection: keep-alive`

## 部署到 KSWEB（手机）

1. 把 `keepalive.php` 复制到 KSWEB 的网站根目录，例如：
   - `/storage/emulated/0/htdocs/keepalive.php`
2. 确认 KSWEB 已启动，记下手机局域网 IP（例如 `192.168.1.50`）和端口（常见 `8080`）
3. 用电脑浏览器先验证：
   - `http://192.168.1.50:8080/keepalive.php`
4. 应看到类似：

```text
OK keep-alive
time=2026-07-09 10:15:00
remote=192.168.1.100
method=GET
path=/keepalive.php
x-request-count=-
```

## 修改板端 URL

在 `ReuseConnection.ino` 中改为：

```cpp
const char *httpUrl = "http://192.168.1.50:8080/keepalive.php";
```

把 IP/端口换成你手机上的实际值。

## 如何判断复用是否成功

看板端串口：

- 成功复用：多次 `[HTTP] GET... code: 200`，中间**很少**出现 `lwip_socket` / `lwip_close`
- 未复用或服务端主动断开：每次请求都有 `create socked` + `close`，或交替出现 `connection lost`

## 说明

- 纯 `.html` 无法可靠设置 `Connection: keep-alive`（由 Web 服务器决定），所以这里用 PHP 显式写响应头
- 若 KSWEB/lighttpd 仍强制关闭连接，需要在 KSWEB 的 lighttpd 配置里确认未禁用 keep-alive
