#!/usr/bin/env python3
import os
from http.server import HTTPServer, SimpleHTTPRequestHandler
import webbrowser

class CustomHandler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory="classes_viz", **kwargs)

def run_server(port=8000):
    server_address = ('', port)
    httpd = HTTPServer(server_address, CustomHandler)
    print(f"Server läuft auf http://localhost:{port}")

    # Browser automatisch öffnen
    webbrowser.open(f'http://localhost:{port}')

    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nServer wird beendet...")
        httpd.server_close()

if __name__ == '__main__':
    # Zum richtigen Verzeichnis wechseln
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    run_server()
