# Next game: native recompilation investigation

The current executable runs Java ME through FreeJ2ME-Plus. No native C++ game has been produced or validated by this project. The next game should evaluate that route alongside a working Java reference, with its original files preserved.

## Tool roles

| Tool | Intended role | Status here |
| --- | --- | --- |
| Craft Anything | Identify archive/platform, inspect classes/methods and recover game-specific information | Used on Meteos; keep in the first triage step |
| FreeJ2ME-Plus | Run the original JAR as a reference and provide the existing Java adaptation route | Implemented backend; each game still needs validation |
| Arlecchino | Candidate member renaming and multi-decompiler recovery of compilable Java | Documentation reviewed; not installed/integrated/tested locally |
| Enigma | Candidate manual mappings, class/member renaming and readable code navigation | Documentation reviewed; not installed/integrated/tested locally |
| J2ME Recomp Universal | Experimental bytecode-to-C++ translator and its own native runtime | Repository/code inspected; no local build or game validation yet |
| romdev | Candidate console tooling, scripted input, frame comparison and flashing checks | Optional metadata bridge; mock contract tested, execution unvalidated; no documented J2ME/Symbian target |

Sources: [Arlecchino](https://github.com/wehrwolfmann/arlecchino), [Enigma](https://github.com/FabricMC/Enigma), [J2ME Recomp Universal](https://github.com/denisfox3554/J2me-recomp-universial), [romdev](https://github.com/monteslu/romdev). Pin the actual revision and check its instructions/license before use. The names alone do not establish maturity or compatibility.

Arlecchino/Enigma help understand or recover Java; they do not directly turn it into C++. A native translator consumes bytecode or another defined input and still needs implementations of the game's Java ME APIs, graphics, sound, threading and saves. Do not promise a universal conversion pipeline.

## 1. Create or resume the private case

Use `scripts/New-GameProject.ps1`, then copy `templates/GAME-PLAN.md` to `games/ID/PLAN.md`. Read the generated project.json and case instructions. Do not create a duplicate case or overwrite saves. If a new session only has a Windows path, this plan and AGENTS.md should be enough to resume the work.

Record SHA-256, size, MIDlet version/class, handset variant, logical resolution, CLDC/MIDP, vendor APIs, resources, audio formats and RMS/save behavior. Default script dimensions are placeholders, not detected facts. Do not apply the Meteos-specific hash or key mapping to another game. JAD needs its associated JAR; native Symbian is a different route.

## 2. Establish the reference

Run the original using the existing Java backend in an isolated data folder. Capture boot, a short playable sequence, pause/settings and exit. Record timing/input steps and preserve representative frames and saves privately. Register hardware actually tested, and distinguish gamepad helper startup from a physical controller test.

Use this reference to judge later changes. A native experiment must not destroy the playable Java case or change its saved progress.

## 3. Recover Java only where useful

Inspect the problematic classes with Craft Anything. If source-level names/collisions block progress, trial Arlecchino on a copy; retain renaming maps, version, command, report and failures. Use Enigma if human-readable mappings/manual investigation will help. Do not install both just to tick a checklist.

Compile recovered sources against the required APIs where possible, and compare behavior with the original before changing rules or resources. Fewer compiler errors are useful evidence, not proof of identical behavior. Keep decompiled commercial code, assets and rebuilt game JARs outside public Git.

Java recovery is optional for a bytecode translator that does not require it. Avoid unnecessary decompile/recompile steps that add semantic changes.

## 4. Prove the native toolchain separately

Before feeding it the commercial game:

1. Inspect and pin the translator/runtime, license and compiler dependencies. Reuse installed tools where possible.
2. Build an original small fixture, such as this repository's demo or a smaller compatible fixture.
3. Check unsupported opcodes/APIs, virtual calls, object lifetime, exceptions, timers and threading that the game actually uses.
4. Produce an executable and demonstrate behavior. Generated C++ files or a successful compiler exit are intermediate milestones.

Keep the experiment under `games/ID/native/` or another ignored private build directory. Record a small blocker list and next experiment; do not repeatedly rebuild an unchanged broken toolchain. Ask for missing game-specific information only after completing independent work.

## 5. Try one playable slice

Target boot, title/menu and one short playable stage first. Reproduce the reference inputs and compare progression, graphics, speed, audio and save behavior. Record any stubs or approximations explicitly. Do not hide unimplemented functions behind a claim of compatibility.

Only after that slice works, bring across the port's UI behavior: Select settings, Start pause, desktop exit, cursor, English/pt-BR, display modes/Scale2x, controls and local logs. Swing components cannot be linked directly into a JVM-free C++ application; these features need a native implementation or a separately designed frontend. Existing profiles and controller mappings can inform that work.

Inspect what the executable actually runs. Distinguish translated native game logic from a C++ interpreter or a launcher still invoking Java. Native does not mean dependency-free. Compare performance under equivalent game speed, resolution and settings; C++ alone does not establish a speedup.

## Completion and fallback

- **Java adaptation:** original JAR runs on the bundled runtime; label it accurately.
- **Native proof:** a documented fixture or partial game slice works; list limits and keep it experimental.
- **Native game preview:** gameplay, UI, audio, saves and exit pass the scoped checks; package with the user's JAR import and exact variant verification, without commercial game data.
- **Blocked:** name the unsupported feature/toolchain problem, preserve the evidence and working Java route, and propose a bounded next step.

Keep logs local and reviewed before sharing; no automatic telemetry. Public releases require source/license notices, a game-free archive audit and a fresh-folder import test. ReShade and Blogger remain separate optional work.

## Português — como pedir o próximo teste

Abra `C:\workspace\LegacyMobileForge` no Work e informe:

> Leia AGENTS.md, docs/STATUS.md e docs/NATIVE-PORTING.md. O próximo jogo está em C:\Games\meu-jogo.jar. Faça a triagem com Craft Anything, preserve o original, crie/retome o caso e avalie C++ com uma referência Java funcional. Use Arlecchino/Enigma apenas se ajudarem e registre os bloqueadores reais. Reaproveite menus, controles, idiomas e logs quando a rota escolhida estiver funcional.
