/* Shared native pre-game shell. Included after the validated bootstrap helpers.
   Java is started only by Play. GDI/WIC and XInput are Windows components. */
#include <wincodec.h>
#include <xinput.h>
#include <windowsx.h>
#include <dwmapi.h>

#define UI_DONE (WM_APP+1)
typedef struct { RECT box; int action,value; } UiHit;
typedef struct { int resolution,filter,integer,fullscreen,bezel,language; } UiSettings;
static UiSettings ui_settings;
static HWND ui_window;
static HBITMAP ui_art;
static int ui_art_w,ui_art_h,ui_tab,ui_focus,ui_count;
static UiHit ui_hits[48];
static BOOL ui_ready,ui_running,ui_reshade;
static HANDLE ui_worker;
static WCHAR ui_notice[256];
static HMODULE ui_xinput;
typedef DWORD (WINAPI *UiGetState)(DWORD,XINPUT_STATE*);
static UiGetState ui_get_state;
static WORD ui_last_buttons;
static DWORD ui_repeat_at;
static int ui_last_axis;
static const COLORREF UI_BG=RGB(9,12,19),UI_PANEL=RGB(21,27,39),UI_LINE=RGB(48,61,80),UI_BLUE=RGB(45,126,255),UI_TEXT=RGB(239,244,255),UI_MUTED=RGB(157,176,202);

static void ui_read_settings(void){
 WCHAR path[PATH_CAP]; FILE *f=NULL; char line[2048],key[100],value[100];
 ui_settings=(UiSettings){1,0,1,0,1,0};
 if(joined(path,app,L"video.properties")&&!_wfopen_s(&f,path,L"rb")&&f){
  while(fgets(line,sizeof(line),f))if(sscanf_s(line,"%99[^=]=%99s",key,(unsigned)sizeof(key),value,(unsigned)sizeof(value))==2){
   if(!strcmp(key,"resolution"))ui_settings.resolution=atoi(value);
   else if(!strcmp(key,"smooth")&&!strcmp(value,"true"))ui_settings.filter=1;
   else if(!strcmp(key,"pixelArt")&&!strcmp(value,"true"))ui_settings.filter=2;
   else if(!strcmp(key,"integerScale"))ui_settings.integer=!strcmp(value,"true");
   else if(!strcmp(key,"fullscreen"))ui_settings.fullscreen=!strcmp(value,"true");
   else if(!strcmp(key,"bezel"))ui_settings.bezel=!strcmp(value,"true");
   else if(!strcmp(key,"language"))ui_settings.language=!strcmp(value,"en")?1:!strcmp(value,"pt-BR")?2:0;
  }
  fclose(f);
 }
 if(ui_settings.resolution<0||ui_settings.resolution>4)ui_settings.resolution=1;
 portuguese=ui_settings.language==2||(ui_settings.language==0&&PRIMARYLANGID(GetUserDefaultUILanguage())==LANG_PORTUGUESE);
}
static BOOL ui_known_property(const char *line){
 const char *keys[]={"resolution=","smooth=","pixelArt=","integerScale=","fullscreen=","bezel=","language="};
 for(int i=0;i<ARRAYSIZE(keys);i++)if(!strncmp(line,keys[i],strlen(keys[i])))return TRUE;
 return FALSE;
}
static BOOL ui_save_settings(void){
 WCHAR path[PATH_CAP],tmp[PATH_CAP]; FILE *input=NULL,*output=NULL;char line[4096];BOOL ok=FALSE;
 if(!joined(path,app,L"video.properties")||!GetTempFileNameW(app,L"vid",0,tmp))return FALSE;
 if(_wfopen_s(&output,tmp,L"wb"))goto done;
 if(!_wfopen_s(&input,path,L"rb")&&input){while(fgets(line,sizeof(line),input))if(!ui_known_property(line))fputs(line,output);fclose(input);}
 fprintf(output,"\nresolution=%d\nsmooth=%s\npixelArt=%s\nintegerScale=%s\nfullscreen=%s\nbezel=%s\nlanguage=%s\n",ui_settings.resolution,ui_settings.filter==1?"true":"false",ui_settings.filter==2?"true":"false",ui_settings.integer?"true":"false",ui_settings.fullscreen?"true":"false",ui_settings.bezel?"true":"false",ui_settings.language==1?"en":ui_settings.language==2?"pt-BR":"auto");
 ok=!ferror(output);if(fclose(output))ok=FALSE;output=NULL;
 if(ok)ok=MoveFileExW(tmp,path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
done:
 if(output)fclose(output);DeleteFileW(tmp);event("settings.save",ok?0:GetLastError());return ok;
}
static void ui_load_art(void){
 WCHAR profile[PATH_CAP],relative[PATH_CAP],path[PATH_CAP];IWICImagingFactory *factory=NULL;IWICBitmapDecoder *decoder=NULL;IWICBitmapFrameDecode *frame=NULL;IWICFormatConverter *converter=NULL;void *bits=NULL;
 if(!joined(profile,app,L"launcher.ini"))return;
 GetPrivateProfileStringW(L"launcher",L"artwork",L"assets\\bezel.png",relative,PATH_CAP,profile);
 if(!safe_relative(relative)||!joined(path,app,relative))return;
 HRESULT hr=CoCreateInstance(&CLSID_WICImagingFactory,NULL,CLSCTX_INPROC_SERVER,&IID_IWICImagingFactory,(void**)&factory);
 if(SUCCEEDED(hr))hr=IWICImagingFactory_CreateDecoderFromFilename(factory,path,NULL,GENERIC_READ,WICDecodeMetadataCacheOnLoad,&decoder);
 if(SUCCEEDED(hr))hr=IWICBitmapDecoder_GetFrame(decoder,0,&frame);
 UINT width=0,height=0;
 if(SUCCEEDED(hr))hr=IWICBitmapFrameDecode_GetSize(frame,&width,&height);
 if(!width||!height||width>8192||height>8192)hr=E_INVALIDARG;
 if(SUCCEEDED(hr))hr=IWICImagingFactory_CreateFormatConverter(factory,&converter);
 if(SUCCEEDED(hr))hr=IWICFormatConverter_Initialize(converter,(IWICBitmapSource*)frame,&GUID_WICPixelFormat32bppBGRA,WICBitmapDitherTypeNone,NULL,0,WICBitmapPaletteTypeCustom);
 if(SUCCEEDED(hr)){
  BITMAPINFO info={0};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=(LONG)width;info.bmiHeader.biHeight=-(LONG)height;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;
  ui_art=CreateDIBSection(NULL,&info,DIB_RGB_COLORS,&bits,NULL,0);
  if(ui_art&&SUCCEEDED(IWICFormatConverter_CopyPixels(converter,NULL,width*4,width*height*4,bits))){ui_art_w=(int)width;ui_art_h=(int)height;}
  else if(ui_art){DeleteObject(ui_art);ui_art=NULL;}
 }
 if(converter)IWICFormatConverter_Release(converter);if(frame)IWICBitmapFrameDecode_Release(frame);if(decoder)IWICBitmapDecoder_Release(decoder);if(factory)IWICImagingFactory_Release(factory);
}
static void ui_fill(HDC dc,RECT r,COLORREF color){HBRUSH brush=CreateSolidBrush(color);FillRect(dc,&r,brush);DeleteObject(brush);}
static void ui_text(HDC dc,const WCHAR *text,RECT r,int size,int weight,COLORREF color,UINT flags){
 HFONT font=CreateFontW(-size,0,0,0,weight,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
 HGDIOBJ old=SelectObject(dc,font);SetBkMode(dc,TRANSPARENT);SetTextColor(dc,color);DrawTextW(dc,text,-1,&r,flags|DT_NOPREFIX);SelectObject(dc,old);DeleteObject(font);
}
static void ui_outline(HDC dc,RECT r,COLORREF color,int thickness){
 ui_fill(dc,(RECT){r.left,r.top,r.right,r.top+thickness},color);ui_fill(dc,(RECT){r.left,r.bottom-thickness,r.right,r.bottom},color);
 ui_fill(dc,(RECT){r.left,r.top,r.left+thickness,r.bottom},color);ui_fill(dc,(RECT){r.right-thickness,r.top,r.right,r.bottom},color);
}
static void ui_button(HDC dc,RECT r,const WCHAR *label,int action,int value,BOOL selected){
 int index=ui_count;if(index>=ARRAYSIZE(ui_hits))return;ui_hits[ui_count++]=(UiHit){r,action,value};
 ui_fill(dc,r,selected?UI_BLUE:UI_PANEL);ui_outline(dc,r,ui_focus==index?RGB(221,237,255):selected?UI_BLUE:UI_LINE,ui_focus==index?2:1);
 ui_text(dc,label,r,14,FW_SEMIBOLD,UI_TEXT,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}
static void ui_row(HDC dc,int width,int y,const WCHAR *label,const WCHAR *description,const WCHAR **options,int count,int current,int action){
 ui_text(dc,label,(RECT){36,y,width-36,y+25},16,FW_SEMIBOLD,UI_TEXT,DT_SINGLELINE);
 ui_text(dc,description,(RECT){36,y+29,width-36,y+53},13,FW_NORMAL,UI_MUTED,DT_SINGLELINE|DT_END_ELLIPSIS);
 int right=width-36,choice_width=min(560,width/2),left=right-choice_width;
 for(int i=0;i<count;i++)ui_button(dc,(RECT){left+i*choice_width/count,y,right-(count-i-1)*choice_width/count,y+30},options[i],action,i,current==i);
 ui_fill(dc,(RECT){36,y+66,width-36,y+67},UI_LINE);
}
static void ui_paint(HDC dest,int width,int height){
 int physical_w=width,physical_h=height;
 HDC dc=CreateCompatibleDC(dest);HBITMAP buffer=CreateCompatibleBitmap(dest,width,height);HGDIOBJ old=SelectObject(dc,buffer);WCHAR text[600];
 SetMapMode(dc,MM_ANISOTROPIC);SetWindowExtEx(dc,1200,850,NULL);SetViewportExtEx(dc,width,height,NULL);width=1200;height=850;
 ui_count=0;ui_fill(dc,(RECT){0,0,width,height},UI_BG);
 if(ui_art){HDC image=CreateCompatibleDC(dc);HGDIOBJ before=SelectObject(image,ui_art);SetStretchBltMode(dc,HALFTONE);StretchBlt(dc,0,0,width,166,image,0,0,ui_art_w,ui_art_h/2,SRCCOPY);SelectObject(image,before);DeleteDC(image);}
 ui_fill(dc,(RECT){26,24,min(width-185,600),141},RGB(9,22,43));
 ui_text(dc,title,(RECT){42,34,width-190,85},32,FW_BOLD,UI_TEXT,DT_SINGLELINE|DT_END_ELLIPSIS);
 ui_text(dc,L"L E G A C Y   M O B I L E   F O R G E",(RECT){44,99,width-190,122},12,FW_SEMIBOLD,RGB(120,199,255),DT_SINGLELINE);
 ui_button(dc,(RECT){width-154,30,width-30,67},L"GitHub",100,0,FALSE);
 ui_fill(dc,(RECT){32,182,width-32,248},UI_PANEL);ui_fill(dc,(RECT){32,182,36,248},UI_BLUE);
 ui_text(dc,title,(RECT){52,191,width-280,220},21,FW_SEMIBOLD,UI_TEXT,DT_SINGLELINE|DT_END_ELLIPSIS);
 StringCchPrintfW(text,ARRAYSIZE(text),L"Java ME  /  %s  /  %s",version,resolution);ui_text(dc,text,(RECT){52,222,width-280,245},12,FW_NORMAL,UI_MUTED,DT_SINGLELINE);
 ui_button(dc,(RECT){width-258,196,width-48,233},ui_ready?tr(L"Change JAR",L"Selecionar JAR"):tr(L"Select original JAR",L"Selecionar JAR original"),101,0,FALSE);
 const WCHAR *tabs[]={tr(L"QUICK SETUP",L"INÍCIO"),tr(L"DISPLAY",L"IMAGEM"),tr(L"CONTROLS",L"CONTROLES"),tr(L"SYSTEM",L"SISTEMA")};
 for(int i=0;i<4;i++)ui_button(dc,(RECT){32+i*148,264,176+i*148,302},tabs[i],10,i,ui_tab==i);
 int y=327;const WCHAR *yesno[]={tr(L"Off",L"Não"),tr(L"On",L"Sim")};
 if(ui_tab==0){
  ui_text(dc,tr(L"Choose how your game looks",L"Escolha o visual do jogo"),(RECT){36,y,width-36,y+30},23,FW_SEMIBOLD,UI_TEXT,DT_SINGLELINE);
  StringCchPrintfW(text,ARRAYSIZE(text),tr(L"Original game: %s. Presentation settings scale it to the window.",L"Resolução original: %s. As opções ajustam a apresentação na janela."),resolution);
  ui_text(dc,text,(RECT){36,y+37,width-36,y+65},14,FW_NORMAL,UI_MUTED,DT_SINGLELINE|DT_END_ELLIPSIS);
  const WCHAR *names[]={tr(L"SHARP PIXELS",L"PIXELS NÍTIDOS"),tr(L"SMOOTH",L"SUAVE"),L"SCALE2X"};
  const WCHAR *details[]={tr(L"Original pixel edges",L"Contornos dos pixels originais"),tr(L"Soft bilinear scaling",L"Ampliação bilinear suave"),tr(L"Pixel art edge filter",L"Filtro de contornos para pixel art")};
  int card=(width-88)/3;
  for(int i=0;i<3;i++){
   RECT r={36+i*(card+8),y+89,36+i*(card+8)+card,y+189};ui_button(dc,r,L"",20,i,ui_settings.filter==i);
   ui_text(dc,names[i],(RECT){r.left+16,r.top+18,r.right-10,r.top+47},18,FW_BOLD,UI_TEXT,DT_SINGLELINE);
   ui_text(dc,details[i],(RECT){r.left+16,r.top+58,r.right-10,r.bottom-8},13,FW_NORMAL,UI_TEXT,DT_WORDBREAK);
  }
  ui_text(dc,tr(L"Full HD window by default. Open Display for resolution, border and fullscreen.",L"Janela Full HD por padrão. Em Imagem, ajuste resolução, moldura e tela cheia."),(RECT){36,y+214,width-36,y+262},14,FW_NORMAL,UI_MUTED,DT_WORDBREAK);
 }else if(ui_tab==1){
  const WCHAR *res[]={L"960 x 540",L"1080p",L"1440p",L"3K",L"4K"};
  const WCHAR *modes[]={tr(L"Window",L"Janela"),tr(L"Fullscreen",L"Tela cheia")};const WCHAR *filters[]={tr(L"Sharp",L"Nítido"),tr(L"Smooth",L"Suave"),L"Scale2x"};
  ui_row(dc,width,y,tr(L"Resolution",L"Resolução"),tr(L"Window size is limited to your screen; this does not add sprite detail.",L"A janela cabe na tela. A ampliação não cria detalhes nos sprites."),res,5,ui_settings.resolution,21);
  ui_row(dc,width,y+76,tr(L"Display mode",L"Modo de exibição"),tr(L"Windowed or fullscreen presentation.",L"Apresentação em janela ou tela cheia."),modes,2,ui_settings.fullscreen,22);
  ui_row(dc,width,y+152,tr(L"Image filter",L"Filtro de imagem"),tr(L"The same filters available from Select / F2 during the game.",L"Os mesmos filtros disponíveis no jogo por Select / F2."),filters,3,ui_settings.filter,20);
  ui_row(dc,width,y+228,tr(L"Integer scaling",L"Escala inteira"),tr(L"On: whole pixel multiples. Off: fill the available height.",L"Sim: múltiplos inteiros de pixels. Não: usa toda a altura disponível."),yesno,2,ui_settings.integer,23);
  ui_row(dc,width,y+304,tr(L"Illustrated border",L"Moldura ilustrada"),tr(L"Use the supplied artwork around the original game screen.",L"Usa a arte fornecida ao redor da tela original do jogo."),yesno,2,ui_settings.bezel,24);
 }else if(ui_tab==2){
  const WCHAR *labels[]={tr(L"Navigate",L"Navegar"),tr(L"Confirm / launch blocks",L"Confirmar / lançar blocos"),tr(L"Pause / play",L"Pausar / jogar"),tr(L"Game settings",L"Configurações no jogo"),tr(L"Launcher categories",L"Abas do launcher")};
  const WCHAR *keys[]={tr(L"D-pad / left stick  |  Arrow keys",L"Direcional / analógico esquerdo  |  Setas"),L"A  |  Enter",L"Start  |  Esc (game) / Enter (launcher)",L"Select / Back  |  F2",L"LB / RB  |  Q / E"};
  for(int i=0;i<5;i++){ui_text(dc,labels[i],(RECT){36,y+i*51,width/2,y+i*51+26},16,FW_SEMIBOLD,UI_TEXT,DT_SINGLELINE);ui_text(dc,keys[i],(RECT){width/2,y+i*51,width-36,y+i*51+33},14,FW_NORMAL,UI_MUTED,DT_WORDBREAK);}
  ui_text(dc,tr(L"Launcher: XInput-compatible pads. PlayStation controllers need a compatible mapping.\nPhysical device compatibility still needs individual testing.",L"Launcher: controles compatíveis com XInput. PlayStation precisa de mapeamento compatível.\nCada dispositivo físico ainda precisa de teste individual."),(RECT){36,y+278,width-36,y+344},14,FW_NORMAL,UI_MUTED,DT_WORDBREAK);
 }else{
  const WCHAR *langs[]={tr(L"Windows",L"Windows"),L"English",L"Português (BR)"};
  ui_row(dc,width,y,tr(L"Language",L"Idioma"),tr(L"Launcher and settings only. Original game text is unchanged.",L"Idioma do launcher e das opções. Não altera os textos do jogo."),langs,3,ui_settings.language,25);
  StringCchPrintfW(text,ARRAYSIZE(text),tr(L"Required game: %s / %s / %llu bytes",L"Jogo necessário: %s / %s / %llu bytes"),version,resolution,expected_size);
  ui_text(dc,text,(RECT){36,y+89,width-36,y+118},16,FW_SEMIBOLD,UI_TEXT,DT_SINGLELINE|DT_END_ELLIPSIS);
  ui_text(dc,reference,(RECT){36,y+125,width-36,y+150},14,FW_NORMAL,UI_MUTED,DT_SINGLELINE);
  ui_text(dc,tr(L"Verified locally with SHA-256. No game download and no automatic telemetry.\nLogs stay in app/logs for you to review and share.",L"Validado localmente por SHA-256. Sem download do jogo e sem telemetria automática.\nOs logs ficam em app/logs para você revisar e compartilhar."),(RECT){36,y+167,width-36,y+229},14,FW_NORMAL,UI_MUTED,DT_WORDBREAK);
  ui_button(dc,(RECT){36,y+249,256,y+290},tr(L"Open logs folder",L"Abrir pasta de logs"),102,0,FALSE);
 }
 int footer=height-110;ui_fill(dc,(RECT){0,footer,width,height},UI_PANEL);ui_fill(dc,(RECT){0,footer,width,footer+1},UI_LINE);
 ui_text(dc,*ui_notice?ui_notice:ui_ready?tr(L"Ready to play",L"Pronto para jogar"):tr(L"Select your original JAR to begin",L"Selecione seu JAR original para começar"),(RECT){36,footer+18,width-270,footer+46},16,FW_SEMIBOLD,UI_TEXT,DT_SINGLELINE|DT_END_ELLIPSIS);
 ui_text(dc,tr(L"Local settings  /  Native C launcher",L"Preferências locais  /  Launcher nativo em C"),(RECT){36,footer+50,width-270,footer+74},12,FW_NORMAL,UI_MUTED,DT_SINGLELINE);
 ui_button(dc,(RECT){width-246,footer+17,width-36,footer+72},ui_reshade?tr(L"PLAY + RESHADE",L"JOGAR + RESHADE"):tr(L"PLAY",L"JOGAR"),103,0,TRUE);
 ui_text(dc,tr(L"A / Enter: select     Arrows: navigate     LB / RB or Q / E: category     Start: play",L"A / Enter: selecionar     Setas: navegar     LB / RB ou Q / E: abas     Start: jogar"),(RECT){36,height-29,width-36,height-7},12,FW_NORMAL,UI_MUTED,DT_SINGLELINE|DT_END_ELLIPSIS);
 SetMapMode(dc,MM_TEXT);BitBlt(dest,0,0,physical_w,physical_h,dc,0,0,SRCCOPY);SelectObject(dc,old);DeleteObject(buffer);DeleteDC(dc);
}
static DWORD WINAPI ui_game_thread(void *unused){(void)unused;DWORD code=start_game(FALSE,ui_reshade,FALSE);PostMessageW(ui_window,UI_DONE,code,0);return code;}
static void ui_activate(int action,int value){
 if(ui_running)return;
 if(action==10){ui_tab=value;ui_focus=2+value;}
 else if(action==20)ui_settings.filter=value;
 else if(action==21)ui_settings.resolution=value;
 else if(action==22)ui_settings.fullscreen=value;
 else if(action==23)ui_settings.integer=value;
 else if(action==24)ui_settings.bezel=value;
 else if(action==25){ui_settings.language=value;portuguese=value==2||(value==0&&PRIMARYLANGID(GetUserDefaultUILanguage())==LANG_PORTUGUESE);}
 else if(action==100)ShellExecuteW(ui_window,L"open",L"https://github.com/piecarlos/legacy-mobile-forge",NULL,NULL,SW_SHOWNORMAL);
 else if(action==102)open_logs();
 else if(action==101||(action==103&&!ui_ready)){
  HRESULT hr=choose_file(FALSE);ui_ready=valid_game(target);
  if(FAILED(hr)&&hr!=HRESULT_FROM_WIN32(ERROR_CANCELLED))StringCchCopyW(ui_notice,ARRAYSIZE(ui_notice),tr(L"Import failed. Check the exact required game version under System.",L"Falha na importação. Confira a versão necessária em Sistema."));
  else *ui_notice=0;
 }else if(action==103){
  if(!valid_game(target)){ui_ready=FALSE;goto redraw;}
  WCHAR dll[PATH_CAP];if(ui_reshade&&(!joined(dll,root,L"d3d9.dll")||GetFileAttributesW(dll)==INVALID_FILE_ATTRIBUTES)){StringCchCopyW(ui_notice,ARRAYSIZE(ui_notice),tr(L"ReShade DLL missing. Use the normal launcher or install the official DLL.",L"DLL do ReShade ausente. Use o launcher normal ou instale a DLL oficial."));goto redraw;}
  if(!ui_save_settings()){StringCchCopyW(ui_notice,ARRAYSIZE(ui_notice),tr(L"Cannot save settings. Extract into a writable folder.",L"Não foi possível salvar. Extraia em uma pasta gravável."));goto redraw;}
  ui_running=TRUE;ui_worker=CreateThread(NULL,0,ui_game_thread,NULL,0,NULL);
  if(ui_worker){ShowWindow(ui_window,SW_HIDE);event("ui.play",0);}else{ui_running=FALSE;StringCchCopyW(ui_notice,ARRAYSIZE(ui_notice),tr(L"Could not start game process.",L"Não foi possível iniciar o processo do jogo."));}
 }
redraw:
 InvalidateRect(ui_window,NULL,FALSE);
}
static void ui_move(int delta){if(ui_count)ui_focus=(ui_focus+ui_count+delta)%ui_count;InvalidateRect(ui_window,NULL,FALSE);}
static void ui_category(int delta){ui_tab=(ui_tab+4+delta)%4;ui_focus=2+ui_tab;InvalidateRect(ui_window,NULL,FALSE);}
static void ui_poll_pad(void){
 if(!ui_get_state||ui_running||GetForegroundWindow()!=ui_window){ui_last_buttons=0;ui_last_axis=0;return;}
 XINPUT_STATE state={0};BOOL connected=FALSE;for(DWORD slot=0;slot<4;slot++)if(ui_get_state(slot,&state)==ERROR_SUCCESS){connected=TRUE;break;}
 if(!connected){ui_last_buttons=0;ui_last_axis=0;return;}
 WORD buttons=state.Gamepad.wButtons,pressed=buttons&~ui_last_buttons;ui_last_buttons=buttons;
 if(pressed&XINPUT_GAMEPAD_LEFT_SHOULDER)ui_category(-1);if(pressed&XINPUT_GAMEPAD_RIGHT_SHOULDER)ui_category(1);
 if(pressed&XINPUT_GAMEPAD_START){ui_activate(103,0);return;}
 if(pressed&XINPUT_GAMEPAD_A&&ui_focus<ui_count)ui_activate(ui_hits[ui_focus].action,ui_hits[ui_focus].value);
 if(pressed&XINPUT_GAMEPAD_B)ui_category(-1);
 int axis=buttons&(XINPUT_GAMEPAD_DPAD_RIGHT|XINPUT_GAMEPAD_DPAD_DOWN)?1:buttons&(XINPUT_GAMEPAD_DPAD_LEFT|XINPUT_GAMEPAD_DPAD_UP)?-1:state.Gamepad.sThumbLX>16000||state.Gamepad.sThumbLY< -16000?1:state.Gamepad.sThumbLX< -16000||state.Gamepad.sThumbLY>16000?-1:0;
 DWORD now=GetTickCount();if(axis&&(axis!=ui_last_axis||(LONG)(now-ui_repeat_at)>=0)){ui_move(axis);ui_repeat_at=now+(axis!=ui_last_axis?350:110);}ui_last_axis=axis;
}
static LRESULT CALLBACK ui_proc(HWND window,UINT message,WPARAM wp,LPARAM lp){
 switch(message){
 case WM_ERASEBKGND:return 1;
 case WM_PAINT:{PAINTSTRUCT ps;HDC dc=BeginPaint(window,&ps);RECT r;GetClientRect(window,&r);ui_paint(dc,r.right,r.bottom);EndPaint(window,&ps);return 0;}
 case WM_PRINTCLIENT:{RECT r;GetClientRect(window,&r);ui_paint((HDC)wp,r.right,r.bottom);return 0;}
 case WM_GETMINMAXINFO:((MINMAXINFO*)lp)->ptMinTrackSize=(POINT){720,560};return 0;
 case WM_SIZE:InvalidateRect(window,NULL,FALSE);return 0;
 case WM_LBUTTONDOWN:{RECT r;GetClientRect(window,&r);POINT p={MulDiv(GET_X_LPARAM(lp),1200,max(1,r.right)),MulDiv(GET_Y_LPARAM(lp),850,max(1,r.bottom))};for(int i=0;i<ui_count;i++)if(PtInRect(&ui_hits[i].box,p)){ui_focus=i;ui_activate(ui_hits[i].action,ui_hits[i].value);break;}return 0;}
 case WM_KEYDOWN:
  if(wp==VK_LEFT||wp==VK_UP)ui_move(-1);else if(wp==VK_RIGHT||wp==VK_DOWN||wp==VK_TAB)ui_move((GetKeyState(VK_SHIFT)&0x8000)?-1:1);
  else if(wp=='Q')ui_category(-1);else if(wp=='E')ui_category(1);else if(wp==VK_RETURN||wp==VK_SPACE){if(ui_focus<ui_count)ui_activate(ui_hits[ui_focus].action,ui_hits[ui_focus].value);}
  else if(wp==VK_ESCAPE)SendMessageW(window,WM_CLOSE,0,0);return 0;
 case WM_TIMER:ui_poll_pad();return 0;
 case UI_DONE:
  if(ui_worker){CloseHandle(ui_worker);ui_worker=NULL;}ui_running=FALSE;ui_read_settings();ui_ready=valid_game(target);ui_last_buttons=0;
  StringCchCopyW(ui_notice,ARRAYSIZE(ui_notice),wp?tr(L"Game closed with an error. See local logs.",L"O jogo encerrou com erro. Consulte os logs locais."):tr(L"Game closed. Ready to play again.",L"Jogo encerrado. Pronto para jogar novamente."));
  ShowWindow(window,SW_SHOW);SetForegroundWindow(window);InvalidateRect(window,NULL,FALSE);event("ui.return",(DWORD)wp);return 0;
 case WM_CLOSE:if(!ui_running){if(!ui_save_settings())MessageBoxW(window,tr(L"Settings could not be saved. Check folder permissions.",L"As opções não foram salvas. Confira as permissões da pasta."),title,MB_OK|MB_ICONERROR);DestroyWindow(window);}return 0;
 case WM_DESTROY:KillTimer(window,1);PostQuitMessage(0);return 0;
 }return DefWindowProcW(window,message,wp,lp);
}
static DWORD run_launcher_ui(HINSTANCE instance,BOOL reshade){
 ui_reshade=reshade;ui_ready=valid_game(target);ui_focus=6;ui_read_settings();ui_load_art();
 ui_xinput=LoadLibraryExW(L"xinput1_4.dll",NULL,LOAD_LIBRARY_SEARCH_SYSTEM32);if(ui_xinput)ui_get_state=(UiGetState)(void*)GetProcAddress(ui_xinput,"XInputGetState");
 WNDCLASSW cls={0};cls.lpfnWndProc=ui_proc;cls.hInstance=instance;cls.hCursor=LoadCursorW(NULL,IDC_ARROW);cls.lpszClassName=L"LegacyMobileForgeLauncher";cls.hIcon=LoadIconW(NULL,IDI_APPLICATION);RegisterClassW(&cls);
 RECT work;SystemParametersInfoW(SPI_GETWORKAREA,0,&work,0);int width=min(1200,work.right-work.left),height=min(910,work.bottom-work.top);
 ui_window=CreateWindowExW(0,cls.lpszClassName,title,WS_OVERLAPPEDWINDOW,work.left+(work.right-work.left-width)/2,work.top+(work.bottom-work.top-height)/2,width,height,NULL,NULL,instance,NULL);
 if(!ui_window){event("ui.failed",GetLastError());if(ui_art)DeleteObject(ui_art);if(ui_xinput)FreeLibrary(ui_xinput);return 1;}
 picker_owner=ui_window;BOOL dark=TRUE;DwmSetWindowAttribute(ui_window,20,&dark,sizeof(dark));
 SetTimer(ui_window,1,30,NULL);ShowWindow(ui_window,SW_SHOW);UpdateWindow(ui_window);event("ui.open",0);
 MSG message;while(GetMessageW(&message,NULL,0,0)>0){TranslateMessage(&message);DispatchMessageW(&message);}
 if(ui_art)DeleteObject(ui_art);if(ui_xinput)FreeLibrary(ui_xinput);return 0;
}
