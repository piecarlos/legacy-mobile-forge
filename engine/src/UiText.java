import java.util.*;

/** Launcher text only: the original MIDlet keeps its own language. */
final class UiText {
 static String selection="auto";
 static final Map<String,String> english=new HashMap<>();
 static {
  String[][] pairs={
   {"VÍDEO","VIDEO"},{"CONTROLE","CONTROLS"},{"GERAL","GENERAL"},{"CONFIGURAÇÕES","SETTINGS"},
   {"Vídeo","Video"},{"Configurações.  F2","Settings...  F2"},{"Janela / tela cheia  F11","Window / fullscreen  F11"},
   {"Sair para o desktop?","Quit to desktop?"},{"A fase atual será encerrada.","The current stage will end."},
   {"Continuar jogando","Resume game"},{"Sair para o desktop","Quit to desktop"},
   {"Resolução em janela","Window resolution"},{"Modo de exibição","Display mode"},{"Escala do jogo","Game scale"},
   {"Filtro de imagem","Image filter"},{"Moldura espacial","Space border"},{"Aplicar e continuar","Apply and resume"},
   {"Voltar sem aplicar","Cancel and resume"},{"Tela cheia sem bordas","Borderless fullscreen"},{"Janela","Window"},
   {"Pixels uniformes","Integer scaling"},{"Ajustar à altura","Fit to height"},{"Suave","Smooth"},{"Pixels nítidos","Sharp pixels"},
   {"Ativada","On"},{"Desativada","Off"},{"Compacta — 960 × 540","Compact — 960 × 540"},
   {"Tela cheia usa a resolução do desktop. Janelas grandes se ajustam ao monitor.","Fullscreen uses the desktop resolution. Large windows fit the monitor."},
   {"Direcional     Mover / navegar","D-pad     Move / navigate"},{"A     Ação / confirmar","A     Action / confirm"},
   {"B     Voltar / fechar configurações","B     Back / close settings"},{"Start     Pausa do jogo","Start     Game pause"},
   {"Select     Abrir / fechar configurações","Select     Open / close settings"},{"LB / RB     Trocar aba","LB / RB     Switch tab"},
   {"↑ ↓  Navegar    ← →  Alterar    A  Confirmar    B / Select  Voltar","↑ ↓  Navigate    ← →  Change    A  Confirm    B / Select  Back"},
   {"Idioma da interface","Interface language"},{"Automático (sistema)","Automatic (system)"},{"Abrir pasta de logs","Open log folder"},
   {"Logs locais de teste. Nenhum dado é enviado automaticamente.","Local test logs. No data is sent automatically."},
   {"O idioma altera apenas a interface do port, não o jogo original.","Language affects the port interface, not the original game."},
   {"Não foi possível abrir a pasta de logs.","Could not open the log folder."},
   {"Não foi possível ler as preferências; usando valores disponíveis.","Could not read preferences; using available values."},
   {"Não foi possível salvar as preferências: ","Could not save preferences: "}
  };
  for(String[] p:pairs)english.put(p[0],p[1]);
 }
 static void select(String value){selection=Set.of("auto","en","pt-BR").contains(value)?value:"auto";}
 static boolean portuguese(){return selection.equals("pt-BR")||selection.equals("auto")&&Locale.getDefault(Locale.Category.DISPLAY).getLanguage().equals("pt");}
 static String t(String source){return portuguese()?source:english.getOrDefault(source,source);}
 static String[] all(String... source){return Arrays.stream(source).map(UiText::t).toArray(String[]::new);}
 static String languageName(String choice){return choice.equals("auto")?t("Automático (sistema)"):choice.equals("pt-BR")?"Português (Brasil)":"English";}
 static String next(String choice,int direction){String[] values={"auto","en","pt-BR"};return values[Math.floorMod(Arrays.asList(values).indexOf(choice)+direction,3)];}
}
