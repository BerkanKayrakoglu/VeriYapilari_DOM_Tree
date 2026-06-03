from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, parse_qs
import json
import random

def generate_node(tag, depth, max_depth, width):
    if depth > max_depth:
        return ""
    id_str    = f' id="node_{depth}_{random.randint(1000,9999)}"' if random.random() > 0.5 else ""
    class_str = f' class="group_{random.choice(["container","wrapper","item"])}"' if random.random() > 0.5 else ""
    indent    = "  " * depth
    html      = f"{indent}<{tag}{id_str}{class_str}>\n"
    if depth < max_depth:
        for _ in range(random.randint(1, width)):
            child = random.choice(["div", "p", "span", "section", "article"])
            html += generate_node(child, depth + 1, max_depth, width)
    html += f"{indent}</{tag}>\n"
    return html

def generate_synthetic_html(depth=4, width=3):
    body = generate_node("div", 1, depth, width)
    return f"<html>\n  <head><title>Synthetic Test</title></head>\n  <body id=\"main\">\n{body}  </body>\n</html>"

class Handler(BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        pass  # sessiz log

    def send_json(self, code, obj):
        body = json.dumps(obj, ensure_ascii=False).encode()
        self.send_response(code)
        self.send_header('Content-Type', 'application/json')
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Content-Length', str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        self.end_headers()

    def do_GET(self):
        parsed = urlparse(self.path)
        params = parse_qs(parsed.query)

        if parsed.path == '/health':
            self.send_json(200, {"status": "active", "message": "DOM Tree AI Service calisiyor."})

        elif parsed.path == '/generate':
            depth = int(params.get('depth', [4])[0])
            width = int(params.get('width', [3])[0])
            depth = max(1, min(depth, 8))
            width = max(1, min(width, 5))
            html  = generate_synthetic_html(depth, width)
            self.send_json(200, {"html": html, "depth": depth, "width": width})

        else:
            self.send_json(404, {"error": "endpoint bulunamadi"})

def run(port=5001):
    httpd = HTTPServer(('', port), Handler)
    print(f"AI Service http://localhost:{port} adresinde calisiyor...")
    httpd.serve_forever()

if __name__ == '__main__':
    run()
