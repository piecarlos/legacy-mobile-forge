# Native Windows launcher

The player-facing importer/bootstrap is C, compiled with MSVC for Windows x64 and using the system IFileOpenDialog and TaskDialog APIs. The embedded Common Controls v6 manifest enables Windows visual styles. No Python, CLR or JVM is needed to choose, hash, copy or validate the game file. The current game's execution backend remains Java ME/FreeJ2ME-Plus; this change does not recompile game logic into C++.

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
