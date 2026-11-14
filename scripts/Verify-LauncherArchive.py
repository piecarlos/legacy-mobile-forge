"""Check a game-free public archive; optional private end-to-end import test."""
import argparse
import hashlib
import pathlib
import subprocess
import tempfile
import zipfile

EXPECTED = "5756d421e3b878d124fb9c8453a6738ecb99ff6425679668ab2cb6e7b6df5021"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("archive", type=pathlib.Path)
    parser.add_argument("--game", type=pathlib.Path)
    parser.add_argument("--wrong-jar", type=pathlib.Path)
    parser.add_argument("--scratch-root", type=pathlib.Path)
    parser.add_argument("--play-smoke", action="store_true")
    args = parser.parse_args()
    with zipfile.ZipFile(args.archive) as archive:
        assert archive.testzip() is None, "Corrupt archive"
        names = [n.replace("\\", "/").lower() for n in archive.namelist()]
        assert any(n.endswith("/meteoslauncher.exe") for n in names)
        assert not any("/game/" in n or "/saves/" in n or n.endswith((".log", "bezel.png", "d3d9.dll")) for n in names), "Private content in public archive"
        assert not any(hashlib.sha256(archive.read(n)).hexdigest() == EXPECTED for n in archive.namelist() if not n.endswith("/")), "Original game bytes found"
        print("PASS: ZIP integrity, launcher present, no original JAR/saves/logs/border/ReShade DLL")
        if args.game is None:
            return
        assert hashlib.sha256(args.game.read_bytes()).hexdigest() == EXPECTED
        if args.scratch_root:
            args.scratch_root.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="meteos-launcher-test-", dir=args.scratch_root) as scratch:
            root = pathlib.Path(scratch)
            if args.scratch_root:
                assert root.resolve().parent == args.scratch_root.resolve(), "Unexpected test directory"
            archive.extractall(root)
            exe = next(root.rglob("MeteosLauncher.exe"))
            app = exe.parent / "app"
            target = app / "game" / "meteos.jar"

            def run(arguments, expected):
                process = subprocess.Popen([str(exe), *arguments], cwd=root, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                try:
                    stdout, stderr = process.communicate(timeout=20)
                except subprocess.TimeoutExpired:
                    subprocess.run(["taskkill.exe", "/PID", str(process.pid), "/T", "/F"], capture_output=True)
                    process.communicate(timeout=5)
                    raise
                if process.returncode != expected:
                    raise AssertionError(f"Unexpected exit {process.returncode}: {stdout}\n{stderr}")

            run(["--validate"], 1)
            assert not target.exists(), "Game created without import"
            if args.wrong_jar:
                run(["--import", str(args.wrong_jar), "--import-only"], 1)
                assert not target.exists(), "Wrong JAR accepted"
            run(["--import", str(args.game), "--import-only"], 0)
            assert hashlib.sha256(target.read_bytes()).hexdigest() == EXPECTED
            if args.wrong_jar:
                run(["--import", str(args.wrong_jar), "--import-only"], 1)
                assert hashlib.sha256(target.read_bytes()).hexdigest() == EXPECTED, "Valid cached game overwritten"
            run(["--validate"], 0)
            assert list((app / "logs").glob("session-*.log"))
            print("PASS: fresh extraction, missing/wrong JAR rejection, valid import, cached validation, retained game and local logs")
            if args.play_smoke:
                run(["--smoke"], 0)
                assert (app / "smoke-result.txt").is_file()
                print("PASS: game boot/menu/cursor/helper/exit smoke")


if __name__ == "__main__":
    main()
