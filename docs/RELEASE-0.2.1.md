# Meteos Launcher 0.2.1 — testing preview

Download **MeteosLauncher-0.2.1-preview-windows-x64.zip**, extract it completely, and open **MeteosLauncher.exe**. Java is included. On the first launch, select your original game JAR; the launcher checks it and starts the game. You do not need to compile anything to play.

**The original game is not included or downloaded.** Required: **Meteos Astro Blocks MIDlet 1.0.4, 240×320**, reference filename `meteos_astro_blocks_240x320_171711.jar`, **303,829 bytes**, SHA-256 `5756d421e3b878d124fb9c8453a6738ecb99ff6425679668ab2cb6e7b6df5021`. Renaming a different JAR will not work.

English and pt-BR interface, system language by default; F2/Select settings, borderless fullscreen, sharp/smooth/Scale2x filters and local logs. Original game text is unchanged. This runs Java ME code through FreeJ2ME-Plus; it is not native C++ recompilation.

Normal rendering is recommended. ReShade binaries, third-party shaders and the illustrated development border are not bundled. The built-in border is neutral. Optional ReShade setup is documented in the repository; graphics-driver compatibility is still experimental.

Settings/saves/logs stay in `app/video.properties`, `app/saves`, `app/logs`. No automatic telemetry. Report bugs through Issues with a reviewed local log, steps and approximate time; do not upload your JAR or saves. Other computers/controllers still need real testing.

The separate `freej2me-plus-087fe289-source.zip` asset is the corresponding upstream runtime source snapshot, not the game. Launcher source is available through this tag's Source code downloads. Runtime legal notices are preserved; Java vendor/version is recorded in `runtime/release`.

## Português

Baixe **MeteosLauncher-0.2.1-preview-windows-x64.zip**, extraia tudo e execute **MeteosLauncher.exe**. Java já acompanha o pacote. Na primeira execução, selecione seu JAR original **1.0.4, 240×320**, com tamanho e hash acima. O jogo não acompanha o ZIP e não será baixado. Não é necessário compilar nada para jogar.

Versão normal recomendada. ReShade e moldura ilustrada não acompanham esta prévia pública. Abra Geral para escolher idioma ou acessar os logs. Preserve saves/configurações ao atualizar e envie relatos em Issues. Nenhuma telemetria automática.
