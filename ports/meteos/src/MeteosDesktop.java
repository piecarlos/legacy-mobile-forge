import org.recompile.mobile.*;
import org.recompile.freej2me.Config;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.imageio.ImageIO;
public class MeteosDesktop {
 static final File appRoot=locateRoot(); static final File dataRoot=new File(System.getProperty("meteos.data",appRoot.getAbsolutePath()));
 static File locateRoot(){try{File location=new File(MeteosDesktop.class.getProtectionDomain().getCodeSource().getLocation().toURI());return location.getParentFile();}catch(Exception ex){return new File(System.getProperty("user.dir"));}}
 static File assetFile(String name){return new File(appRoot,name);}
 static MobilePlatform platform;
 static final java.util.concurrent.atomic.AtomicLong producedFrames=new java.util.concurrent.atomic.AtomicLong();
 static JPanel screen; static VideoSettings video;
 static JFrame window;
 static Process pad; static volatile boolean videoDialogPending;
 static void requestVideo(){ if(window==null||video==null||!window.isFocused()||videoDialogPending)return; videoDialogPending=true; SwingUtilities.invokeLater(()->{try{video.showDialog();}finally{videoDialogPending=false;}}); }
 static java.util.Set<Integer> uiHeld=java.util.concurrent.ConcurrentHashMap.newKeySet(); static java.util.Set<Integer> held=java.util.concurrent.ConcurrentHashMap.newKeySet();
 static void input(int k,boolean down){if(video!=null&&video.overlay!=null&&video.overlay.opened){if(down&&window.isFocused()){if(uiHeld.add(k))SwingUtilities.invokeLater(()->video.overlay.command(k));}else uiHeld.remove(k);return;}uiHeld.remove(k);if(window!=null&&!window.isFocused())down=false; if(down){if(held.add(k))MobilePlatform.keyPressed(k);}else if(held.remove(k))MobilePlatform.keyReleased(k);}
 static int keyboard(int k){return switch(k){case KeyEvent.VK_UP,KeyEvent.VK_W->-1;case KeyEvent.VK_DOWN,KeyEvent.VK_S->-2;case KeyEvent.VK_LEFT,KeyEvent.VK_A->-3;case KeyEvent.VK_RIGHT,KeyEvent.VK_D->-4;case KeyEvent.VK_ENTER,KeyEvent.VK_SPACE->-5;case KeyEvent.VK_Q,KeyEvent.VK_ESCAPE,KeyEvent.VK_F1->-6;case KeyEvent.VK_E->-7;case KeyEvent.VK_X->48;case KeyEvent.VK_TAB,KeyEvent.VK_PAGE_DOWN->-9;case KeyEvent.VK_PAGE_UP->-8;default->0;};}
 static void controller(){Thread t=new Thread(()->{try{pad=new ProcessBuilder(assetFile("tools/freej2me/Win32Pad.exe").getAbsolutePath(),"0").start();Runtime.getRuntime().addShutdownHook(new Thread(()->pad.destroy()));DataInputStream in=new DataInputStream(pad.getInputStream());byte[] b=new byte[8];while(true){in.readFully(b);int v=(short)((b[4]&255)|((b[5]&255)<<8)),type=b[6]&127,id=b[7]&255;if(type==1){if(id==5){if(v!=0)requestVideo();continue;}int k=switch(id){case 12,0->-5;case 13,1->-7;case 4,14,2->-6;case 15,3->48;case 8->-8;case 9->-9;default->0;};if(k!=0)input(k,v!=0);}else if(type==2){if(id==16){input(-3,v < -12000);input(-4,v > 12000);}if(id==17){input(-1,v < -12000);input(-2,v > 12000);}}}}catch(Exception ex){if(!MobilePlatform.appTerminated)System.err.println("Controller: "+ex);}},"GameSir-input");t.setDaemon(true);t.start();}
 static void screenDoubleBufferPolicy(){RepaintManager.currentManager(null).setDoubleBufferingEnabled(true);}
 static void watchForExit(){new javax.swing.Timer(100,e->{if(MobilePlatform.appTerminated)exitToDesktop();}).start();}
 static void exitToDesktop(){ if(video!=null)video.save();try{if(Mobile.midlet!=null)Mobile.midlet.requestDestroy(true);}catch(Exception ex){System.err.println(ex);}if(pad!=null)pad.destroy();System.exit(0); }
 static void press(int k) throws Exception { MobilePlatform.keyPressed(k); Thread.sleep(100); MobilePlatform.keyReleased(k); }
 public static void main(String[] args) throws Exception {
  SessionLog.start(new File(dataRoot,"logs").toPath(),appRoot);
  System.out.println("START");
  boolean test=args.length>0 && args[0].equals("--test");
  System.setProperty("file.encoding","ISO_8859_1");
  Mobile.lcdWidth=240; Mobile.lcdHeight=320; Mobile.sound=!test;
  System.out.println("PLATFORM");
  platform=new MobilePlatform(240,320);
  System.out.println("CONFIG");
  Mobile.setPlatform(platform,()->{Mobile.maskIndex=0;Mobile.renderLCDMask=false;});
  Mobile.config.onChange=()->{Mobile.maskIndex=0;Mobile.renderLCDMask=false;};
  Mobile.maskIndex=0;Mobile.renderLCDMask=false;
  platform.dataPath=new File(dataRoot,"saves").getAbsolutePath()+File.separator;
  new File(platform.dataPath).mkdirs();
  platform.setPainter(()->{producedFrames.incrementAndGet();if(screen!=null)screen.repaint();});
  System.out.println("LOAD");
  if(!platform.load(assetFile("game/meteos.jar").toURI().toString())) throw new IOException("Failed to load game");
  if(!test) SwingUtilities.invokeAndWait(()->{
   JFrame frame=new JFrame("Meteos — Astro Blocks | PC prototype");
   VideoSettings.prepareFrame(frame);screenDoubleBufferPolicy();window=frame;try{if(assetFile("assets/icon.png").isFile())frame.setIconImage(ImageIO.read(assetFile("assets/icon.png")));}catch(IOException ex){System.err.println(ex);}video=new VideoSettings(new File(dataRoot,"video.properties").toPath());
   screen=new JPanel(){protected void paintComponent(Graphics g){super.paintComponent(g); video.paint(g,platform.getLcdFrontbuffer().getCanvas(),getWidth(),getHeight());}};
   screen.setCursor(Toolkit.getDefaultToolkit().createCustomCursor(new java.awt.image.BufferedImage(1,1,java.awt.image.BufferedImage.TYPE_INT_ARGB),new Point(0,0),"hidden"));screen.setBackground(Color.BLACK); screen.setPreferredSize(new Dimension(480,640));
   screen.setDoubleBuffered(true);screen.setFocusable(true);
   screen.addKeyListener(new KeyAdapter(){ public void keyPressed(KeyEvent e){int k=keyboard(e.getKeyCode());if(k!=0)input(k,true);} public void keyReleased(KeyEvent e){int k=keyboard(e.getKeyCode());if(k!=0)input(k,false);}});
   frame.addWindowFocusListener(new WindowAdapter(){public void windowLostFocus(WindowEvent e){for(int k:java.util.Set.copyOf(held))input(k,false);}});
   frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);frame.add(screen);frame.pack();video.attach(frame,screen);frame.setVisible(true);video.start();screen.requestFocusInWindow();
   // ReShade processes input at Present. Redraw the complete Swing root even
   // when the MIDlet is paused or displays a static screen, never a dirty fragment.
   if(System.getProperty("reshade.library")!=null){javax.swing.Timer presentation=new javax.swing.Timer(16,e->frame.getRootPane().repaint());presentation.setCoalesce(true);presentation.start();}
  });
  System.out.println("RUN");
  platform.runJar();
  if(!test){controller();watchForExit();new javax.swing.Timer(30000,e->SessionLog.event("runtime.status","framesProduced="+producedFrames.get()+" paused="+Mobile.isPaused+" settings="+(video.overlay!=null&&video.overlay.opened)+" focused="+window.isFocused())).start();if(args.length>0&&args[0].equals("--smoke")){javax.swing.Timer smoke=new javax.swing.Timer(6000,e->{try{if(Mobile.midlet==null||!window.isDisplayable()||screen.getCursor().getType()!=Cursor.CUSTOM_CURSOR)throw new IllegalStateException("Startup/cursor failure");if(pad==null||!pad.isAlive())throw new IllegalStateException("Controller helper stopped");video.showDialog();if(video.overlay.getCursor().getType()!=Cursor.DEFAULT_CURSOR||!video.overlay.opened)throw new IllegalStateException("Menu cursor failure");video.overlay.close();java.nio.file.Files.writeString(new File(dataRoot,"smoke-result.txt").toPath(),"PASS: bundled runtime; independent paths; game loaded; hidden game cursor; visible settings cursor; controller helper alive; game exit signal\\n");Mobile.midlet.notifyDestroyed();}catch(Exception ex){ex.printStackTrace();System.exit(1);}});smoke.setRepeats(false);smoke.start();}}
  if(test){new File("analysis").mkdirs();Thread.sleep(6000);ImageIO.write(platform.getLcdFrontbuffer().getCanvas(),"png",new File("analysis/boot.png")); for(int i=0;i<12;i++){press(Mobile.NOKIA_SOFT1);Thread.sleep(1200);} ImageIO.write(platform.getLcdFrontbuffer().getCanvas(),"png",new File("analysis/menu.png")); press(-5);Thread.sleep(3000);press(-6);Thread.sleep(1000);ImageIO.write(platform.getLcdFrontbuffer().getCanvas(),"png",new File("analysis/pause.png"));System.out.println("BOOT_TEST_FINISHED"); System.exit(0);}
 }
}
