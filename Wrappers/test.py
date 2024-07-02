import socket
import threading
import time

host = "127.0.0.1"
port = 8045
class SocketHandler:
    def __init__(self):
        self.thread_read = None
        self.thread_write = None
        self.read_data = ""
        self.r_ready = False
        self.w_ready = False
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect((host, port))
    def read_socket(self):
        while True:
            if not self.r_ready: self.read_data = self.s.recv(1024).decode(errors="ignore").strip()
            self.r_ready = True
    def write_socket(self):
        payload = input("")
        self.s.send(payload.encode())
    def start_threads(self):
        self.thread_read = threading.Thread(target=self.read_socket)
        self.thread_write = threading.Thread(target=self.write_socket)
        self.thread_read.start()
        self.thread_write.start()

handler = SocketHandler()
handler.start_threads()

while True:
    if handler.r_ready:
        print(handler.read_data)
        handler.r_ready = False
        handler.read_data = ""
    time.sleep(0.5)
