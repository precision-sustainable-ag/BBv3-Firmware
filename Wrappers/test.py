import socket
import threading
import subprocess
import time

port = 8888

def find_port():
    global port
    cmd = "netstat --all --programs | grep tcp"
    process = subprocess.run(cmd, shell=True, capture_output= True)
    target_line = [x for x in process.stdout.decode().split("\n") if "build/par" in x]
    if len(target_line) != 1:
        print("ERROR! Parser process not found! Quitting...")
        exit()
    target_line = [x for x in target_line[0].split(" ") if len(x)>1]
    if "tcp" not in target_line[0]:
        print("ERROR! Unexpected format found! Quitting...")
        exit()
    target_line = target_line[1].split(":")[1]
    if not target_line.isnumeric():
        print("ERROR! Port number invalid! Quitting...")
    port = int(target_line)

host = "192.168.1.8" #"127.0.0.1"
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
        self.s.send((payload + "\r\n").encode())
    def start_threads(self):
        self.thread_read = threading.Thread(target=self.read_socket)
        self.thread_write = threading.Thread(target=self.write_socket)
        self.thread_read.start()
        self.thread_write.start()

#find_port()
handler = SocketHandler()
handler.start_threads()

while True:
    if handler.r_ready:
        print(handler.read_data)
        handler.r_ready = False
        handler.read_data = ""
    time.sleep(0.5)
