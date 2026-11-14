# Controller/input references supplied by the owner

Reviewed 2026-11-14. This catalogue records upstream documentation/metadata, not local hardware compatibility or an integration of all the projects below. No driver, mapper or new dependency was installed.

## Choice for Forge

Keep SDL3 plus the pinned SDL_GameControllerDB as the main reusable input route. Both are already in dependencies.lock.json and the generic engine's SDL bridge. The Meteos game adapter retains its earlier Win32 helper; the new native launcher currently polls XInput. Unifying the launcher and game through SDL3 is a subsequent implementation step, not completed by this reference update. See UNCHARTED-REFERENCE.md for the inspected SDL/hotplug and focus-navigation example.

SDL2 calls this the GameController API; SDL3 calls it the Gamepad API (`SDL_gamepad.h`, `SDL_OpenGamepad`, etc.). Use the version matching the existing bridge. A mapping database helps normalize known devices; it is not proof that a particular Xbox, PlayStation or generic pad works on every connection.

## Catalogue

| Project | Role / current assessment | Forge priority |
| --- | --- | --- |
| [SDL](https://github.com/libsdl-org/SDL) | Cross-platform gamepad input. SDL3 gamepad documentation reviewed; pinned dependency already present | Main input route |
| [SDL_GameControllerDB](https://github.com/mdqinc/SDL_GameControllerDB) | Community mappings. Repository metadata available, Zlib license; existing snapshot pinned locally | Main mapping source |
| [gilrs](https://gitlab.com/gilrs-project/gilrs) ([GitHub mirror](https://github.com/Arvamer/gilrs)) | Rust gamepad/force-feedback library; upstream docs say Windows defaults to WGI, with an optional XInput feature | Alternative if a Rust component has a concrete need; do not add a second input stack now |
| [ViGEmBus](https://github.com/nefarius/ViGEmBus) | Virtual Windows controller driver; upstream explicitly retired/archived | Historical/reference only; no automatic installation |
| [HidHide](https://github.com/nefarius/HidHide) | Windows device-hiding filter; metadata available, MIT | Optional troubleshooting reference for a physical+virtual duplicate-input case; not needed for ordinary SDL input |
| [Ryochan7/DS4Windows](https://github.com/Ryochan7/DS4Windows) | Original supplied URL returned GitHub API 404 and web 404 in this check | Historical lead; no replacement fork or download endorsed without separate review |
| [InputPlumber](https://github.com/ShadowBlip/InputPlumber) | Linux input routing/remapping daemon; metadata available, GPL-3.0 | Future Linux integration reference |
| [xpadneo](https://github.com/atar-axis/xpadneo) | Linux driver for Xbox Bluetooth devices; metadata available, GitHub license identification inconclusive | Future Linux/device-specific troubleshooting; read actual license before reuse |
| [Kernel uinput docs](https://docs.kernel.org/input/uinput.html) | Official userspace virtual-device examples; docs recommend considering libevdev for new code | Future virtual-device study, not a requirement to read normal pads |
| [GameMapperMind](https://github.com/NanoMindExplorer/GameMapperMind) | README describes Shizuku v13+, raw evdev trigger discovery and AIDL/shell touch-injection paths; metadata license identification inconclusive | Android touch-only third-party games; inspect actual license/code before reuse |
| [TouchMapper](https://github.com/Shyri/TouchMapper) | README describes gamepad-to-touch for Android TV without root, with a computer connected through ADB throughout use; GPLv3 | Android touch-mapping reference, not a dependency for our own game UI |
| [VirtualGamePad](https://github.com/kitswas/VirtualGamePad) | Metadata describes an Android phone as a PC gamepad, GPL-3.0; implementation/backends were not inspected | Remote gamepad idea only; ViGEm/uinput claims from the supplied list remain unverified here |
| [GFNxCloudController](https://github.com/ayazzxcs/GFNxCloudController) | README describes Android WebView/cloud-gaming touch overlay using a virtual Web Gamepad API approach; metadata has no detected license | UI/reference candidate only; not a native Android driver, no code import without license verification |

## Bounded next work

1. Get feedback on the current Meteos native launcher, restored frame and initial size.
2. Improve directional row/value navigation and evaluate the existing SDL3 dependency for launcher input. Keep one normalized event route to avoid duplicate actions.
3. Where exposed by the device, record name/GUID/VID/PID and controller type, show corresponding button labels, and support hotplug without stuck keys. Do not use VID/PID alone as proof of an exact model or mapping.
4. Record actual tests: GameSir already used by the owner in the game; Xbox/XInput, DualShock and DualSense still need individual checks, USB/Bluetooth separately. Include disconnect/reconnect, pause, Select settings, launch and exit.
5. If Android becomes an implementation target, first route physical input directly inside our app and implement its own touch controls. External touch injection/Shizuku/ADB are separate use cases. Do not require them for games we control.

Source checks: [SDL3 gamepads](https://wiki.libsdl.org/SDL3/CategoryGamepad), [SDL migration](https://github.com/libsdl-org/SDL/blob/main/docs/README-migration.md), [gilrs docs](https://docs.rs/gilrs/latest/gilrs/), [ViGEm retirement](https://docs.nefarius.at/projects/ViGEm/End-of-Life/), and linked repository metadata/READMEs. Pin revision/license before copying any code. Library coverage remains distinct from physical device evidence.
