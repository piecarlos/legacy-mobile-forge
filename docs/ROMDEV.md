# Optional romdev development bridge

Upstream: https://github.com/monteslu/romdev . Its current documented targets cover retro consoles/computers, not Java ME or Symbian. It is not a JAR-to-C++ backend for Meteos.

The Forge adapter is deliberately limited to local tool discovery and candidate-platform checks. Its profile is `engine/tools/romdev.json`. It does not install/start romdev, upload a game, run ROMs or become a dependency of the player launcher.

After explicitly choosing a supported console project, start a version-pinned upstream romdev server using its documentation. The upstream README currently requires Node 24+. Then inspect the local connection:

```powershell
python .\scripts\Romdev-Bridge.py --probe
python .\scripts\Romdev-Bridge.py --game 'C:\Games\own-example.nes' --probe
```

Only localhost HTTP endpoints are accepted. The request is a GET of `/openapi.json`; no local file bytes or paths are sent. Extension matching returns a candidate platform, not a claim that the ROM was validated. JAR/JAD/SIS/SISX are rejected before any request.

Next integration steps, only for a suitable console case: pin the server revision/license, exercise an original fixture, validate the documented load/input/frame APIs, and then add an explicit case-specific execution adapter. Potential benefits include deterministic input/frame comparison and flashing checks. Do not route J2ME or Symbian games to console cores.

The metadata bridge can be tested with a local mock OpenAPI server; that verifies the adapter contract, not the upstream emulator/recompiler. Actual romdev execution remains unvalidated in this project.
