# Estado do projeto

Base inicial de adaptação, não recompilador universal.

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
- Autenticação GitHub nesta sessão para envio ao destino solicitado. Não publicar binários e conteúdo comercial por acidente.

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
