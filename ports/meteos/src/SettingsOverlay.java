import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/** In-window settings: one input path for keyboard, mouse and gamepad. */
final class SettingsOverlay extends JComponent {
 final VideoSettings owner;
 volatile boolean opened;
 int tab,row,resolution;
 String language;
 boolean full,integer,smooth,pixelArt,bezel,wasPaused,confirmExit;

 static final Color TEXT=new Color(232,238,245), MUTED=new Color(145,162,181), ACCENT=new Color(95,217,226);
 SettingsOverlay(VideoSettings owner) {
  this.owner=owner;setCursor(Cursor.getDefaultCursor());setOpaque(false);setFocusable(false);
  addMouseListener(new MouseAdapter(){public void mousePressed(MouseEvent e){mouse(e.getX(),e.getY());}});
 }
 void open() {
  if(opened)return;SessionLog.event("settings.open","");
  for(int key:java.util.Set.copyOf(MeteosDesktop.held))MeteosDesktop.input(key,false);
  language=owner.language;resolution=owner.resolution;full=owner.fullscreen;integer=owner.integerScale;smooth=owner.smooth;pixelArt=owner.pixelArt;bezel=owner.bezel;
  MeteosDesktop.uiHeld.clear();tab=0;row=0;confirmExit=false;wasPaused=org.recompile.mobile.Mobile.isPaused;
  org.recompile.mobile.Mobile.isPaused=true;opened=true;setVisible(true);repaint();SwingUtilities.invokeLater(()->owner.canvas.requestFocusInWindow());
 }
 void close() {
  if(!opened)return;SessionLog.event("settings.close","");
  opened=false;setVisible(false);org.recompile.mobile.Mobile.isPaused=wasPaused;MeteosDesktop.uiHeld.clear();owner.canvas.revalidate();owner.canvas.repaint();owner.frame.repaint();
  SwingUtilities.invokeLater(()->owner.canvas.requestFocusInWindow());
 }
 int rows(){return confirmExit?2:tab==0?8:tab==1?2:4;}
 void command(int key) {
  if(!opened)return;
  if(key==-7||key==-6){if(confirmExit){confirmExit=false;row=0;repaint();}else close();return;}
  if(key==-8||key==-9){if(!confirmExit){tab=Math.floorMod(tab+(key==-8?-1:1),3);row=0;}repaint();return;}
  if(key==-1)row=Math.floorMod(row-1,rows());
  else if(key==-2)row=(row+1)%rows();
  else if(key==-3||key==-4)change(key==-3?-1:1);
  else if(key==-5)activate();
  repaint();
 }
 void change(int direction) {
  if(confirmExit)return;
  if(tab==2){if(row==0)language=UiText.next(language,direction);return;}
  if(tab!=0)return;
  switch(row){case 0:resolution=Math.floorMod(resolution+direction,VideoSettings.SIZES.length);break;
   case 1:full=!full;break;case 2:integer=!integer;break;case 3:int mode=Math.floorMod((pixelArt?2:smooth?1:0)+direction,3);smooth=mode==1;pixelArt=mode==2;break;case 4:bezel=!bezel;break;}
 }
 void activate() {
  if(confirmExit){if(row==1)MeteosDesktop.exitToDesktop();else{confirmExit=false;row=0;}return;}
  if(tab==2){if(row==0)change(1);else if(row==1)SessionLog.openFolder();else if(row==2)apply();else{confirmExit=true;row=0;}return;}
  if(tab==1){if(row==0)close();else{confirmExit=true;row=0;}return;}
  if(row<5){change(1);return;}
  if(row==5){apply();return;}if(row==6){close();return;}confirmExit=true;row=0;
 }
 void apply() {
  boolean changed=owner.fullscreen!=full;owner.language=language;UiText.select(language);owner.refreshLanguage();
  owner.resolution=resolution;owner.integerScale=integer;owner.smooth=smooth;owner.pixelArt=pixelArt;owner.bezel=bezel;owner.fullscreen=full;
  close();if(changed)owner.changeMode();else if(!full)owner.applySize();owner.save();owner.canvas.repaint();
 }
 Rectangle panelBounds(){double scale=Math.min(getWidth()/1000.0,getHeight()/700.0);return new Rectangle((int)((getWidth()-920*scale)/2),(int)((getHeight()-630*scale)/2),(int)(920*scale),(int)(630*scale));}
 void mouse(int px,int py) {
  Rectangle r=panelBounds();double s=r.width/920.0;int x=(int)((px-r.x)/s),y=(int)((py-r.y)/s);
  if(x<0||x>920||y<0||y>630)return;
  if(!confirmExit&&y>=78&&y<124){tab=Math.max(0,Math.min(2,(x-24)/182));row=0;repaint();return;}
  int start=confirmExit?260:tab==0?156:tab==1?402:230,step=confirmExit?62:tab==0?46:52;
  int selected=(y-start)/step;
  if(y>=start&&selected<rows()){row=selected;if(row<5&&tab==0&&!confirmExit&&x<500)change(-1);else activate();repaint();}
 }
 protected void paintComponent(Graphics graphics) {
  Graphics2D dim=(Graphics2D)graphics.create();dim.setColor(new Color(0,0,0,185));dim.fillRect(0,0,getWidth(),getHeight());
  Rectangle p=panelBounds();dim.translate(p.x,p.y);dim.scale(p.width/920.0,p.height/630.0);
  draw(dim);dim.dispose();
 }
 void draw(Graphics2D g) {
  g.setRenderingHint(RenderingHints.KEY_ANTIALIASING,RenderingHints.VALUE_ANTIALIAS_ON);
  g.setColor(new Color(14,21,33));g.fillRoundRect(0,0,920,630,18,18);
  g.setColor(new Color(42,61,79));g.drawRoundRect(0,0,919,629,18,18);
  text(g,"METEOS",30,45,27,TEXT,Font.BOLD);text(g,UiText.t("CONFIGURAÇÕES"),665,43,15,MUTED,Font.PLAIN);
  String[] tabs=UiText.all("VÍDEO","CONTROLE","GERAL");
  for(int i=0;i<3;i++){g.setColor(i==tab?new Color(30,57,73):new Color(19,29,43));g.fillRect(24+i*182,78,174,45);text(g,tabs[i],43+i*182,107,17,i==tab?TEXT:MUTED,Font.BOLD);if(i==tab){g.setColor(ACCENT);g.fillRect(24+i*182,120,174,3);}}
  if(confirmExit){text(g,"Sair para o desktop?",34,205,26,TEXT,Font.BOLD);text(g,"A fase atual será encerrada.",34,237,17,MUTED,Font.PLAIN);line(g,0,260,62,"Continuar jogando","",true);line(g,1,322,62,"Sair para o desktop","",true);}
  else if(tab==0){
   String[] labels={"Resolução em janela","Modo de exibição","Escala do jogo","Filtro de imagem","Moldura espacial","Aplicar e continuar","Voltar sem aplicar","Sair para o desktop"};
   String[] values={UiText.t(VideoSettings.RESOLUTIONS[resolution]),full?"Tela cheia sem bordas":"Janela",integer?"Pixels uniformes":"Ajustar à altura",pixelArt?"Pixel art (Scale2x)":smooth?"Suave":"Pixels nítidos",bezel?"Ativada":"Desativada","","",""};
   for(int i=0;i<8;i++)line(g,i,156+i*46,46,labels[i],values[i],i>=5);
   text(g,"Tela cheia usa a resolução do desktop. Janelas grandes se ajustam ao monitor.",34,554,14,MUTED,Font.PLAIN);
  }else if(tab==1){
   String[] hints={"Direcional     Mover / navegar","A     Ação / confirmar","B     Voltar / fechar configurações","Start     Pausa do jogo","Select     Abrir / fechar configurações","LB / RB     Trocar aba"};
   for(int i=0;i<hints.length;i++)text(g,hints[i],36,170+i*34,19,i%2==0?TEXT:MUTED,Font.PLAIN);
   line(g,0,402,52,"Continuar jogando","",true);line(g,1,454,52,"Sair para o desktop","",true);
  }
  if(tab==2&&!confirmExit){
   line(g,0,230,52,"Idioma da interface",UiText.languageName(language),false);
   line(g,1,282,52,"Abrir pasta de logs","",true);
   line(g,2,334,52,"Aplicar e continuar","",true);
   line(g,3,386,52,"Sair para o desktop","",true);
   text(g,"O idioma altera apenas a interface do port, não o jogo original.",34,495,15,MUTED,Font.PLAIN);
   text(g,"Logs locais de teste. Nenhum dado é enviado automaticamente.",34,529,15,MUTED,Font.PLAIN);
  }
  g.setColor(new Color(42,61,79));g.drawLine(28,577,892,577);
  text(g,"↑ ↓  Navegar    ← →  Alterar    A  Confirmar    B / Select  Voltar",34,610,16,ACCENT,Font.PLAIN);
 }
 void line(Graphics2D g,int index,int y,int h,String label,String value,boolean action) {
  boolean active=index==row;g.setColor(active?new Color(28,64,80):new Color(20,31,45));g.fillRoundRect(26,y,868,h-5,8,8);
  if(active){g.setColor(ACCENT);g.fillRect(26,y+4,3,h-13);}
  text(g,label,43,y+(h-5)/2+7,18,active?TEXT:MUTED,action?Font.BOLD:Font.PLAIN);
  if(!value.isEmpty())text(g,"‹  "+value+"  ›",450,y+(h-5)/2+7,17,active?ACCENT:TEXT,Font.PLAIN);
 }
 static void text(Graphics2D g,String text,int x,int y,int size,Color color,int style){g.setFont(new Font("SansSerif",style,size));g.setColor(color);g.drawString(UiText.t(text),x,y);}
}
