gitimport http.server
import socketserver
import subprocess

class Handler(http.server.BaseHTTPRequestHandler):
    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type, Authorization')
        self.end_headers()

    def do_POST(self):
        content_length = int(self.headers.get('Content-Length', 0))
        post_data = self.rfile.read(content_length).decode('utf-8')
        
        self.send_response(200)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Content-Type', 'application/json')
        self.end_headers()

        combined_text = (self.path + " " + post_data).lower()

        if 'start' in combined_text:
            print("[Bridge] Modtog START fra Grafana! Sender til Mosquitto...")
            subprocess.run(["podman", "exec", "iot-mosquitto", "mosquitto_pub", "-h", "localhost", "-t", "robot/commands", "-m", "START"])
            self.wfile.write(b'{"status": "ok", "command": "START"}')
        elif 'stop' in combined_text:
            print("[Bridge] Modtog STOP fra Grafana! Sender til Mosquitto...")
            subprocess.run(["podman", "exec", "iot-mosquitto", "mosquitto_pub", "-h", "localhost", "-t", "robot/commands", "-m", "STOP"])
            self.wfile.write(b'{"status": "ok", "command": "STOP"}')
        else:
            print(f"[Bridge] Hverken start eller stop fundet. Sti: {self.path}, Data: {post_data}")
            self.wfile.write(b'{"status": "error", "message": "No command found"}')

print("Starter Grafana HTTP -> MQTT Bridge på port 5000...")
socketserver.TCPServer(("0.0.0.0", 5000), Handler).serve_forever()