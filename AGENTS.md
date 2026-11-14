# Legacy Mobile Forge — instruções para o Work

## Objetivo

Receber o caminho de um jogo antigo e produzir uma adaptação Windows simples, controlável e verificável, reaproveitando esta base. Leia README.md, docs/STATUS.md, docs/NATIVE-PORTING.md e o project.json do jogo antes de agir. Não recomece a infraestrutura já existente.

Esta base NÃO é um recompilador universal. Java ME/J2ME usa bytecode e APIs móveis; SIS/SISX/Symbian nativo e N-Gage podem exigir outro runtime. Classifique pelo conteúdo antes de prometer execução.

## Fluxo obrigatório por jogo

1. Inspecionar o caminho fornecido sem modificar a entrada. Calcular hash e criar caso com scripts/New-GameProject.ps1, ou retomar o caso existente.
2. Usar Craft Anything na triagem: `craft-anything-cli --deep triage ARQUIVO`. Para JAR, usar também `--deep java-archive-show --path ARQUIVO`. Salvar evidências em games/ID/analysis. Consultar `commands` e os `--help` das ferramentas específicas antes de aprofundar.
3. Craft Anything é ferramenta central de análise, não um conversor automático. Usar java-class-show/java-method-show ou Vineflower quando necessário. Não lançar `chat`/`rebuild` ou outros agentes automaticamente. Não instalar Ghidra/SDKs grandes para uma simples inspeção de JAR.
4. Para J2ME: conferir MIDlet, CLDC/MIDP, dimensões, variantes, bibliotecas Nokia/Sony/Motorola e áudio. Escolher dimensões reais, testar o original, gerar profile.properties e compilar a base. JAD é descritor: localizar seu JAR associado.
5. Para SIS/SISX: inspecionar o pacote e pesquisar EKA2L1/compatibilidade. Não enviar binário Symbian para o backend J2ME. Separar necessidade de ambiente/firmware do trabalho do launcher; não obter material do sistema automaticamente.
6. Preservar menus de vídeo, Select, pausa/Start, saída para desktop, cursor contextual, controle e teclado. Regras/teclas específicas do jogo vão no perfil ou adaptador do caso.
7. Separar lógica do launcher, backend de execução, entrada normalizada e apresentação. Não supor que qualquer jogo responderá aos mesmos softkeys do Meteos.
8. Testar boot, entrada em partida, áudio, pausa, configurações, saída, saves, reconexão de controle e janela/tela cheia conforme a mudança. Não criar testes que apenas repetem a implementação, nem repetir verificações sem mudança pertinente.
9. Registrar fatos em docs/compatibility-matrix.csv ou no caso: observado, teste automatizado, hardware testado, não testado, falhou. GameSir usado no Meteos não valida automaticamente Xbox, PlayStation nem outro jogo.
10. Empacotar com jpackage/jlink e Java incluído somente após boot funcional. Verificar a partir de outra pasta e com dados isolados. Não sobrescrever releases que contenham progresso.

## Próximo jogo: investigação de C++

O usuário quer avaliar recompilação nativa no próximo jogo, preservando a rota Java como referência. Seguir docs/NATIVE-PORTING.md e preencher um plano por jogo com templates/GAME-PLAN.md em games/ID/PLAN.md. A entrada original e os resultados recuperados continuam privados.

- Craft Anything permanece obrigatório na triagem. Arlecchino é candidato para renomear membros e combinar decompiladores; Enigma é candidato para mapear/renomear o código. Nenhum dos dois é um compilador Java-para-C++.
- J2ME Recomp Universal é um candidato experimental para tradução bytecode/C++; não está integrado nem validado nesta base. Antes de executar, registrar revisão/licença, examinar os caminhos incompletos e provar uma fixture própria. Não substituir o backend atual automaticamente.
- Separar análise, recuperação de Java e tradução nativa. Compilar Java recuperado ou emitir C++ não prova equivalência nem um jogo jogável.
- Primeiro buscar uma partida mínima funcional; depois adaptar apresentação, menus, idiomas, controle e logs para a rota nativa. As classes Swing atuais não se tornam menus C++ automaticamente.
- Registrar bloqueadores concretos e uma lista curta de correções. Não entrar em ciclos de investigação/revisão, instalar SDKs grandes ou acionar outros agentes por conta própria. Se a rota nativa não avançar, entregar a adaptação Java funcional e deixar a experiência C++ separada.
- Usar romdev apenas como referência para futuros alvos de console documentados. Não encaminhar JARs J2ME ou SIS/SISX para essa ferramenta como se fossem ROMs compatíveis.

## Controles

A base inclui SDL3 + banco comunitário de mapeamentos e bridge próprio. SDL padroniza posições físicas de botões; validar o dispositivo real, USB/Bluetooth, hotplug e rótulos. Não tratar DualShock/DualSense como XInput nativo. Não instalar drivers ou remapeadores de sistema automaticamente. Backend Win32 anterior é fallback selecionável no perfil.

## Launcher e ferramentas opcionais

Seleção/importação de jogo no Windows deve usar o frontend nativo em C (`engine/native/GameLauncher.c`), com IFileOpenDialog, validação e logs antes da JVM. Não reintroduzir JFileChooser/JOptionPane no fluxo de importação. O backend Java do Meteos é headless nessa etapa; os menus internos do port permanecem separados. Leia docs/NATIVE-LAUNCHER.md para reaproveitar o perfil por jogo.

romdev possui uma ponte opcional de descoberta em scripts/Romdev-Bridge.py e um perfil em engine/tools/romdev.json. Leia docs/ROMDEV.md antes de usar. Ela consulta somente um servidor local iniciado pelo usuário, não instala dependências e rejeita J2ME/Symbian antes de qualquer requisição. Descoberta de API não equivale a integração de execução de ROMs.

## ReShade

Ler docs/RESHADE.md. Experimentos em experiments/ são privados e separados de qualquer release estável. Não publicar DLL/setup/shaders baixados do ReShade. Registrar a API realmente interceptada e compilação do efeito. Um DLL carregado não comprova melhoria visual. OpenGL apresentou falha no driver AMD neste ambiente; não habilitá-lo por padrão.

## Persistência e publicação

Jogos comerciais, saves pessoais, arquivos extraídos, credenciais, logs com caminhos privados e experimentos não entram no Git. Não enviar pacote privado que contenha o jogo. Usar `git status` e revisar a lista efetiva de arquivos antes de commit/push. Publicação no GitHub de piecarlos foi autorizada nesta sessão para a base limpa; em sessões futuras, respeitar o escopo pedido pelo usuário e não publicar automaticamente cada jogo.

## Comunicação

Agir com autonomia nas etapas autorizadas. Atualizações curtas, sem autoelogio, sem revisões recursivas ou investigação repetida. Dizer o que mudou, como foi testado e o que falta. Aprovação estética é do usuário. Não prometer recompilação nativa, compatibilidade universal ou suporte a controles sem evidência.
