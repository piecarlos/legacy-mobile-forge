# Publication

Public repository: https://github.com/piecarlos/legacy-mobile-forge

Initial testing preview: source and build tooling, with exact Meteos 1.0.4 / 240x320 JAR requirements. No commercial game, personal saves, extracted assets, private portable packages, ReShade DLLs or credentials are published.

Use installed local Git for source delivery: review the tracked files, commit, then push to origin/main. The owner explicitly requested that agents never use the GitHub connector Create Blob operation. Do not retry that operation or switch back to it when local network access fails.

On 2026-11-14 the Work shell could not resolve github.com, while PowerShell opened by the owner could access it and push successfully. This was a session access limitation; authentication/passkey failure was not established. If this recurs, prepare and verify the local commit, report the blocker, and provide the exact push command for the owner to run outside Work.

Before publishing: inspect the effective tracked tree, run the relevant small checks, and keep generated game builds private. See ports/meteos/README.md and THIRD-PARTY.md.
