# Meteos Launcher 0.2.2 — Windows x64 preview

1. Extract the entire ZIP into a writable folder. Do not run it from inside the ZIP.
2. Open **MeteosLauncher.exe**. Java is included; no JDK or compiler is needed to play.
3. The **native C launcher** opens the Windows file picker. Select your original **Meteos Astro Blocks 1.0.4 / 240×320** JAR. It verifies and copies the file before starting Java. Later launches reuse that verified copy.

**The game is not included. No game download occurs.**

Required reference file: `meteos_astro_blocks_240x320_171711.jar`, **303,829 bytes**, SHA-256 `5756d421e3b878d124fb9c8453a6738ecb99ff6425679668ab2cb6e7b6df5021`. Renaming a different variant does not make it compatible.

This is a Java ME runtime adaptation, not native C++ recompilation. The first public binary uses the backend pinned in the repository and a neutral border. The illustrated development border and ReShade DLLs/shaders are not included. ReShade remains optional/experimental; see the repository instructions before using `Run-ReShade.cmd`.

Select/Back or F2 opens settings. D-pad/arrows navigate, left/right changes values, A/Enter confirms, B/E goes back; Start/Esc opens the original pause menu. F11 toggles borderless fullscreen. General changes the interface language (system/English/pt-BR) and opens logs. The original game text is unchanged.

Local data: `app/game/meteos.jar` (your imported game), `app/saves`, `app/video.properties`, `app/logs`. Import/bootstrap logs use `launcher.log`; gameplay logs use `session-*.log`. Back up saves/settings before replacing the folder. No automatic telemetry. Close the game and attach the relevant reviewed logs with steps/time to <https://github.com/piecarlos/legacy-mobile-forge/issues>. Do not attach the JAR or personal saves.

## Português

Extraia o ZIP inteiro e execute **MeteosLauncher.exe**. O importador é nativo em C e usa a janela de arquivos do Windows. Na primeira execução, selecione seu JAR original **1.0.4, 240×320**, com tamanho e hash acima. Ele valida e copia o jogo antes de iniciar Java; depois disso não precisa selecionar novamente. Java continua incluído para executar o jogo. Não é necessário instalar Python nem compilar nada para jogar.

O jogo não acompanha o ZIP e não é baixado automaticamente. Select/F2 abre configurações; a aba Geral seleciona o idioma e abre a pasta de logs. Preserve `app/saves` e `app/video.properties` ao atualizar. Esta é uma prévia para testes; compatibilidade com outros computadores e controles ainda precisa de relatos reais.
