import socket
import sys

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
except socket.error as msg:
    print('Failed to create socket:', msg)
    sys.exit()

try:
    s.bind(('', 3333))
except socket.error as msg:
    print('Bind failed:', msg)
    sys.exit()

print('Server listening')

while True:
    data, _ = s.recvfrom(1024)
    if not data:
        break
    print(data.decode('utf-8', errors='replace').strip())

s.close()
