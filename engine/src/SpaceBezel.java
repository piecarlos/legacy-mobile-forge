import java.awt.*;
import java.awt.geom.*;
import java.awt.image.BufferedImage;
import java.util.Random;

/** Static vector artwork, cached at the output size; never alters game pixels. */
final class SpaceBezel {
 private BufferedImage cache; private BufferedImage illustration; private boolean attempted;
 private Rectangle viewport;
 int builds;
 BufferedImage image(int width,int height,Rectangle game) {
  if(cache!=null&&cache.getWidth()==width&&cache.getHeight()==height&&game.equals(viewport))return cache;
  cache=new BufferedImage(width,height,BufferedImage.TYPE_INT_RGB);viewport=new Rectangle(game);builds++;
  Graphics2D g=cache.createGraphics();
  if(!attempted){attempted=true;try{if(MobileGameDesktop.assetFile("assets/bezel.png").isFile())if(MobileGameDesktop.assetFile("assets/bezel.png").isFile())illustration=javax.imageio.ImageIO.read(MobileGameDesktop.assetFile("assets/bezel.png"));}catch(java.io.IOException ex){System.err.println("Bezel: "+ex.getMessage());}}
  if(illustration!=null){g.setRenderingHint(RenderingHints.KEY_INTERPOLATION,RenderingHints.VALUE_INTERPOLATION_BICUBIC);int sw=illustration.getWidth(),sh=illustration.getHeight();g.drawImage(illustration,0,0,width,height,null);g.drawImage(illustration,0,0,game.x,height,0,0,sw/3,sh,null);g.drawImage(illustration,game.x+game.width,0,width,height,sw*2/3,0,sw,sh,null);g.dispose();return cache;}
  g.setRenderingHint(RenderingHints.KEY_ANTIALIASING,RenderingHints.VALUE_ANTIALIAS_ON);
  g.setPaint(new GradientPaint(0,0,new Color(7,14,30),width,height,new Color(18,10,32)));
  g.fillRect(0,0,width,height);
  side(g,0,game.x,height,false);
  side(g,game.x+game.width,width-game.x-game.width,height,true);
  g.setColor(new Color(82,131,152));g.setStroke(new BasicStroke(Math.max(1,width/1920f)));
  if(game.x>0)g.drawLine(game.x-1,game.y,game.x-1,game.y+game.height);
  if(game.x+game.width<width)g.drawLine(game.x+game.width,game.y,game.x+game.width,game.y+game.height);
  g.dispose();return cache;
 }
 private void side(Graphics2D source,int x,int width,int height,boolean right) {
  if(width<24)return;
  Graphics2D g=(Graphics2D)source.create();g.clipRect(x,0,width,height);
  g.translate(x,0);g.scale(width/560.0,height/1080.0);
  Random rng=new Random(right?892:145);
  for(int i=0;i<150;i++) {
   int px=rng.nextInt(560),py=rng.nextInt(1080),a=30+rng.nextInt(100);
   g.setColor(new Color(163,194,221,a));double r=i%13==0?2:1;g.fill(new Ellipse2D.Double(px,py,r,r));
  }
  g.setStroke(new BasicStroke(1));g.setColor(new Color(64,103,132,75));
  for(int i=0;i<3;i++)g.draw(new Ellipse2D.Double(-180+i*60,300+i*100,720,360));
  if(width>160) {
   g.setColor(new Color(128,182,199));g.setFont(new Font("SansSerif",Font.PLAIN,15));
   g.drawString(right?"FLIGHT / 02":"ORBIT / 01",52,72);
   g.setColor(new Color(74,111,135));g.drawLine(52,90,508,90);
   if(!right) {
    g.setColor(new Color(220,230,234));g.setFont(new Font("SansSerif",Font.BOLD,48));g.drawString(GameProfile.shortTitle,52,192);
    g.setColor(new Color(135,175,190));g.setFont(new Font("SansSerif",Font.PLAIN,17));g.drawString("LEGACY MOBILE FORGE",54,225);
    planet(g,185,678,105);
   } else {
    g.setColor(new Color(133,162,188));g.setFont(new Font("SansSerif",Font.PLAIN,16));g.drawString("IGNITE. CONNECT. LAUNCH.",52,190);
    for(int i=0;i<6;i++)block(g,100+(i%3)*105,430+(i/3)*105-i*16,62,i);
    g.setColor(new Color(85,120,151,150));g.draw(new Arc2D.Double(40,310,450,450,205,210,Arc2D.OPEN));
   }
   g.setColor(new Color(77,115,138));g.drawLine(52,954,508,954);
   g.setColor(new Color(155,180,193));g.setFont(new Font("SansSerif",Font.PLAIN,15));
   g.drawString(right?"SELECT / VÍDEO":"START / PAUSA",52,990);
   g.setColor(new Color(86,122,141));g.setFont(new Font("SansSerif",Font.PLAIN,12));g.drawString("P C   E D I T I O N",52,1020);
  }
  g.dispose();
 }
 private void planet(Graphics2D g,int x,int y,int r) {
  g.setPaint(new RadialGradientPaint(new Point2D.Float(x-r/3f,y-r/3f),r*1.5f,new float[]{0,0.7f,1},new Color[]{new Color(77,135,155),new Color(25,51,77),new Color(8,17,32)}));
  g.fillOval(x-r,y-r,r*2,r*2);
  g.setColor(new Color(117,161,177,150));g.setStroke(new BasicStroke(3));
  AffineTransform old=g.getTransform();g.rotate(-0.28,x,y);g.drawOval(x-r-62,y-27,r*2+124,54);g.setTransform(old);
 }
 private void block(Graphics2D g,int x,int y,int size,int i) {
  Color[] colors={new Color(55,130,161),new Color(122,78,143),new Color(153,123,63)};Color c=colors[i%3];
  g.setPaint(new GradientPaint(x,y,c.brighter(),x+size,y+size,c.darker()));g.fillRect(x,y,size,size);
  g.setColor(new Color(202,226,235,135));g.drawRect(x,y,size,size);g.drawRect(x+7,y+7,size-14,size-14);
  g.setColor(new Color(9,18,34,110));g.fillRect(x+7,y+size-15,size-14,8);
 }
}
