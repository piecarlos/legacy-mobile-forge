"""Exercise the real native window with keyboard/mouse messages on an isolated copy.

Windows only. Optional PNG captures require Pillow in the developer environment.
Does not claim physical controller or full gameplay validation.
"""
import argparse
import ctypes as c
from ctypes import wintypes as w
import pathlib
import subprocess
import time

user = c.WinDLL("user32", use_last_error=True)
gdi = c.WinDLL("gdi32", use_last_error=True)
user.FindWindowW.argtypes = [w.LPCWSTR, w.LPCWSTR]
user.FindWindowW.restype = w.HWND
user.SendMessageW.argtypes = [w.HWND, w.UINT, w.WPARAM, w.LPARAM]
user.SendMessageW.restype = c.c_ssize_t
user.GetClientRect.argtypes = [w.HWND, c.POINTER(w.RECT)]
user.IsWindowVisible.argtypes = [w.HWND]
user.GetDC.argtypes = [w.HWND]
user.GetDC.restype = w.HDC
user.ReleaseDC.argtypes = [w.HWND, w.HDC]
user.PrintWindow.argtypes = [w.HWND, w.HDC, w.UINT]
gdi.CreateCompatibleDC.argtypes = [w.HDC]
gdi.CreateCompatibleDC.restype = w.HDC
gdi.CreateCompatibleBitmap.argtypes = [w.HDC, c.c_int, c.c_int]
gdi.CreateCompatibleBitmap.restype = w.HBITMAP
gdi.SelectObject.argtypes = [w.HDC, w.HGDIOBJ]
gdi.SelectObject.restype = w.HGDIOBJ
gdi.DeleteObject.argtypes = [w.HGDIOBJ]
gdi.DeleteDC.argtypes = [w.HDC]
gdi.GetDIBits.argtypes = [w.HDC, w.HBITMAP, w.UINT, w.UINT, c.c_void_p, c.c_void_p, w.UINT]


def window_for(process):
    found = []
    callback_type = c.WINFUNCTYPE(w.BOOL, w.HWND, w.LPARAM)

    @callback_type
    def callback(hwnd, _):
        pid = w.DWORD()
        user.GetWindowThreadProcessId(hwnd, c.byref(pid))
        name = c.create_unicode_buffer(100)
        user.GetClassNameW(hwnd, name, 100)
        if pid.value == process.pid and name.value == "LegacyMobileForgeLauncher":
            found.append(hwnd)
        return True

    user.EnumWindows(callback, 0)
    return found[0] if found else None


def wait_for(predicate, timeout=8):
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        result = predicate()
        if result:
            return result
        time.sleep(0.05)
    raise AssertionError("Timed out waiting for native UI")


def capture(hwnd, path):
    if path is None:
        return
    from PIL import Image
    import struct
    rect = w.RECT()
    assert user.GetClientRect(hwnd, c.byref(rect))
    width, height = rect.right, rect.bottom
    dc = user.GetDC(hwnd)
    memory = gdi.CreateCompatibleDC(dc)
    bitmap = gdi.CreateCompatibleBitmap(dc, width, height)
    old = gdi.SelectObject(memory, bitmap)
    try:
        assert user.PrintWindow(hwnd, memory, 3), "Native window capture failed"
        gdi.SelectObject(memory, old)
        info = c.create_string_buffer(struct.pack("<IiiHHIIiiII", 40, width, -height, 1, 32, 0, width * height * 4, 0, 0, 0, 0))
        pixels = c.create_string_buffer(width * height * 4)
        assert gdi.GetDIBits(dc, bitmap, 0, height, pixels, info, 0) == height
        image = Image.frombytes("RGB", (width, height), pixels.raw, "raw", "BGRX")
        assert image.getextrema() != ((0, 0), (0, 0), (0, 0)), "Windows returned a blank capture"
        image.save(path)
    finally:
        gdi.SelectObject(memory, old)
        gdi.DeleteObject(bitmap)
        gdi.DeleteDC(memory)
        user.ReleaseDC(hwnd, dc)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("exe", type=pathlib.Path)
    parser.add_argument("--capture-dir", type=pathlib.Path)
    parser.add_argument("--backend-fixture", action="store_true", help="Play test only with an isolated instrumented backend, never a user's runtime")
    args = parser.parse_args()
    exe = args.exe.resolve()
    props = exe.parent / "app/video.properties"
    assert not props.exists(), "Use an isolated package with no preferences/saves"
    assert not (exe.parent / "app/saves").exists(), "Never exercise settings against personal saves"
    if args.capture_dir:
        args.capture_dir.mkdir(parents=True, exist_ok=True)
    def screenshot(hwnd, name):
        capture(hwnd, args.capture_dir / name if args.capture_dir else None)
    def click(hwnd, x, y):
        rect = w.RECT()
        user.GetClientRect(hwnd, c.byref(rect))
        px, py = x * rect.right // 1200, y * rect.bottom // 850
        user.SendMessageW(hwnd, 0x201, 1, px | (py << 16))
        user.SendMessageW(hwnd, 0x202, 0, px | (py << 16))
        user.SendMessageW(hwnd, 0xF, 0, 0)
    def key(hwnd, key):
        user.SendMessageW(hwnd, 0x100, key, 0)
        user.SendMessageW(hwnd, 0x101, key, 0)
        user.SendMessageW(hwnd, 0xF, 0, 0)
    process = subprocess.Popen([str(exe)], cwd=exe.parent)
    try:
        hwnd = wait_for(lambda: window_for(process))
        screenshot(hwnd, "launcher-quick.png")
        key(hwnd, ord("E"))
        screenshot(hwnd, "launcher-display.png")
        click(hwnd, 1108, 342)  # 4K
        click(hwnd, 1024, 418)  # fullscreen
        click(hwnd, 1070, 494)  # Scale2x
        click(hwnd, 744, 570)   # integer scaling off
        key(hwnd, ord("E"))
        screenshot(hwnd, "launcher-controls.png")
        key(hwnd, ord("E"))
        click(hwnd, 884, 342)   # English
        screenshot(hwnd, "launcher-system.png")
        user.SendMessageW(hwnd, 0x10, 0, 0)
        assert process.wait(timeout=8) == 0
        text = props.read_text("ascii")
        for expected in ("resolution=4", "fullscreen=true", "pixelArt=true", "smooth=false", "integerScale=false", "bezel=true", "language=en"):
            assert expected in text, expected
        print("PASS: real native window, four tabs, keyboard navigation, mouse settings, language and preference persistence")
        # Prepare a clean Full HD/Sharp presentation for the next user launch.
        props.write_text("resolution=1\nfullscreen=false\npixelArt=false\nsmooth=false\nintegerScale=true\nbezel=true\nlanguage=auto\ncustom.keep=preserved\n", encoding="ascii")
        process = subprocess.Popen([str(exe)], cwd=exe.parent)
        hwnd = wait_for(lambda: window_for(process))
        if args.backend_fixture:
            assert (exe.parent / "runtime/bin/backend-fixture.marker").is_file()
            click(hwnd, 1060, 785)
            wait_for(lambda: not user.IsWindowVisible(hwnd))
            wait_for(lambda: user.IsWindowVisible(hwnd))
            assert "native.ui.return code=0" in (exe.parent / "app/logs/launcher.log").read_text()
            screenshot(hwnd, "launcher-return.png")
            print("PASS: Play starts instrumented child, hides launcher, restores it after child exit")
        user.SendMessageW(hwnd, 0x10, 0, 0)
        assert process.wait(timeout=8) == 0
        assert "custom.keep=preserved" in props.read_text()
        print("PASS: reopen/close preserves unknown backend preferences")
    finally:
        if process.poll() is None:
            subprocess.run(["taskkill.exe", "/PID", str(process.pid), "/T", "/F"], capture_output=True)
            process.wait(timeout=5)


if __name__ == "__main__":
    main()
