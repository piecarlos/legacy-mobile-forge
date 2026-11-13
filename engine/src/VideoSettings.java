import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.*;
import java.nio.file.*;
import java.util.Properties;
import javax.swing.*;

/** Presentation only: keeps each game's configured logical screen. */
public final class VideoSettings {
    static final String[] RESOLUTIONS = {"Compacta — 960 × 540", "Full HD — 1920 × 1080", "QHD — 2560 × 1440", "3K — 2880 × 1620", "4K UHD — 3840 × 2160"};
    static final int[][] SIZES = {{960,540},{1920,1080},{2560,1440},{2880,1620},{3840,2160}};
    final Path file;
    int resolution;
    boolean integerScale = true, smooth, fullscreen;
    boolean pixelArt; final PixelArtFilter pixelFilter=new PixelArtFilter(); boolean bezel = true; final SpaceBezel artwork = new SpaceBezel();
    JFrame frame;
    JPanel canvas;
    JMenuBar bar; SettingsOverlay overlay;
    Rectangle windowBounds;
    String notice = "";

    VideoSettings(Path file) { this.file=file; load(); }
    void load() {
        Properties p = new Properties();
        if (!Files.exists(file)) return;
        try (InputStream in=Files.newInputStream(file)) {
            p.load(in);
            resolution=Math.max(0,Math.min(SIZES.length-1,Integer.parseInt(p.getProperty("resolution","0"))));
            integerScale=Boolean.parseBoolean(p.getProperty("integerScale","true"));
            smooth=Boolean.parseBoolean(p.getProperty("smooth","false"));pixelArt=Boolean.parseBoolean(p.getProperty("pixelArt","false"));if(pixelArt)smooth=false;
            fullscreen=Boolean.parseBoolean(p.getProperty("fullscreen","false")); bezel=Boolean.parseBoolean(p.getProperty("bezel","true"));
        } catch (IOException | IllegalArgumentException ex) { notice="Não foi possível ler as preferências; usando valores disponíveis."; }
    }
    void save() {
        Properties p=new Properties();
        p.setProperty("resolution",String.valueOf(resolution));
        p.setProperty("integerScale",String.valueOf(integerScale));
        p.setProperty("smooth",String.valueOf(smooth));p.setProperty("pixelArt",String.valueOf(pixelArt));
        p.setProperty("fullscreen",String.valueOf(fullscreen));p.setProperty("bezel",String.valueOf(bezel));
        try {
            Path parent=file.toAbsolutePath().getParent(); Files.createDirectories(parent);
            Path tmp=Files.createTempFile(parent,"video-",".tmp");
            try(OutputStream out=Files.newOutputStream(tmp)){p.store(out,"Legacy Mobile Forge - video");}
            Files.move(tmp,file.toAbsolutePath(),StandardCopyOption.REPLACE_EXISTING);
        } catch(IOException ex){ notice="Não foi possível salvar as preferências: "+ex.getMessage(); }
    }
    static Rectangle viewport(int width,int height,boolean integer) {
        double scale=Math.min(width/(double)GameProfile.width,height/(double)GameProfile.height);
        if(integer && scale>=1) scale=Math.floor(scale);
        int w=Math.max(1,(int)Math.floor(GameProfile.width*scale)),h=Math.max(1,(int)Math.floor(GameProfile.height*scale));
        return new Rectangle((width-w)/2,(height-h)/2,w,h);
    }
    void paint(Graphics graphics,BufferedImage image,int width,int height) {
        Graphics2D g=(Graphics2D)graphics.create();
        try {
            g.setColor(Color.BLACK);g.fillRect(0,0,width,height);
            g.setRenderingHint(RenderingHints.KEY_INTERPOLATION,smooth&&!pixelArt?RenderingHints.VALUE_INTERPOLATION_BILINEAR:RenderingHints.VALUE_INTERPOLATION_NEAREST_NEIGHBOR);
            Rectangle r=viewport(width,height,integerScale); if(bezel)g.drawImage(artwork.image(width,height,r),0,0,null);
            g.drawImage(pixelArt?pixelFilter.apply(image):image,r.x,r.y,r.width,r.height,null);
        } finally {g.dispose();}
    }
    void attach(JFrame frame,JPanel canvas) {
        this.frame=frame;this.canvas=canvas;
        bar=new JMenuBar();JMenu menu=new JMenu("Vídeo");bar.add(menu);
        JMenuItem settings=new JMenuItem("Configurações…  F2");settings.addActionListener(e->showDialog());menu.add(settings);
        JMenuItem toggle=new JMenuItem("Janela / tela cheia  F11");toggle.addActionListener(e->toggleFullscreen());menu.add(toggle);
        frame.setJMenuBar(bar);
        bind("F2","video-settings",this::showDialog);
        bind("F11","video-fullscreen",this::toggleFullscreen);
        // Esc and Start continue to belong to the game's own pause menu.
        frame.addWindowListener(new WindowAdapter(){public void windowClosing(WindowEvent e){save();}});
        applySize();
    }
    void bind(String key,String name,Runnable action) {
        frame.getRootPane().getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(KeyStroke.getKeyStroke(key),name);
        frame.getRootPane().getActionMap().put(name,new AbstractAction(){public void actionPerformed(ActionEvent e){action.run();}});
    }
    void showDialog(){ if(overlay==null){overlay=new SettingsOverlay(this);frame.setGlassPane(overlay);} if(overlay.opened)overlay.close();else overlay.open(); }
    void toggleFullscreen(){ if(overlay!=null&&overlay.opened)return;fullscreen=!fullscreen;changeMode();save();canvas.requestFocusInWindow(); }
    void changeMode() {
        Rectangle monitor=frame.getGraphicsConfiguration().getBounds();
        if(fullscreen)windowBounds=frame.getBounds();
        frame.dispose();frame.setUndecorated(fullscreen);bar.setVisible(!fullscreen);
        if(fullscreen){frame.setBounds(monitor);}
        else if(windowBounds!=null){frame.setBounds(windowBounds);}
        else applySize();
        frame.setVisible(true);canvas.requestFocusInWindow();
    }
    void start(){if(fullscreen){fullscreen=false;toggleFullscreen();}}
    void applySize() {
        GraphicsConfiguration config=frame.getGraphicsConfiguration();
        Rectangle bounds=config.getBounds();Insets work=Toolkit.getDefaultToolkit().getScreenInsets(config);
        Insets chrome=frame.getInsets();int menuHeight=bar==null?0:bar.getPreferredSize().height;
        int availableW=Math.max(1,bounds.width-work.left-work.right-chrome.left-chrome.right);
        int availableH=Math.max(1,bounds.height-work.top-work.bottom-chrome.top-chrome.bottom-menuHeight);
        int[] target=SIZES[resolution];double s=Math.min(1,Math.min(availableW/(double)target[0],availableH/(double)target[1]));
        canvas.setPreferredSize(new Dimension((int)(target[0]*s),(int)(target[1]*s)));
        frame.pack();frame.setLocation(bounds.x+work.left+(bounds.width-work.left-work.right-frame.getWidth())/2,bounds.y+work.top+(bounds.height-work.top-work.bottom-frame.getHeight())/2);
    }
}
