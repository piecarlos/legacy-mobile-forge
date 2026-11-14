# Meteos PC — testing preview

Windows x64 adaptation using Java ME / FreeJ2ME-Plus. **This is not native C++ recompilation.** The normal build is recommended; ReShade is optional and experimental. No original game, commercial artwork or personal saves are included in the public repository.

## Required game — exact supported variant

| Field | Required value |
| --- | --- |
| Game | Meteos - Astro Blocks |
| MIDlet version | **1.0.4** |
| Original screen | **240 × 320** |
| Reference filename | `meteos_astro_blocks_240x320_171711.jar` |
| File size | **303,829 bytes** |
| MIDlet class | `GloftMTOS` |
| SHA-256 | `5756d421e3b878d124fb9c8453a6738ecb99ff6425679668ab2cb6e7b6df5021` |

Supply your own JAR. Filename alone is insufficient: renamed or different handset variants are rejected. JAD, ZIP and other screen sizes are not interchangeable. This repository does not supply download links to the commercial game.

## Build your portable copy

For a prebuilt game-free launcher, use [Releases](https://github.com/piecarlos/legacy-mobile-forge/releases/tag/v0.2.2-preview.1). Extract the ZIP, open MeteosLauncher.exe and select your original JAR. Java is included; no compiler is needed to play.

Building from source requires Windows x64, PowerShell and **JDK 21+**. Craft Anything is used for additional triage when installed. From the repository root:

```powershell
.\scripts\Setup-Dependencies.ps1
.\scripts\Build-MeteosPreview.ps1 -GamePath 'C:\Games\meteos_astro_blocks_240x320_171711.jar' -Package
```

Pass `-JdkPath 'C:\Path\To\jdk'` if Java is not on PATH. The script prints the generated `MeteosPreview.exe` path. The portable result includes Java; players do not need to install it separately. Keep its entire folder together. Your generated output contains your original game and must stay private.

The public builder uses the backend pinned in `dependencies.lock.json`. Earlier private Meteos 1.0.2 packages used a different snapshot; do not mix runtime JARs between packages. The illustrated private border is omitted; public builds use the neutral built-in border unless you supply `app/assets/bezel.png`.

## Controls and settings

- D-pad / arrows / WASD: move. A / Enter / Space: action.
- Start / Esc: original game pause menu. Q: left softkey; E / B: right softkey. X / Y: phone key 0.
- Select/Back or F2: port settings. Up/down select; left/right change; A confirms; B/Select closes. LB/RB switch tabs.
- F11: window / borderless fullscreen. Exit in the original game or Quit to desktop closes the application.
- General: system language / English / pt-BR, and Open log folder. This changes the port interface, not the original game's text.
- Window presets up to 4K preserve the original logical screen. Sharp, smooth and Scale2x are image filters, not HD textures.

User testing confirmed a GameSir controller on the private build. Other controllers, machines and connections are not physically validated. The Meteos adapter retains its original Win32 controller backend; the generic Forge engine also offers SDL3.

## Optional ReShade

Use the normal executable first. ReShade binaries and third-party shaders are not bundled. Obtain the official **64-bit** ReShade from <https://reshade.me/>, place its Direct3D 9 DLL beside the portable executable as `d3d9.dll`, then use `Run-ReShade.cmd`. The repository's original shader is in `engine/reshade`; additional includes must come from their official projects. HOME opens/closes ReShade. OpenGL is not recommended: a driver crash occurred on the tested AMD setup. See `docs/RESHADE.md` in the repository.

## Report a bug

Open **General → Open log folder**, or use `app/logs`. Close the game, attach the latest `session-*.log` to a GitHub issue and describe what happened, the approximate time, and whether ReShade was active. Review the file before sharing. No log is uploaded automatically. Logs are capped at ten sessions, 2 MiB each, with known user/app paths redacted. Native driver crashes may need a separate crash report.

Settings: `app/video.properties`. Saves: `app/saves`. Back these up before replacing a portable folder. Never attach your original JAR or saves to a public issue. Avoid replacing a working installation: extract each test version into a fresh folder.

## Português

Use a versão **1.0.4, 240×320**, com tamanho e SHA-256 da tabela. O nome sozinho não identifica a variante. Forneça seu próprio JAR; não incluímos o jogo. Em Releases, o launcher pronto pede o JAR na primeira execução; Java já acompanha e não é necessário compilar. Os comandos acima são para build a partir dos fontes. A versão normal é a recomendada e o ReShade é experimental. Logs ficam em `app/logs` e podem ser abertos na aba Geral. Não existe telemetria automática.
