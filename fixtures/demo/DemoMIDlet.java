import javax.microedition.midlet.MIDlet;
import javax.microedition.lcdui.*;
/** Original test fixture, not a commercial game or copied asset. */
public final class DemoMIDlet extends MIDlet implements Runnable {
 private boolean running;private int frame;private String key="none";
 private final Canvas screen=new Canvas(){
  protected void paint(Graphics g){g.setColor(0x101c30);g.fillRect(0,0,getWidth(),getHeight());g.setColor(0x5edbe1);g.drawString("Legacy Mobile Forge",12,25,Graphics.TOP|Graphics.LEFT);g.setColor(0xf7cc59);g.fillRect(20+(frame%140),105,24,24);g.setColor(0xffffff);g.drawString("Original J2ME fixture",12,65,Graphics.TOP|Graphics.LEFT);g.drawString("Key: "+key,12,165,Graphics.TOP|Graphics.LEFT);g.drawString("Select: settings",12,210,Graphics.TOP|Graphics.LEFT);}
  public void keyPressed(int value){key=""+value;repaint();}
 };
 public void startApp(){Display.getDisplay(this).setCurrent(screen);if(!running){running=true;new Thread(this).start();}}
 public void pauseApp(){}
 public void destroyApp(boolean unconditional){running=false;}
 public void run(){while(running){frame++;screen.repaint();try{Thread.sleep(30);}catch(InterruptedException ex){running=false;}}}
}
