using System;
using System.IO;
using System.Threading;
using System.Runtime.InteropServices;

// SDL3 -> the 8-byte logical input protocol consumed by the Java front end.
class SdlPadBridge {
 [DllImport("SDL3.dll",CallingConvention=CallingConvention.Cdecl)] [return: MarshalAs(UnmanagedType.I1)] static extern bool SDL_Init(uint flags);
 [DllImport("SDL3.dll",CallingConvention=CallingConvention.Cdecl)] static extern void SDL_Quit();
 [DllImport("SDL3.dll",CallingConvention=CallingConvention.Cdecl)] static extern void SDL_PumpEvents();
 [DllImport("SDL3.dll",CallingConvention=CallingConvention.Cdecl)] static extern IntPtr SDL_GetGamepads(out int count);
 [DllImport("SDL3.dll",CallingConvention=CallingConvention.Cdecl)] static extern void SDL_free(IntPtr value);
 [DllImport("SDL3.dll",CallingConvention=CallingConvention.Cdecl)] static extern IntPtr SDL_OpenGamepad(uint id);
 [DllImport("SDL3.dll",CallingConvention=CallingConvention.Cdecl)] static extern void SDL_CloseGamepad(IntPtr pad);
 [DllImport("SDL3.dll",CallingConvention=CallingConvention.Cdecl)] [return: MarshalAs(UnmanagedType.I1)] static extern bool SDL_GamepadConnected(IntPtr pad);
 [DllImport("SDL3.dll",CallingConvention=CallingConvention.Cdecl)] [return: MarshalAs(UnmanagedType.I1)] static extern bool SDL_GetGamepadButton(IntPtr pad,int button);
 [DllImport("SDL3.dll",CallingConvention=CallingConvention.Cdecl)] static extern short SDL_GetGamepadAxis(IntPtr pad,int axis);
 [DllImport("SDL3.dll",CallingConvention=CallingConvention.Cdecl)] static extern IntPtr SDL_GetGamepadName(IntPtr pad);
 [DllImport("SDL3.dll",CallingConvention=CallingConvention.Cdecl)] static extern IntPtr SDL_GetError();
 [DllImport("SDL3.dll",CallingConvention=CallingConvention.Cdecl)] [return: MarshalAs(UnmanagedType.I1)] static extern bool SDL_SetHint(string name,string value);
 [DllImport("SDL3.dll",CallingConvention=CallingConvention.Cdecl)] static extern IntPtr SDL_IOFromFile(string file,string mode);
 [DllImport("SDL3.dll",CallingConvention=CallingConvention.Cdecl)] static extern int SDL_AddGamepadMappingsFromIO(IntPtr io,[MarshalAs(UnmanagedType.I1)] bool closeio);
 static readonly int[] buttons={0,1,2,3,4,6,9,10};
 static readonly byte[] logical={12,13,14,15,5,4,8,9};
 static void Send(BinaryWriter writer,short value,byte type,byte number) {writer.Write(0);writer.Write(value);writer.Write(type);writer.Write(number);writer.Flush();}
 public static int Main(string[] args) {
  bool list=Array.IndexOf(args,"--list")>=0;
  int slot=0;for(int i=0;i+1<args.Length;i++)if(args[i]=="--slot")int.TryParse(args[i+1],out slot);
  SDL_SetHint("SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS","1");
  if(!SDL_Init(0x00002000)){Console.Error.WriteLine(Marshal.PtrToStringAnsi(SDL_GetError()));return 2;}
  if(!list){Thread parent=new Thread(delegate(){try{if(Console.OpenStandardInput().ReadByte()<0)Environment.Exit(0);}catch(IOException){Environment.Exit(0);}});parent.IsBackground=true;parent.Start();}
  string mappings=Path.Combine(AppDomain.CurrentDomain.BaseDirectory,"gamecontrollerdb.txt");
  if(File.Exists(mappings)){IntPtr io=SDL_IOFromFile(mappings,"rb");if(io!=IntPtr.Zero)SDL_AddGamepadMappingsFromIO(io,true);}
  IntPtr gamepad=IntPtr.Zero;bool[] old=new bool[buttons.Length];short oldX=0,oldY=0;
  try {
   using(BinaryWriter output=new BinaryWriter(Console.OpenStandardOutput()))while(true) {
    SDL_PumpEvents();
    if(gamepad!=IntPtr.Zero&&!SDL_GamepadConnected(gamepad)) {
     for(int b=0;b<old.Length;b++)if(old[b]){Send(output,0,1,logical[b]);old[b]=false;}
     Send(output,0,2,16);Send(output,0,2,17);oldX=oldY=0;
     SDL_CloseGamepad(gamepad);gamepad=IntPtr.Zero;Console.Error.WriteLine("Controller disconnected");
    }
    if(gamepad==IntPtr.Zero||list) {
     int count;IntPtr devices=SDL_GetGamepads(out count);
     if(list) {
      Console.WriteLine("SDL3 recognized gamepads: "+count);
      for(int i=0;i<count;i++){IntPtr p=SDL_OpenGamepad((uint)Marshal.ReadInt32(devices,i*4));Console.WriteLine(i+": "+Marshal.PtrToStringAnsi(SDL_GetGamepadName(p)));SDL_CloseGamepad(p);}
      SDL_free(devices);return 0;
     }
     if(count>slot&&slot>=0){gamepad=SDL_OpenGamepad((uint)Marshal.ReadInt32(devices,slot*4));if(gamepad!=IntPtr.Zero)Console.Error.WriteLine("SDL3 controller: "+Marshal.PtrToStringAnsi(SDL_GetGamepadName(gamepad)));}
     SDL_free(devices);
    }
    if(gamepad!=IntPtr.Zero) {
     for(int i=0;i<buttons.Length;i++){bool state=SDL_GetGamepadButton(gamepad,buttons[i]);if(state!=old[i]){Send(output,(short)(state?1:0),1,logical[i]);old[i]=state;}}
     int axisX=SDL_GetGamepadAxis(gamepad,0),axisY=SDL_GetGamepadAxis(gamepad,1);
     short x=(short)(SDL_GetGamepadButton(gamepad,13)||axisX < -12000?-32767:SDL_GetGamepadButton(gamepad,14)||axisX > 12000?32767:0);
     short y=(short)(SDL_GetGamepadButton(gamepad,11)||axisY < -12000?-32767:SDL_GetGamepadButton(gamepad,12)||axisY > 12000?32767:0);
     if(x!=oldX){Send(output,x,2,16);oldX=x;}if(y!=oldY){Send(output,y,2,17);oldY=y;}
    }
    Thread.Sleep(8);
   }
  }catch(IOException){return 0;}finally{if(gamepad!=IntPtr.Zero)SDL_CloseGamepad(gamepad);SDL_Quit();}
 }
}
