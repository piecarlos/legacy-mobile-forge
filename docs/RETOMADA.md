# Ponto de retomada - 2026-11-14

## Estado preservado

- Repositorio: https://github.com/piecarlos/legacy-mobile-forge
- Previa publicada: https://github.com/piecarlos/legacy-mobile-forge/releases/tag/v0.2.3-preview.1
- Commit da release: 4a864f254369e0b0c8608d062797ca0cd6e3bd2a.
- Actions: https://github.com/piecarlos/legacy-mobile-forge/actions/runs/43612766110
- Pacote: MeteosLauncher-0.2.3-preview-windows-x64.zip; SHA-256 publicado: 3272eb5c27f96459f98053adb0690f219e6fde5c56b4dd5fb4875670bdadad0d.
- O ZIP publicado foi recompilado pelo Actions. Seu hash difere do ZIP local anterior; nao sao o mesmo arquivo.

## O que existe

Launcher Windows nativo C com arte, quatro abas e botao Jogar; Java incluido para executar o backend FreeJ2ME-Plus; verificacao/importacao do JAR original; moldura fornecida pelo usuario; Full HD inicial limitado ao desktop; preferencias existentes preservadas. O pacote publico nao inclui jogo, saves, logs ou DLL ReShade.

O launcher nativo nao equivale a recompilacao do jogo. A rota funcional atual executa Java ME. Traducao do jogo para C++ continua investigacao separada, sem jogo nativo validado. Craft Anything permanece a ferramenta de triagem; Arlecchino e Enigma sao auxiliares candidatos, nao compiladores Java-para-C++.

## Evidencias e pendencias

A verificacao local do ZIP passou: integridade, executavel x64 nativo sem CLR, hash da moldura e ausencia de jogo/saves/logs/ReShade. No Actions passaram compilacao, contrato da ponte romdev, auditoria, API do seletor Windows e navegacao/preferencias do launcher; a release foi publicada com ZIP, fontes do runtime e SHA256SUMS.txt.

Pendente: partida prolongada e controle fisico com o novo shell. Nao declarar Xbox/PlayStation ou outros jogos validados apenas por testes automatizados. Preservar os pacotes anteriores e todo progresso pessoal. Blogger fica para depois.

## Como retomar

1. Ler AGENTS.md, README.md, docs/STATUS.md e docs/NATIVE-PORTING.md.
2. Conferir git status e retomar o caso existente; para outro jogo, receber o caminho e seguir o plano por jogo sem reconstruir a infraestrutura.
3. Manter originais, saves, logs privados, credenciais e builds com jogo fora do Git.
4. Registrar decisoes, resultados e bloqueadores concretos nestes documentos ao terminar cada etapa.

## Publicacao e continuidade

Nunca usar a operacao Create Blob do conector GitHub, por instrucao expressa do usuario. Usar Git instalado. Nesta sessao o shell Work falhou no DNS de github.com; o PowerShell externo do usuario fez o push com sucesso. Se isso se repetir, entregar o commit revisado e o comando de push, sem afirmar publicacao antes de confirma-la.

Esta documentacao e um resumo operacional, nao uma transcricao integral. O anexo conv chat gpt recomp.txt contem apenas o inicio da conversa: proposta de adaptar Meteos, identificacao Java ME 1.0.4 e uso de Craft Anything. As etapas posteriores estao registradas no repositorio. Nao publicar a conversa privada bruta automaticamente.
