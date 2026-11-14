# Native Windows launcher

The player-facing importer/bootstrap is C, compiled with MSVC for Windows x64 and using the system IFileOpenDialog and TaskDialog APIs. The embedded Common Controls v6 manifest enables Windows visual styles. No Python, CLR or JVM is needed to choose, hash, copy or validate the game file. The current game's execution backend remains Java ME/FreeJ2ME-Plus; this change does not recompile game logic into C++.

## Standard pre-game shell (0.2.3 local preview)

`engine/native/LauncherUi.h` supplies the shared GDI/WIC Windows interface, inspired by the owner's launcher reference: a game-art header, selected game card, Quick Setup / Display / Controls / System tabs, local status and a fixed Play button. It appears even when a JAR was previously imported. Play validates the cache, saves settings, hides the launcher, starts the game and restores the launcher after the child exits. Error exits are reported in its status and local logs. Closing the launcher leaves the desktop.

Display offers the implemented resolutions (540p, 1080p, 1440p, 3K, 4K), window/fullscreen, sharp/bilinear/Scale2x, integer scaling and frame artwork. No GPU performance presets, DLSS or unrelated 3D settings are implied. The original game's logical resolution is unchanged. New settings default to Full HD windowed with the artwork enabled; saved choices, including compact mode, are retained. Java still clamps the actual window to the monitor work area.

Keyboard: arrows/Tab select controls, Enter/Space activate, Q/E change categories, Esc closes the launcher. Mouse selects directly. The launcher polls Windows XInput for D-pad/left stick, A, B, LB/RB and Start (Play); it does not convert PlayStation pads to XInput or install mappings. This route is distinct from the game's existing controller helper. Physical pad validation remains required.

Both frontends use `app/video.properties`; saving in the launcher preserves unrelated properties. The launcher reloads game-side changes on return. Language follows Windows (English fallback) or the saved English/pt-BR choice. The game text is unchanged. WIC loads optional profile `artwork=assets\bezel.png`; missing artwork falls back to the dark header. The artwork must belong to the specific case. Meteos uses the supplied custom frame with its provenance/hash in `ports/meteos/assets/README.md`.

`Build-GameProject.ps1 -Package` now builds this shell and writes the case-specific INI before starting `MobileGameDesktop`; that developer package still includes the private input JAR and must not be published. `Build-MeteosLauncher.ps1` remains the separate audited game-free public packaging route.

`Verify-LauncherUi.py` exercises the real native window with keyboard/mouse messages, changes all display options/language, reopens it and checks settings persistence. Optional PNG capture uses Windows PrintWindow and developer-side Pillow. A separate original Java lifecycle fixture tests Play/hide/return without pretending it is a gameplay test. Run only on an isolated package without personal saves/preferences. These tests do not establish physical controller compatibility.

## Reuse in another Java game case

- Shared source: `engine/native/GameLauncher.c`; compiler script: `scripts/Build-NativeLauncher.ps1`.
- Package root: native executable, `runtime/`, `app/`. Metadata: `app/launcher.ini` (UTF-16LE for Windows INI APIs).
- Profile fields: `[launcher]`, `backend=java`, title, game_version, resolution, reference filename, required byte size/SHA-256, safe relative game_path below app/, entry_class and relative classpath.
- The native frontend reads the saved language from `app/video.properties`, otherwise Windows UI language; English fallback and pt-BR strings are built in.
- Game imports validate the source and temporary copied file before replacing an invalid cache. A valid cache is retained; saves/settings are untouched. Selected paths are not recorded in the native log.
- Native import/bootstrap events: `app/logs/launcher.log`, rotating to `launcher.previous.log` at 1 MiB. Backend logs retain their existing session policy.
- Normal launch starts the bundled Java only after validation. `--reshade` loads the optional DLL through the backend bootstrap after native import; no ReShade binary is bundled.

The generic frontend currently supports the Java backend only. A future native C++ game backend will need its own launch contract after a playable proof; do not label this importer as a game recompiler.

## Build and checks

MSVC C++ Build Tools (C17 compiler) and Windows SDK are build-time requirements. `/MT` links the compiler runtime into the frontend; the player does not need build tools or Python. The public CI runner builds the same source. The bundled JVM and existing backend license notices are preserved.

`Verify-LauncherArchive.py` checks native x64 PE/no CLR, absence of a Swing file picker, game-free archive contents and optional private JAR import. It exercises a Unicode/space-containing source path, invalid variants, cache preservation and a dialog API probe with javaw unavailable. The probe constructs/configures IFileOpenDialog without showing a window; it does not constitute visual approval of the UI. Review the appearance on a real Windows desktop.

Useful flags for development: `--validate`, `--import FILE --import-only`, `--probe-dialog`, `--backend-validate`, `--smoke`, `--reshade`. `--probe-dialog` initializes the native API without opening a picker or starting Java. `--backend-validate` starts the bundled JVM for the headless backend's independent JAR check, exercising the process/classpath handoff without starting a game or synthesizer.
