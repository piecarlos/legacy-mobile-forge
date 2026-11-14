# Referências técnicas

- Craft Anything: instalação local e ajuda do CLI; usar triage, java-archive-show e ferramentas de classe/método. Integração executada no Meteos nesta sessão.
- FreeJ2ME-Plus: https://github.com/TASEmulators/freej2me-plus — runtime J2ME, não recompilador universal.
- SDL3: https://github.com/libsdl-org/SDL — entrada padronizada, hotplug e drivers de gamepads.
- SDL gamepad: https://wiki.libsdl.org/SDL3/CategoryGamepad
- Banco de mapeamentos: https://github.com/mdqinc/SDL_GameControllerDB
- Jamepad: https://github.com/williamahartman/Jamepad — alternativa Java/SDL investigada; esta base usa um bridge SDL3 separado, não Jamepad.
- EKA2L1: https://github.com/EKA2L1/EKA2L1 — emulação Symbian/N-Gage, não substituto direto do backend J2ME.
- ReShade: https://reshade.me/ e https://github.com/crosire/reshade

Versões e hashes locais estão em dependencies.lock.json. Não transformar cobertura declarada por uma biblioteca em afirmação de teste físico deste projeto.

## J2ME source recovery and native candidates

- Arlecchino: https://github.com/wehrwolfmann/arlecchino — candidate for member renaming, multiple decompilers and source recovery. Documentation reviewed; not locally installed or validated.
- Enigma: https://github.com/FabricMC/Enigma — Java mappings/deobfuscation tool, not a C++ translator. Documentation reviewed; not locally integrated.
- J2ME Recomp Universal: https://github.com/denisfox3554/J2me-recomp-universial — repository contains an experimental bytecode/C++ pipeline and native runtime. No local compiler build or complete game validation has been performed here. Do not treat the README as proof of universal compatibility.
- The Blink J2ME Game Recompiler page is a promotional reference, not a verified toolchain dependency.

Use docs/NATIVE-PORTING.md and templates/GAME-PLAN.md. Pin actual revisions and licenses before experiments; preserve the original JAR and working Java reference. Source recovery can be skipped when the chosen native tool operates on original bytecode and decompilation would not help.

## romdev — candidate for future console projects

- Repository: https://github.com/monteslu/romdev
- Reviewed README on 2026-11-13. Documents an MCP/HTTP tool server for console ROM builds, emulation, frame inspection, reverse engineering and selected static recompilation targets. Node 24+; toolchains and emulator cores are delivered through npm/WebAssembly.
- Useful candidates: scripted input and screenshots, frame-by-frame differential checks, flashing detection, and tooling for future console projects.
- The documented systems do not include Java ME/J2ME or Symbian. It is not a drop-in JAR-to-C++ converter and does not replace Craft Anything or FreeJ2ME in the current Meteos adapter.
- Optional discovery adapter: scripts/Romdev-Bridge.py, profile engine/tools/romdev.json, instructions docs/ROMDEV.md. The local mock OpenAPI contract and unsupported-target rejection are tested. No server is installed/running in the tested environment, and actual upstream execution/emulation has not been validated. No game data is sent. Evaluate a small original fixture on a documented target before adding an execution adapter.
