# Legacy Mobile Forge

Base de trabalho para trazer jogos Java ME e investigar jogos Symbian no Windows, com menus, controles, filtros e empacotamento reaproveitáveis.

**Não é um recompilador universal.** O backend implementado nesta primeira versão executa JARs J2ME sobre FreeJ2ME-Plus. Symbian nativo é uma trilha de investigação separada.

## Para começar no Work

Abra esta pasta como projeto e peça:

> Leia AGENTS.md. Quero adaptar o jogo em C:\caminho\meu-jogo.jar. Identifique a versão com Craft Anything e reutilize o motor, os menus e os controles desta base. Preserve o arquivo original e trabalhe até obter um teste funcional.

Se existir um caso em games/, continue nele em vez de recriar.

## Ferramentas prontas

- `scripts/New-GameProject.ps1`: copia a entrada para um caso privado, calcula hash, classifica extensão e chama Craft Anything.
- `scripts/Build-GameProject.ps1`: compila o launcher genérico e pode gerar um app Windows portátil.
- `scripts/Setup-Dependencies.ps1`: prepara dependências verificadas; requer JDK 21+ e Windows x64 para empacotar.
- `engine/src`: apresentação, configurações via controle, Scale2x e execução J2ME.
- `engine/native/SdlPadBridge.cs`: SDL3 para entrada normalizada e conexão/desconexão.
- `docs`: estado, compatibilidade, referências e roteiro de publicação.

```powershell
.\scripts\Setup-Dependencies.ps1
.\scripts\New-GameProject.ps1 -GamePath 'C:\Jogos\MeuJogo.jar' -Id meu-jogo -Title 'Meu Jogo' -Width 240 -Height 320
.\scripts\Build-GameProject.ps1 -ProjectPath .\games\meu-jogo -Package
```

Craft Anything é detectado em Program Files; `-CraftPath` aceita outro local. Java pode ser indicado com `-JdkPath` na compilação.

## O que precisa ser validado em cada jogo

Dimensões, APIs do celular, teclas, som, gravação de progresso, pausa e game over. A base reutiliza infraestrutura, não garante que todo JAR rode sem ajustes. Profile.properties seleciona resolução lógica, título, backend e slot do controle.

Os modos de vídeo chegam a saída 4K, preservando a área lógica. Não inventam detalhes HD. Moldura customizada é opcional em assets/bezel.png do caso; há fallback neutro.

## Controles

SDL3 é o backend novo. Xbox/XInput e controles PlayStation usam a camada de mapeamento da SDL, mas testes físicos ainda devem ser registrados. Select/Back abre configurações; Start mantém o softkey de pausa padrão; A confirma, B volta e LB/RB alternam abas. Esses papéis podem exigir adaptação para o jogo.

## Conteúdo público e privado

O repositório contém código próprio e instruções. Jogos comerciais, saves, experimentos e builds privados são ignorados. A demonstração em fixtures/demo é código original deste projeto. Dependências baixadas são verificadas e mantidas fora do Git. Consulte THIRD-PARTY.md antes de distribuir binários.

Licença do código próprio: GPL-3.0-or-later. Sem afiliação com fabricantes ou titulares dos jogos.
