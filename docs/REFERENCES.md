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

## romdev — candidate for future console projects

- Repository: https://github.com/monteslu/romdev
- Reviewed README on 2026-11-13. Documents an MCP/HTTP tool server for console ROM builds, emulation, frame inspection, reverse engineering and selected static recompilation targets. Node 24+; toolchains and emulator cores are delivered through npm/WebAssembly.
- Useful candidates: scripted input and screenshots, frame-by-frame differential checks, flashing detection, and tooling for future console projects.
- The documented systems do not include Java ME/J2ME or Symbian. It is not a drop-in JAR-to-C++ converter and does not replace Craft Anything or FreeJ2ME in the current Meteos adapter.
- Keep as a research reference. No dependency installed or integrated, and no local functionality/compatibility has been validated. Evaluate a small original fixture on a documented target before adding it to a future workflow.
