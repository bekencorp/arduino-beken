<?php
/**
 * HTTP keep-alive test endpoint for ReuseConnection.ino
 *
 * Deploy: copy this file into KSWEB htdocs (or any PHP server),
 * then point the sketch to:
 *   http://<phone-ip>:8080/keepalive.php
 *
 * Expected response headers include:
 *   Connection: keep-alive
 *   Keep-Alive: timeout=30, max=100
 */

header('Content-Type: text/plain; charset=utf-8');
header('Connection: keep-alive');
header('Keep-Alive: timeout=30, max=100');
header('Cache-Control: no-store');

$now = date('Y-m-d H:i:s');
$count = isset($_SERVER['HTTP_X_REQUEST_COUNT']) ? $_SERVER['HTTP_X_REQUEST_COUNT'] : '-';

echo "OK keep-alive\n";
echo "time={$now}\n";
echo "remote=" . ($_SERVER['REMOTE_ADDR'] ?? 'unknown') . "\n";
echo "method=" . ($_SERVER['REQUEST_METHOD'] ?? 'GET') . "\n";
echo "path=" . ($_SERVER['REQUEST_URI'] ?? '/keepalive.php') . "\n";
echo "x-request-count={$count}\n";
