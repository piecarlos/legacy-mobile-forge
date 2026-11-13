import java.awt.image.BufferedImage;
import java.util.Arrays;

/** Scale2x: edge-aware pixel enlargement, no invented detail or blur. */
final class PixelArtFilter {
 private int[] current,previous,output;
 private BufferedImage enlarged;
 int builds;
 BufferedImage apply(BufferedImage source) {
  int w=source.getWidth(),h=source.getHeight();
  boolean resized=enlarged==null||enlarged.getWidth()!=w*2||enlarged.getHeight()!=h*2;
  if(resized){current=new int[w*h];previous=new int[w*h];output=new int[w*h*4];enlarged=new BufferedImage(w*2,h*2,BufferedImage.TYPE_INT_ARGB);}
  source.getRGB(0,0,w,h,current,0,w);
  if(!resized&&Arrays.equals(current,previous))return enlarged;
  int stride=w*2;
  for(int y=0;y<h;y++)for(int x=0;x<w;x++) {
   int e=current[y*w+x],b=current[Math.max(0,y-1)*w+x],d=current[y*w+Math.max(0,x-1)],f=current[y*w+Math.min(w-1,x+1)],hPixel=current[Math.min(h-1,y+1)*w+x];
   int pos=y*2*stride+x*2;
   if(b!=hPixel&&d!=f){
    output[pos]=d==b?d:e;output[pos+1]=b==f?f:e;
    output[pos+stride]=d==hPixel?d:e;output[pos+stride+1]=hPixel==f?f:e;
   }else{output[pos]=e;output[pos+1]=e;output[pos+stride]=e;output[pos+stride+1]=e;}
  }
  enlarged.setRGB(0,0,w*2,h*2,output,0,stride);
  System.arraycopy(current,0,previous,0,current.length);builds++;return enlarged;
 }
}
