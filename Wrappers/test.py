import socket

host = "127.0.0.1"
port = 8045

with socket.socket(socket.AF_NET, socket.SOCK_STREAM) as s:
    s.connect(host, port)
    s.send()