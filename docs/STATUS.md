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
