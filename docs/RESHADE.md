# ReShade — experimento local, não padrão de todos os jogos

13/11/2026, Meteos, Windows x64, RX 6600M, Java 25 incluído no pacote.

| Rota | Resultado observado |
| --- | --- |
| Proxy DLL ao lado do executável/runtime, sem carga explícita | Jogo abriu; não foi encontrado log do ReShade. Não comprovou injeção. |
| OpenGL com carga explícita antes da inicialização Java2D | ReShade interceptou WGL; houve EXCEPTION_ACCESS_VIOLATION em atio6axx.dll ao encerrar. Não habilitar como padrão. |
| Direct3D 9 com carga explícita antes do Java2D | Log registrou runtime ReShade, driver AMD e encerramento limpo. |
| Direct3D 9 + LMF_Subtle.fx | Shader compilado com sucesso; processo terminou normalmente. A aprovação da aparência e a estabilidade em partida prolongada estão pendentes. |

O preset LMF_Subtle usa nitidez moderada, saturação 1.03 e contraste 1.02. Ele processa a imagem apresentada, inclusive moldura/menus; não substitui texturas nem adiciona detalhes originais.

## Executar o teste privado preparado

Na cópia local: experiments/reshade/d3d9/MeteosAstroBlocks/Testar-ReShade.cmd. O Meteos estável original foi preservado. Home abre a interface ReShade; Scroll Lock alterna os efeitos (conforme configuração do experimento).

ReShade_Setup_6.8.0.exe foi obtido do site oficial e a DLL x64 extraída do próprio arquivo, sem instalar componentes globais. A extração do proxy sozinha não bastou; ReshadeBootstrap.java carrega a DLL antes de iniciar o launcher. Não repetir cegamente em outros drivers/JDKs.

## Próximos testes

- Conferir comparação com efeito ligado/desligado durante partida e leitura das fontes.
- Validar Select/Start, telas de configuração, resize e tela cheia com ReShade.
- Avaliar custo e estabilidade em sessão prolongada.
- Só depois considerar opção experimental por jogo. Nunca substituir automaticamente a versão estável por este teste.

Não publicar DLL, instalador ou shaders baixados. O fonte próprio do preset fica em engine/reshade; o include ReShade.fxh e o ReShade devem ser obtidos pelo usuário no projeto oficial. Logs locais contêm caminhos da máquina e ficam fora do Git.

## Reutilizar em outro caso

engine/src/ReshadeBootstrap.java oferece uma entrada opcional e desacoplada. A base normal continua iniciando MobileGameDesktop. Numa cópia de teste, iniciar ReshadeBootstrap com `-Dforge.reshade.library=CAMINHO_ABSOLUTO_DA_DLL`, `-Dsun.java2d.d3d=true` e `--enable-native-access=ALL-UNNAMED` no Java recente. O classpath deve incluir o JAR do launcher e do FreeJ2ME. `forge.entryClass` permite usar outro launcher compatível, sem editar a classe bootstrap.

Tratar presença de log, criação de runtime, compilação do shader, comparação visual e encerramento como verificações distintas. Não basta o processo retornar código zero: o teste OpenGL teve falha nativa mesmo retornando zero neste ambiente.
