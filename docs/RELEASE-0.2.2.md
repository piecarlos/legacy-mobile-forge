# Meteos Launcher 0.2.2 — native Windows importer

Download **MeteosLauncher-0.2.2-preview-windows-x64.zip**, extract everything, then open **MeteosLauncher.exe**.

The import screen is now **native C**, using the Windows file picker. SHA-256 validation and copying happen before Java starts. No Python, compiler or separately installed Java is required to play. The game backend still uses Java ME/FreeJ2ME-Plus; this is not C++ recompilation of the game.

**The game is not included or downloaded.** Supply Meteos Astro Blocks **MIDlet 1.0.4, 240×320**, reference `meteos_astro_blocks_240x320_171711.jar`, **303,829 bytes**, SHA-256 `5756d421e3b878d124fb9c8453a6738ecb99ff6425679668ab2cb6e7b6df5021`. Later launches reuse your validated copy.

Windows-native error/import dialogs, system/English/pt-BR selection consistent with saved settings, Unicode file paths, local import logs and preserved saves/settings. Existing game settings, controls, Scale2x and fullscreen remain. Normal rendering is recommended; ReShade and illustrated artwork are not bundled.

For import issues, review and attach `app/logs/launcher.log`; for gameplay issues, attach the latest reviewed `session-*.log`. No automatic telemetry. Never upload the JAR or saves. Use a fresh extracted folder for testing and back up existing user data before updating.

The developer repository also includes an optional local romdev discovery bridge for documented console targets. It is not included as a player dependency, does not upload files and does not supply J2ME/Symbian support.

The separate FreeJ2ME source ZIP is the corresponding upstream runtime source, not the game. Launcher source is in the tag's Source code downloads; runtime legal notices remain in the package.

## Português

O seletor do jogo agora é **nativo em C**, com a janela de arquivos do Windows. Ele valida e copia seu JAR antes de iniciar Java. Extraia o ZIP inteiro, abra **MeteosLauncher.exe** e selecione o jogo na versão acima. Java já acompanha; Python e ferramentas de compilação não são necessários para jogar.

O jogo e o ReShade não acompanham o ZIP. Logs da importação ficam em `app/logs/launcher.log`; logs do jogo em `session-*.log`. A execução do jogo continua pelo backend Java; o novo importador não significa recompilação nativa do jogo.
