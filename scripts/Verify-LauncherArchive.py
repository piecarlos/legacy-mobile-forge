"""Check a game-free public archive; optional private end-to-end import test."""
import argparse
import hashlib
import pathlib
import subprocess
import tempfile
import zipfile
import io
import struct

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
        assert not any("/game/" in n or "/saves/" in n or n.endswith((".log", "d3d9.dll")) for n in names), "Private content in public archive"
        borders = [n for n in archive.namelist() if n.replace("\\", "/").lower().endswith("/assets/bezel.png")]
        if borders:
            assert len(borders) == 1
            assert hashlib.sha256(archive.read(borders[0])).hexdigest() == "1a06bfa0a898a50843af4cbf241eb28394dce5ebd6b38c552eefdcd49b9e6c9d", "Unexpected border asset"
        assert not any(hashlib.sha256(archive.read(n)).hexdigest() == EXPECTED for n in archive.namelist() if not n.endswith("/")), "Original game bytes found"
        print("PASS: ZIP integrity, launcher present, no original JAR/saves/logs/ReShade DLL; supplied border hash checked if present")
        if any(n.endswith("/app/launcher.ini") for n in names):
            executable = archive.read(next(n for n in archive.namelist() if n.lower().endswith("/meteoslauncher.exe")))
            pe = struct.unpack_from("<I", executable, 0x3C)[0]
            assert executable[pe:pe + 4] == b"PE\0\0"
            assert struct.unpack_from("<H", executable, pe + 4)[0] == 0x8664, "Native frontend is not x64"
            optional = pe + 24
            assert struct.unpack_from("<H", executable, optional)[0] == 0x20B
            assert struct.unpack_from("<II", executable, optional + 112 + 14 * 8) == (0, 0), "Frontend depends on CLR"
            with zipfile.ZipFile(io.BytesIO(archive.read(next(n for n in archive.namelist() if n.endswith("/app/meteos-preview.jar"))))) as launcher:
                assert not any(b"javax/swing/JFileChooser" in launcher.read(n) for n in launcher.namelist() if n.endswith(".class")), "Java file picker reintroduced"
            print("PASS: native x64 PE, no CLR, no Swing file picker in launcher classes")
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
            native = (app / "launcher.ini").is_file()

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

            if native:
                # Import/validation must work even if Java cannot be started.
                javaw = exe.parent / "runtime/bin/javaw.exe"
                hidden_javaw = javaw.with_suffix(".native-test")
                assert javaw.resolve().is_relative_to(exe.parent.resolve())
                javaw.rename(hidden_javaw)
            run(["--validate"], 1)
            assert not target.exists(), "Game created without import"
            if args.wrong_jar:
                run(["--import", str(args.wrong_jar), "--import-only"], 1)
                assert not target.exists(), "Wrong JAR accepted"
            unicode_source = root / "Meu jogo ç 测试.jar"
            unicode_source.write_bytes(args.game.read_bytes())
            run(["--import", str(unicode_source), "--import-only"], 0)
            assert hashlib.sha256(target.read_bytes()).hexdigest() == EXPECTED
            if args.wrong_jar:
                run(["--import", str(args.wrong_jar), "--import-only"], 1)
                assert hashlib.sha256(target.read_bytes()).hexdigest() == EXPECTED, "Valid cached game overwritten"
            run(["--validate"], 0)
            assert list((app / "logs").glob("session-*.log")) or (app / "logs/launcher.log").is_file()
            if native:
                run(["--probe-dialog"], 0)
                hidden_javaw.rename(javaw)
                print("PASS: native import/validation with Java launcher unavailable; Windows IFileOpenDialog API probe")
                run(["--backend-validate"], 0)
                print("PASS: native-to-Java backend handoff and independent backend JAR validation")
            print("PASS: fresh extraction, missing/wrong JAR rejection, valid import, cached validation, retained game and local logs")
            if args.play_smoke:
                run(["--smoke"], 0)
                assert (app / "smoke-result.txt").is_file()
                print("PASS: game boot/menu/cursor/helper/exit smoke")


if __name__ == "__main__":
    main()
