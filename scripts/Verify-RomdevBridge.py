"""Test discovery against a mock server, without installing/running romdev."""
import http.server
import json
import pathlib
import subprocess
import sys
import tempfile
import threading

requests = []


class Metadata(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        requests.append(self.path)
        self.send_response(200)
        self.end_headers()
        self.wfile.write(json.dumps({"info": {"title": "mock-romdev"}, "paths": {"/tool/frame": {}, "/tool/build": {}}}).encode())

    def log_message(self, *args):
        pass


root = pathlib.Path(__file__).resolve().parents[1]
server = http.server.ThreadingHTTPServer(("127.0.0.1", 0), Metadata)
thread = threading.Thread(target=server.serve_forever, daemon=True)
thread.start()
endpoint = f"http://127.0.0.1:{server.server_port}"


def call(*args):
    return subprocess.run([sys.executable, str(root / "scripts/Romdev-Bridge.py"), *args], capture_output=True, text=True, timeout=10)


try:
    result = call("--probe", "--endpoint", endpoint)
    data = json.loads(result.stdout)
    assert result.returncode == 0 and data["connected"] and not data["game_uploaded"]
    assert data["tool_routes"] == ["/tool/build", "/tool/frame"]
    baseline = len(requests)
    for name in ["game.jar", "game.jad", "game.sis", "game.sisx"]:
        result = call("--game", name, "--probe", "--endpoint", endpoint)
        assert result.returncode == 2 and len(requests) == baseline, "Unsupported game caused network traffic"
    result = call("--probe", "--endpoint", "https://example.com")
    assert result.returncode == 2 and len(requests) == baseline
    # A fixture checks extension classification only, not actual console emulation.
    fixture = root / "build/romdev-bridge-fixture.nes"
    fixture.parent.mkdir(parents=True, exist_ok=True)
    fixture.write_bytes(b"NES\x1a" + bytes(12))
    result = call("--game", str(fixture))
    data = json.loads(result.stdout)
    assert result.returncode == 0 and data["candidate_platform"] == "nes" and data["content_triage_required"]
    assert len(requests) == baseline
    print("PASS: mock OpenAPI discovery; J2ME/Symbian blocked before requests; no file upload; local endpoint restriction; candidate-only classification")
finally:
    server.shutdown()
    server.server_close()
