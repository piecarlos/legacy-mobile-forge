import java.awt.*;
import java.io.*;
import java.nio.file.*;
import java.security.*;
import java.util.*;
import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;

/** Public entry point: imports the user's exact JAR before starting the adapter. */
public final class MeteosLauncher {
 static final long GAME_SIZE=303829;
 static final String GAME_SHA256="5756d421e3b878d124fb9c8453a6738ecb99ff6425679668ab2cb6e7b6df5021";
 static final String GAME_NAME="meteos_astro_blocks_240x320_171711.jar";
 static Path gameFile(){return MeteosDesktop.assetFile("game/meteos.jar").toPath();}
 static boolean valid(Path path)throws IOException {
  if(!Files.isRegularFile(path)||Files.size(path)!=GAME_SIZE)return false;
  try{MessageDigest digest=MessageDigest.getInstance("SHA-256");try(InputStream in=Files.newInputStream(path)){byte[] b=new byte[16384];int n;while((n=in.read(b))!=-1)digest.update(b,0,n);}return HexFormat.of().formatHex(digest.digest()).equals(GAME_SHA256);}catch(NoSuchAlgorithmException ex){throw new IllegalStateException(ex);}
 }
 static void importGame(Path source)throws IOException {
  if(!valid(source))throw new IOException("Unsupported JAR. Requires Meteos Astro Blocks 1.0.4 / 240x320 / "+GAME_SIZE+" bytes / SHA-256 "+GAME_SHA256);
  Path target=gameFile().toAbsolutePath();
  if(valid(target)){SessionLog.event("game.import","already-valid");return;}
  Files.createDirectories(target.getParent());
  Path temp=Files.createTempFile(target.getParent(),"import-",".tmp");
  try{Files.copy(source,temp,StandardCopyOption.REPLACE_EXISTING);if(!valid(temp))throw new IOException("Copied JAR verification failed");Files.move(temp,target,StandardCopyOption.REPLACE_EXISTING);}finally{Files.deleteIfExists(temp);}
  SessionLog.event("game.import","verified variant=1.0.4 resolution=240x320 sha256="+GAME_SHA256);
 }
 static String tr(String english,String portuguese){return UiText.portuguese()?portuguese:english;}
 static boolean chooseGame()throws Exception {
  final boolean[] imported={false};
  SwingUtilities.invokeAndWait(()->{
   while(!imported[0]){
    String message=tr("Supply your original Meteos Astro Blocks JAR to start.\nThe game is not included and nothing is downloaded.\n\nRequired: version 1.0.4, 240x320, 303,829 bytes.\nReference filename: ","Forneça seu JAR original do Meteos Astro Blocks para iniciar.\nO jogo não acompanha o launcher e nada será baixado.\n\nNecessário: versão 1.0.4, 240x320, 303.829 bytes.\nNome de referência: ")+GAME_NAME;
    Object[] choices={tr("Select game JAR","Selecionar JAR do jogo"),tr("Exit","Sair")};
    if(JOptionPane.showOptionDialog(null,message,"Meteos PC",JOptionPane.DEFAULT_OPTION,JOptionPane.INFORMATION_MESSAGE,null,choices,choices[0])!=0)return;
    JFileChooser picker=new JFileChooser();picker.setDialogTitle(tr("Select the original game JAR","Selecione o JAR original do jogo"));picker.setFileFilter(new FileNameExtensionFilter("Java ME (*.jar)","jar"));picker.setAcceptAllFileFilterUsed(false);
    if(picker.showOpenDialog(null)!=JFileChooser.APPROVE_OPTION)continue;
    try{importGame(picker.getSelectedFile().toPath());imported[0]=true;}
    catch(IOException ex){SessionLog.event("game.import.rejected",ex.getClass().getSimpleName());JOptionPane.showMessageDialog(null,tr("This file is not the supported game variant, or could not be copied.\nUse version 1.0.4, 240x320, 303,829 bytes.\nA different filename or a renamed JAR is not sufficient.\nCheck that this extracted folder is writable.","Este arquivo não é a variante compatível ou não pôde ser copiado.\nUse a versão 1.0.4, 240x320, com 303.829 bytes.\nRenomear outro JAR não resolve.\nConfira se esta pasta extraída permite gravação."),"Meteos PC",JOptionPane.ERROR_MESSAGE);}
   }
  });return imported[0];
 }
 public static void main(String[] args)throws Exception {
  SessionLog.start(new File(MeteosDesktop.dataRoot,"logs").toPath(),MeteosDesktop.appRoot);
  // Load the same saved language as the port before showing the import dialog.
  new VideoSettings(new File(MeteosDesktop.dataRoot,"video.properties").toPath());
  ArrayList<String> forwarded=new ArrayList<>();
  try{
   for(int i=0;i<args.length;i++){if(args[i].equals("--import")){if(++i>=args.length)throw new IOException("--import requires a JAR path");importGame(Path.of(args[i]));}else forwarded.add(args[i]);}
   if(forwarded.contains("--validate")||forwarded.contains("--import-only")){
    if(!valid(gameFile()))throw new IOException("No validated game installed");
    SessionLog.event("game.validation","verified");System.out.println("PASS: installed game JAR verified");System.exit(0);return;
   }
   if(!valid(gameFile())){
    if(forwarded.contains("--smoke"))throw new IOException("No validated game installed; import a user JAR first");
    if(!chooseGame()){SessionLog.event("game.import","cancelled");System.exit(0);return;}
   }
   MeteosDesktop.main(forwarded.toArray(String[]::new));
  }catch(Exception ex){ex.printStackTrace();if(args.length==0&&!GraphicsEnvironment.isHeadless())JOptionPane.showMessageDialog(null,tr("Could not start. Check the local logs folder.","Não foi possível iniciar. Consulte a pasta local de logs."),"Meteos PC",JOptionPane.ERROR_MESSAGE);System.exit(1);}
 }
}
