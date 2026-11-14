"""Optional local romdev discovery. Never installs tools or uploads a game."""
import argparse
import json
import pathlib
import sys
import urllib.error
import urllib.parse
import urllib.request


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--probe", action="store_true")
    parser.add_argument("--game", type=pathlib.Path)
    parser.add_argument("--endpoint", default="http://127.0.0.1:7331")
    args = parser.parse_args()
    profile = json.loads((pathlib.Path(__file__).resolve().parents[1] / "engine/tools/romdev.json").read_text(encoding="utf-8"))
    result = {"adapter": "romdev", "scope": "discovery-only", "game_uploaded": False}
    if args.game:
        extension = args.game.suffix.lower()
        if extension in profile["unsupported_here"]:
            result.update(accepted=False, reason="J2ME/Symbian is not a documented romdev target; use the existing Forge route.")
            print(json.dumps(result))
            return 2
        platform = profile["candidate_formats"].get(extension)
        if not platform or not args.game.is_file():
            result.update(accepted=False, reason="Supply an existing candidate console file; extension alone is not content validation.")
            print(json.dumps(result))
            return 2
        result.update(accepted=True, candidate_platform=platform, content_triage_required=True)
    if args.probe:
        address = urllib.parse.urlsplit(args.endpoint)
        if address.scheme != "http" or address.hostname not in {"127.0.0.1", "localhost", "::1"} or address.username or address.password or address.query or address.fragment or address.path not in {"", "/"}:
            parser.error("Use a local HTTP endpoint, for example http://127.0.0.1:7331")
        try:
            request = urllib.request.Request(args.endpoint.rstrip("/") + "/openapi.json", headers={"User-Agent": "LegacyMobileForge-romdev-discovery"})
            with urllib.request.urlopen(request, timeout=5) as response:
                payload = response.read(1024 * 1024 + 1)
            if len(payload) > 1024 * 1024:
                raise ValueError("OpenAPI response is too large")
            schema = json.loads(payload)
            if not isinstance(schema, dict) or not isinstance(schema.get("paths"), dict):
                raise ValueError("No OpenAPI paths returned")
            paths = [p for p in schema["paths"] if p.startswith("/tool/")]
            result.update(connected=True, api_title=schema.get("info", {}).get("title", "romdev"), tool_routes=sorted(paths))
        except (OSError, ValueError, urllib.error.URLError) as error:
            result.update(connected=False, reason=str(error))
            print(json.dumps(result))
            return 1
    if not args.probe and not args.game:
        parser.print_help()
        return 0
    print(json.dumps(result))
    return 0


if __name__ == "__main__":
    sys.exit(main())
