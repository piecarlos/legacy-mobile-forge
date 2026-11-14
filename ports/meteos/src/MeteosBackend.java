import java.io.*;
import java.nio.file.*;
import java.security.*;
import java.util.*;

/** Headless entry after native import. Never opens a Java file picker. */
public final class MeteosBackend {
 static boolean valid(Path path)throws Exception {
  if(!Files.isRegularFile(path)||Files.size(path)!=303829)return false;
  MessageDigest digest=MessageDigest.getInstance("SHA-256");
  try(InputStream in=Files.newInputStream(path)){byte[] b=new byte[16384];int n;while((n=in.read(b))!=-1)digest.update(b,0,n);}
  return HexFormat.of().formatHex(digest.digest()).equals("5756d421e3b878d124fb9c8453a6738ecb99ff6425679668ab2cb6e7b6df5021");
 }
 public static void main(String[] args)throws Exception {
  SessionLog.start(new File(MeteosDesktop.dataRoot,"logs").toPath(),MeteosDesktop.appRoot);
  try{
   if(!valid(MeteosDesktop.assetFile("game/meteos.jar").toPath()))throw new IOException("No validated game. Start MeteosLauncher.exe to import your JAR.");
   if(Arrays.asList(args).contains("--validate")){SessionLog.event("game.validation","verified");return;}
   MeteosDesktop.main(args);
  }catch(Exception ex){ex.printStackTrace();System.exit(1);}
 }
}
