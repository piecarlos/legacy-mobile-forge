import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.time.*;
import java.time.format.DateTimeFormatter;
import java.util.*;

/** Local diagnostic log. No network, credentials, saves, or raw input events. */
final class SessionLog {
 static Path folder;
 static Writer writer;
 static long written;
 static final long MAX_BYTES=2*1024*1024;
 static boolean capped;
 static String root="";
 static synchronized void start(Path directory,File appRoot){
  if(writer!=null)return;
  folder=directory.toAbsolutePath();root=appRoot.getAbsolutePath();
  try{
   Files.createDirectories(folder);
   try(var files=Files.list(folder)){
    List<Path> old=files.filter(p->p.getFileName().toString().matches("session-.*\\.log")).sorted(Comparator.comparing(p->p.getFileName().toString())).toList();
    for(int i=0;i<old.size()-9;i++)Files.deleteIfExists(old.get(i));
   }
   String name="session-"+LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyyMMdd-HHmmss"))+"-"+ProcessHandle.current().pid()+".log";
   writer=Files.newBufferedWriter(folder.resolve(name),StandardCharsets.UTF_8,StandardOpenOption.CREATE_NEW);
   System.setOut(new PrintStream(new Tee(System.out,"stdout"),true,StandardCharsets.UTF_8));
   System.setErr(new PrintStream(new Tee(System.err,"stderr"),true,StandardCharsets.UTF_8));
   event("session.start","app="+System.getProperty("jpackage.app-version","development")+" java="+System.getProperty("java.version")+" os="+System.getProperty("os.name")+" arch="+System.getProperty("os.arch")+" reshade="+(System.getProperty("reshade.library")!=null||System.getProperty("forge.reshade.library")!=null));
   Runtime.getRuntime().addShutdownHook(new Thread(()->{event("session.end","shutdown");try{writer.close();}catch(IOException ignored){}},"local-log-close"));
  }catch(IOException ex){System.err.println("Local log unavailable: "+ex.getClass().getSimpleName());}
 }
 static String clean(String value){
  String home=System.getProperty("user.home","");
  for(String path:new String[]{root,home})if(!path.isEmpty()){value=value.replace(path,"<local>").replace(path.replace('\\','/'),"<local>");}
  return value;
 }
 static synchronized void event(String kind,String details){
  if(writer==null||capped)return;
  try{
   String line=Instant.now()+" "+kind+" "+clean(details)+System.lineSeparator();
   if(written+line.getBytes(StandardCharsets.UTF_8).length>MAX_BYTES){writer.write("Log size limit reached; further entries omitted.\n");writer.flush();capped=true;return;}
   writer.write(line);writer.flush();written+=line.getBytes(StandardCharsets.UTF_8).length;
  }catch(IOException ignored){}
 }
 static void openFolder(){try{if(folder!=null&&java.awt.Desktop.isDesktopSupported())java.awt.Desktop.getDesktop().open(folder.toFile());}catch(IOException|UnsupportedOperationException|SecurityException ex){event("logs.open.failed",ex.toString());javax.swing.JOptionPane.showMessageDialog(null,UiText.t("Não foi possível abrir a pasta de logs."));}}
 static final class Tee extends OutputStream{
  final PrintStream console;final String kind;final ByteArrayOutputStream line=new ByteArrayOutputStream();
  Tee(PrintStream console,String kind){this.console=console;this.kind=kind;}
  public synchronized void write(int b){console.write(b);if(b=='\n'){event(kind,line.toString(StandardCharsets.UTF_8).replace("\r",""));line.reset();}else if(line.size()<16384)line.write(b);}
  public synchronized void flush(){console.flush();}
 }
}
