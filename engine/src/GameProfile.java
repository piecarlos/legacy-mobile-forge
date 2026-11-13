import java.io.*;
import java.util.Properties;
final class GameProfile {
 static final Properties values=new Properties();
 static {File file=MobileGameDesktop.assetFile("profile.properties");try(Reader in=new StringReader(java.nio.file.Files.readString(file.toPath()).replace("\uFEFF",""))){values.load(in);}catch(IOException ex){System.err.println("Profile unavailable: "+ex.getMessage());}}
 static final String title=values.getProperty("title","Legacy Mobile Game");
 static final String shortTitle=title.length()>25?title.substring(0,22)+"...":title;
 static final int width=dimension("width",240),height=dimension("height",320);
 static final int slot=number("controller.slot",0,0,15);
 static final boolean useSdl=!values.getProperty("controller.backend","sdl").equals("win32");
 static int dimension(String key,int fallback){return number(key,fallback,96,2048);}
 static int number(String key,int fallback,int min,int max){try{return Math.max(min,Math.min(max,Integer.parseInt(values.getProperty(key,""+fallback))));}catch(NumberFormatException ex){return fallback;}}
}
