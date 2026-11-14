# Estado do projeto

Base de adaptação; launcher 0.2.2-preview.1 publicado com importador nativo C. Não é um recompilador universal. Próximo trabalho: novo jogo com referência Java e investigação nativa separada; ler NATIVE-PORTING.md.

## 0.2.3 - launcher visual padrão (prévia local, 14/11/2026)

O usuário forneceu uma referência de launcher com arte, abas e botão Jogar e pediu esse padrão para o Forge. Implementado em engine/native/LauncherUi.h, em C/GDI/WIC: Início, Imagem, Controles, Sistema, importação pelo seletor Windows, idioma Windows/English/pt-BR, filtros, resolução, janela/tela cheia, escala inteira e moldura. Java só inicia por Jogar. O shell oculta durante a execução e retorna ao encerrar o processo filho; fechá-lo sai para o desktop. Sem opções 3D sem implementação.

A arte fornecida pelo usuário foi restaurada em ports/meteos/assets/bezel.png, com origem/hash documentados; integra a moldura e o cabeçalho do launcher. Preferências novas usam Full HD em janela, limitada ao monitor. Preferências existentes prevalecem. Build-GameProject -Package também usa o shell, com INI por caso; seus pacotes continuam privados e incluem o jogo. A rota pública Meteos continua sem JAR.

Verificações locais: C17 compilou; ZIP auditado sem jogo/saves/logs/DLL ReShade, com hash da moldura; importação válida, caminhos Unicode, cache e handoff Java passaram. Verify-LauncherUi.py exercitou a janela real, quatro abas, teclado/mouse, todas as opções de imagem, idioma, persistência e preservação de propriedades extras. Capturas Windows PrintWindow foram inspecionadas. Uma fixture Java original comprovou Jogar/ocultar/retornar sem representar teste de gameplay. A demonstração Forge foi empacotada pelo novo script e passou o mesmo teste de interface. Verificação Java offscreen confirmou Full HD inicial, viewport inteiro de 720x960 numa área 1920x1000 e carregamento efetivo da arte.

O shell inclui polling XInput; o controle físico e a partida prolongada com essa nova tela ainda precisam do usuário. A versão pública no GitHub continua 0.2.2; 0.2.3 está preparada localmente para avaliação visual. O backend continua Java, sem recompilação C++ do jogo. Blogger permanece por último.

## Implementado

- Núcleo J2ME reaproveitado do launcher Meteos, generalizado por perfil: título, dimensões, backend e slot do controle.
- Menus de vídeo, janela/tela cheia, moldura opcional, três filtros, cursor contextual e saída por término do MIDlet.
- Scripts de criação de caso com Craft Anything, compilação e empacotamento.
- SDL3 e banco de mapeamentos, com bridge próprio para botões lógicos e hotplug.
- Demonstração original sem jogo comercial para testar a infraestrutura.
- ReShade experimental no Meteos por Direct3D; preset compilado. OpenGL falhou no driver AMD ao encerrar.

## Pendências

- Manter registro de novos jogos e resultados conforme forem adaptados.
- Testes físicos com controles Xbox, DualShock, DualSense e conexões USB/Bluetooth. A enumeração SDL funcionou, mas não havia gamepad conectado no teste.
- Remapeamento de ações por jogo, wizard de botões e backend Symbian executável.
- Migração dos jogos existentes para esta base somente após teste comparativo. O Meteos portátil estável não foi substituído.
- ReShade precisa de avaliação visual, sessões prolongadas e teste de troca de tela/menu.
- Ampliar a investigação nativa no próximo jogo, com versão fixada das ferramentas e uma fixture própria antes do jogo comercial. Arlecchino/Enigma e o tradutor C++ ainda não estão integrados.

Consulte compatibility-matrix.csv para distinguir possibilidade, teste automatizado e hardware efetivamente validado.

## Verificação inicial executada

- Setup de dependências validou hashes locais e compilou o bridge.
- Scripts de demo e de build executados também no Windows PowerShell 5.1.
- Demonstração própria compilada e executada, menus/cursor e saída verificados por smoke test.
- Script de empacotamento gerou executável com Java incluído; executável abriu de outra pasta e encerrou pelo evento de término do MIDlet.
- SDL3 inicializou sem erros e enumerou zero gamepads conectados; helper permaneceu ativo. Isso não valida hardware Xbox/PlayStation.
- ReShade Direct3D carregou e compilou LMF_Subtle.fx; OpenGL apresentou falha no driver ao encerrar.

## Correção 0.1.1 — retorno das configurações

A troca janela/tela cheia mantém o mesmo peer nativo, evitando recriar o alvo gráfico do ReShade. Redesenho e foco são solicitados ao fechar o menu; buffers dinâmicos do jogo não usam cache acelerado de imagem. Testes de geometria, preferências, menu e ciclos de tela executados; o reteste visual no hardware do usuário permanece necessário.

## 0.1.2 — presentation, language and local test logs

English and pt-BR launcher UI; system locale by default, English fallback, manual selection under General. Original MIDlet text is unchanged. Local session logs capture lifecycle/settings/errors/status, retain ten sessions up to 2 MiB each, redact known user/app paths and make no network requests. General opens the log folder. No automatic GitHub submission. Double buffering restored; complete repaint enabled only for explicit ReShade experiments. Compile check passed for the generic engine; localized menu/persistence/log checks passed on Meteos. Extended physical and flicker testing remains pending. Blogger is last, after stability, clean publication and verified recompiler research.

## 0.2.0-preview.1 — public source preview

Meteos adapter separated under ports/meteos. Build script verifies MIDlet 1.0.4 / 240x320 JAR size and SHA-256 before creating output, runs Craft Anything, verifies the pinned backend and builds a portable copy. Wrong fixture JAR rejected before output creation. Full build passed on Windows PowerShell 5.1; copied portable package boot/settings/cursor/helper/exit/log smoke passed from another directory. Public source build uses the locked newer backend; it is distinct from the earlier private 1.0.2 packages. GameSir and ReShade extended feedback still comes from user testing. Reports use local logs and a bilingual GitHub issue form.

## 0.2.1-preview.1 — game-free public launcher

Published: https://github.com/piecarlos/legacy-mobile-forge/releases/tag/v0.2.1-preview.1 . Windows x64 ZIP includes Java and asks for the user's original JAR on first launch. Requires Meteos MIDlet 1.0.4 / 240x320, 303829 bytes and the documented SHA-256. It retains the verified local copy for later launches. No original game, saves, logs, illustrated border or ReShade DLL is shipped.

GitHub Actions run 43553190268 completed build, archive audit and prerelease publication successfully. The actual published ZIP was downloaded and verified against SHA-256 `a444bb52adae9c99bdc3f13463fe347c1c9a2042047a74061ca1ccbd37ba1904`. Fresh extraction, missing/wrong JAR rejection, correct import, cached validation, preservation of a valid game after an invalid replacement attempt, and local logs passed against that downloaded artifact. The CI package uses Temurin Java 21; a separate FreeJ2ME corresponding-source archive is attached to the release.

These import checks do not replace extended gameplay/controller testing. Local full-play smoke in the current restricted tool environment was blocked by Windows registry access needed by Java's synthesizer; no product workaround was introduced for that sandbox restriction. Earlier adapter boot/menu/exit and user gameplay evidence remain documented above.

## Next-game preparation — native route

AGENTS.md, NATIVE-PORTING.md and templates/GAME-PLAN.md now define a staged investigation: Craft triage, original Java reference, optional source recovery with Arlecchino/Enigma, an original fixture for a pinned native translator, then one playable game slice. Menus/controllers/languages/logs are integrated into the native route only after that slice works. This is workflow preparation, not an installed or functioning C++ recompilation backend. The new game's Windows path is still needed to start a case. romdev is recorded for future supported console projects; Blogger remains last.

## 0.2.2-preview.1 — native importer and optional discovery bridge

Importer/bootstrap moved to engine/native/GameLauncher.c: Windows TaskDialog/IFileOpenDialog, native SHA-256 validation and safe copy, per-game INI profile, saved/system language and local rotating launcher logs. The Java backend has no file chooser and starts only after native validation. No Python/CLR dependency is added to the player package. Game execution remains FreeJ2ME/Java.

Local checks passed for archive contents, invalid/correct JAR import, Unicode/space-containing source paths, valid-cache preservation and IFileOpenDialog initialization. Import/validation ran with javaw renamed/unavailable. The probe configures the native dialog object without displaying it; visual appearance needs desktop review. The optional romdev adapter passed its mock-server discovery/local-endpoint/rejection checks. The real localhost probe found no server running. No romdev runtime or game execution was integrated.

Published: https://github.com/piecarlos/legacy-mobile-forge/releases/tag/v0.2.2-preview.1 . CI run 43596454226 passed native C compilation, archive audit, romdev bridge contract, native Windows dialog API probe and prerelease publication. Native-to-Java handoff and backend JAR validation also passed locally; this does not replace full gameplay/controller testing.
