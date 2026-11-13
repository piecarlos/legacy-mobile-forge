/** Optional experimental entry point. No download or global installation. */
public final class ReshadeBootstrap {
 public static void main(String[] args)throws Exception {
  String library=System.getProperty("forge.reshade.library");
  if(library==null||library.isBlank())throw new IllegalArgumentException("Provide -Dforge.reshade.library=absolute-path-to-d3d9.dll from the official ReShade setup.");
  java.nio.file.Path path=java.nio.file.Path.of(library).toRealPath();
  System.load(path.toString());
  String entry=System.getProperty("forge.entryClass","MobileGameDesktop");
  Class.forName(entry).getMethod("main",String[].class).invoke(null,(Object)args);
 }
}
