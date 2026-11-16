#!/usr/bin/env python3
import argparse
import functools
import http.server
import pathlib
import shutil
import socketserver
import subprocess
import sys
import webbrowser

DEFAULT_PORT = 8060
PROJECT_ROOT = pathlib.Path(__file__).resolve().parents[2]
BUILD_WEB_DIR = PROJECT_ROOT / "build" / "web"


class ReusableTCPServer(socketserver.TCPServer):
    allow_reuse_address = True


class GodotWebHandler(http.server.SimpleHTTPRequestHandler):
    extensions_map = {
        **http.server.SimpleHTTPRequestHandler.extensions_map,
        ".wasm": "application/wasm",
    }

    def end_headers(self):
        # Mandatory security headers for Godot WebAssembly & SharedArrayBuffer
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        # Prevent stale .wasm / .pck caching between re-exports
        self.send_header("Cache-Control", "no-store, no-cache, must-revalidate")
        super().end_headers()


def open_browser(url: str) -> None:
    firefox_bin = shutil.which("firefox")
    if firefox_bin is not None:
        subprocess.Popen(
            [firefox_bin, url],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            start_new_session=True,
        )
        return

    webbrowser.open(url)


def main() -> int:
    parser = argparse.ArgumentParser(description="Local COOP/COEP web server for Godot WASM builds.")
    parser.add_argument("--target", choices=("release", "debug"), default="release", help="Build folder to serve")
    parser.add_argument("--port", type=int, default=DEFAULT_PORT, help="Port to listen on")
    parser.add_argument("--no-open", action="store_true", help="Do not automatically launch Firefox")
    args = parser.parse_args()

    serve_dir = BUILD_WEB_DIR / args.target
    if not (serve_dir / "index.html").is_file():
        print(f"Error: '{serve_dir / 'index.html'}' not found. Export the project first.", file=sys.stderr)
        return 1

    handler = functools.partial(GodotWebHandler, directory=str(serve_dir))
    url = f"http://localhost:{args.port}"

    with ReusableTCPServer(("localhost", args.port), handler) as httpd:
        print(f"Serving '{serve_dir}' at: {url}")
        if not args.no_open:
            open_browser(url)

        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nServer stopped.")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())