#!/usr/bin/env python3
"""A stand-in OSM Scout Server for meego-maps' tests: raster tiles (a grid
with each tile's z/x/y; needs Pillow) and a search that finds a few
made-up places near the given point.

    fake-osmscout.py [PORT]      (default 18553)
    MEEGO_MAPS_OSMSCOUT=http://127.0.0.1:18553 meego-maps
"""
import io
import json
import sys
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import parse_qs, urlparse

from PIL import Image, ImageDraw

PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 18553


def tile(z, x, y):
    img = Image.new("RGB", (256, 256), (242, 239, 233) if (x + y) % 2 else (232, 236, 224))
    d = ImageDraw.Draw(img)
    d.rectangle([0, 0, 255, 255], outline=(200, 196, 188))
    # a "road" across each tile and a "river" down some
    d.line([(0, 128), (255, 128)], fill=(255, 255, 255), width=10)
    d.line([(0, 128), (255, 128)], fill=(250, 210, 140), width=6)
    if x % 3 == 0:
        d.line([(96, 0), (160, 255)], fill=(170, 211, 223), width=14)
    d.text((8, 8), "%d/%d/%d" % (z, x, y), fill=(120, 120, 120))
    out = io.BytesIO()
    img.save(out, "PNG")
    return out.getvalue()


class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        u = urlparse(self.path)
        q = {k: v[0] for k, v in parse_qs(u.query).items()}
        if u.path == "/v1/tile":
            body, kind = tile(int(q["z"]), int(q["x"]), int(q["y"])), "image/png"
        elif u.path == "/v1/search":
            lat, lng = float(q.get("lat", 47.56)), float(q.get("lng", 13.65))
            word = q.get("search", "")
            places = [dict(title="%s %s" % (word.capitalize(), n), admin_region="Testdorf, Salzkammergut",
                           lat=lat + 0.01 * i, lng=lng + 0.012 * i, type="amenity_cafe")
                      for i, n in enumerate(["Mühle", "am See", "Bahnhof"], 1)]
            body, kind = json.dumps(places[: int(q.get("limit", 25))]).encode(), "application/json"
        else:
            self.send_error(404)
            return
        self.send_response(200)
        self.send_header("Content-Type", kind)
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, *args):
        pass


ThreadingHTTPServer(("127.0.0.1", PORT), Handler).serve_forever()
