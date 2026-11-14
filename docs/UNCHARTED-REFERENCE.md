# Uncharted launcher reference review

Reviewed on 2026-11-14 at upstream main revision `30f0daae2f88c9cf374493ca729ea86b6bb6b290` (release 0.8.5). Owner-supplied reference: [bc160201983/Uncharted-The-Nathan-Drake-Collection-PC-port](https://github.com/bc160201983/Uncharted-The-Nathan-Drake-Collection-PC-port). This is a source inspection, not a local build or gameplay validation. No upstream code, dependencies or executable was copied into the Forge implementation.

The Uncharted launcher is Python/Tkinter/Pillow; its small Windows C starter launches the bundled Python and waits for its exit. The repository's main C++ language label describes the wider game/runtime project, not the launcher UI. Forge currently implements its shell directly in C, so Python UI classes are references for adaptation, not drop-in C modules.

## Useful source areas

| Upstream file / symbol | Observed implementation | Relevance to Forge |
| --- | --- | --- |
| [launcher/uncharted_launcher.pyw](https://github.com/bc160201983/Uncharted-The-Nathan-Drake-Collection-PC-port/blob/30f0daae2f88c9cf374493ca729ea86b6bb6b290/launcher/uncharted_launcher.pyw), `Gamepads` around line 562 | SDL3 through ctypes; device open/close on hotplug; logical buttons/stick; detects PlayStation type for labels; slower discovery when disconnected | Highest priority: extend the native launcher's current XInput path using the SDL infrastructure already present in the generic game engine; test real devices before claiming compatibility |
| Same file, `pad_tick` around line 2573 | Focus-aware input; separate polling cadence while playing; game button presses must not activate launcher actions | Preserve focus/hidden-window isolation and reduce idle work. Forge already ignores its launcher input while the game runs |
| [launcher/ps5_ui.py](https://github.com/bc160201983/Uncharted-The-Nathan-Drake-Collection-PC-port/blob/30f0daae2f88c9cf374493ca729ea86b6bb6b290/launcher/ps5_ui.py), `FocusNav` around line 2163 | Separate activate/adjust actions, row navigation, optional row/column grid, focus highlighting and visibility | Improve the C shell's currently sequential button navigation: up/down changes setting row and left/right changes a value; keep mouse and keyboard behavior aligned |
| Same file, `Art`, `Animator` and `StickWatcher` | Cached artwork, animation timing, deliberate hold-to-close shortcut with launch grace period | Artwork caching and lightweight transitions are later possibilities. Do not add forced termination of a game/saves automatically |
| [launcher/updater.py](https://github.com/bc160201983/Uncharted-The-Nathan-Drake-Collection-PC-port/blob/30f0daae2f88c9cf374493ca729ea86b6bb6b290/launcher/updater.py), `latest_release`, `download`, `stage` | Release/version discovery, package size and SHA-256 checks, staging with path containment and required-file checks | Future optional launcher update flow. Adapt package names, trust model, data exclusion and failure recovery; do not enable network/update behavior just by documenting this reference |
| [tools/windows_starter/starter.c](https://github.com/bc160201983/Uncharted-The-Nathan-Drake-Collection-PC-port/blob/30f0daae2f88c9cf374493ca729ea86b6bb6b290/tools/windows_starter/starter.c) | Start bundled Python without console and wait for process lifetime | Pattern already served by Forge's own native Java bootstrap; replacing it gains little |

The updater's application/rollback implementation is in `tools/apply_update.py`; it was identified from the call site but not reviewed here. Do not treat rollback as locally tested or the entire updater as ready for reuse. Also inspect tests and the exact pinned file before implementing an update system.

## License / attribution

The upstream [README credits/license section](https://github.com/bc160201983/Uncharted-The-Nathan-Drake-Collection-PC-port/blob/30f0daae2f88c9cf374493ca729ea86b6bb6b290/README.md) declares GPL v2 or later, points to [gow3_pc/LICENSE](https://github.com/bc160201983/Uncharted-The-Nathan-Drake-Collection-PC-port/blob/30f0daae2f88c9cf374493ca729ea86b6bb6b290/gow3_pc/LICENSE), and requests attribution/link-back to BilalReloaded. GitHub's top-level license metadata is empty; that metadata alone does not override the README statement. Any copied/adapted code must retain notices, record origin/revision and changes, and be checked against its actual module/dependency license before distribution. No external code has been imported by this review.

Do not carry over Uncharted artwork, PS4 runtime fixes, FSR/DLSS binaries or game-specific settings as generic Java ME features. The PS4 route executes x86-64 game code with platform/GPU compatibility libraries. It does not translate J2ME bytecode or provide a Symbian backend.

## Other repositories from this author

The public repository listing also contains [gow3_pc](https://github.com/bc160201983/gow3_pc), [Matterfall-PC-port](https://github.com/bc160201983/Matterfall-PC-port), [medievil_2019_pc_port](https://github.com/bc160201983/medievil_2019_pc_port) and [Captain-America-Super-Soldier-PC](https://github.com/bc160201983/Captain-America-Super-Soldier-PC). Their metadata suggests additional packaging/runtime examples, with the last describing ReXGlue static recompilation for Xbox 360. These were catalogued from metadata only; their source, licenses and build behavior were not reviewed. None is evidence of a functioning J2ME native translator.

## Next bounded steps

1. First get the user's visual/gameplay feedback on the current C launcher and restored Meteos frame.
2. Adapt row/value navigation, then evaluate optional SDL3 input in the launcher using the existing pinned dependency. Keep XInput fallback and original labels where device type is unknown.
3. Test one actual Xbox/XInput device and one actual DualShock/DualSense over available connections; record what was observed.
4. Leave updater and animations for a separate change, with an original package fixture and explicit preservation of imported games/saves/settings. Blogger remains last.
