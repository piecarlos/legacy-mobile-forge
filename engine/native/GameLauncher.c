#define COBJMACROS
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0A00
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <commctrl.h>
#include <bcrypt.h>
#include <strsafe.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#define PATH_CAP 32768
static WCHAR root[PATH_CAP], app[PATH_CAP], target[PATH_CAP], logs[PATH_CAP];
static WCHAR title[256], version[80], resolution[80], reference[256], expected[80];
static WCHAR entry_class[256], classpath[2048];
static ULONGLONG expected_size;
static BOOL portuguese;
static const WCHAR *tr(const WCHAR *en,const WCHAR *pt){return portuguese?pt:en;}

static BOOL joined(WCHAR *out,const WCHAR *base,const WCHAR *relative){return SUCCEEDED(StringCchPrintfW(out,PATH_CAP,L"%s\\%s",base,relative));}
static BOOL safe_relative(const WCHAR *p){return *p && *p!=L'\\' && *p!=L'/' && !wcschr(p,L':') && !wcsstr(p,L"..");}
static BOOL make_directory(const WCHAR *path){return CreateDirectoryW(path,NULL)||GetLastError()==ERROR_ALREADY_EXISTS;}
static void event(const char *kind,DWORD code){
 WCHAR file[PATH_CAP],previous[PATH_CAP];SYSTEMTIME t;HANDLE h;DWORD count;char line[200];LARGE_INTEGER size;
 if(!make_directory(logs)||!joined(file,logs,L"launcher.log")||!joined(previous,logs,L"launcher.previous.log"))return;
 h=CreateFileW(file,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
 if(h!=INVALID_HANDLE_VALUE){if(GetFileSizeEx(h,&size)&&size.QuadPart>1024*1024){CloseHandle(h);MoveFileExW(file,previous,MOVEFILE_REPLACE_EXISTING);}else CloseHandle(h);}
 h=CreateFileW(file,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
 if(h==INVALID_HANDLE_VALUE)return;
 GetSystemTime(&t);int length=sprintf_s(line,sizeof(line),"%04u-%02u-%02uT%02u:%02u:%02uZ native.%s code=%lu\r\n",t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond,kind,code);
 if(length>0)WriteFile(h,line,(DWORD)length,&count,NULL);CloseHandle(h);
}
static BOOL valid_game(const WCHAR *file){
 HANDLE f=CreateFileW(file,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,NULL);
 BCRYPT_ALG_HANDLE algorithm=NULL;BCRYPT_HASH_HANDLE hash=NULL;LARGE_INTEGER size;BOOL good=FALSE;BYTE buffer[16384],digest[32];DWORD n;WCHAR hex[65];
 if(f==INVALID_HANDLE_VALUE)return FALSE;
 if(!GetFileSizeEx(f,&size)||(ULONGLONG)size.QuadPart!=expected_size)goto done;
 if(BCryptOpenAlgorithmProvider(&algorithm,BCRYPT_SHA256_ALGORITHM,NULL,0)<0||BCryptCreateHash(algorithm,&hash,NULL,0,NULL,0,0)<0)goto done;
 for(;;){if(!ReadFile(f,buffer,sizeof(buffer),&n,NULL))goto done;if(!n)break;if(BCryptHashData(hash,buffer,n,0)<0)goto done;}
 if(BCryptFinishHash(hash,digest,sizeof(digest),0)<0)goto done;
 for(int i=0;i<32;i++){static const WCHAR digits[]=L"0123456789abcdef";hex[i*2]=digits[digest[i]>>4];hex[i*2+1]=digits[digest[i]&15];}hex[64]=0;
 good=_wcsicmp(hex,expected)==0;
done:
 if(hash)BCryptDestroyHash(hash);if(algorithm)BCryptCloseAlgorithmProvider(algorithm,0);CloseHandle(f);return good;
}
static BOOL import_game(const WCHAR *source){
 WCHAR parent[PATH_CAP],temporary[PATH_CAP];
 if(!valid_game(source)){SetLastError(ERROR_INVALID_DATA);event("import.rejected",ERROR_INVALID_DATA);return FALSE;}
 if(valid_game(target)){event("import.cached",0);return TRUE;}
 if(FAILED(StringCchCopyW(parent,PATH_CAP,target)))return FALSE;
 WCHAR *last=wcsrchr(parent,L'\\');if(!last)return FALSE;*last=0;
 /* Every parent is constructed below app/, never from the selected source path. */
 for(WCHAR *p=parent+wcslen(app)+1;*p;p++){if(*p==L'\\'){*p=0;BOOL ok=make_directory(parent);*p=L'\\';if(!ok)return FALSE;}}
 if(!make_directory(parent)||!GetTempFileNameW(parent,L"lmf",0,temporary))return FALSE;
 BOOL ok=CopyFileW(source,temporary,FALSE)&&valid_game(temporary)&&MoveFileExW(temporary,target,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
 DWORD error=ok?0:GetLastError();DeleteFileW(temporary);event(ok?"import.accepted":"import.failed",error);SetLastError(error);return ok;
}
static void open_logs(void){ShellExecuteW(NULL,L"open",logs,NULL,NULL,SW_SHOWNORMAL);}
static int dialog(BOOL error){
 TASKDIALOGCONFIG config={0};WCHAR content[1200],requirements[1200];int selected=IDCANCEL;
 TASKDIALOG_BUTTON buttons[]={{100,tr(L"Select game JAR\nChoose your original game file",L"Selecionar JAR do jogo\nEscolha seu arquivo original")},{101,tr(L"Open logs folder",L"Abrir pasta de logs")},{IDCANCEL,tr(L"Exit",L"Sair")}};
 StringCchPrintfW(content,1200,tr(L"%s\nVersion %s  ·  %s\n\nThe original game is not included or downloaded.\nYour verified file is copied once and stays on this PC.",L"%s\nVersão %s  ·  %s\n\nO jogo original não acompanha o launcher e não será baixado.\nO arquivo validado será copiado uma vez e ficará neste PC."),title,version,resolution);
 StringCchPrintfW(requirements,1200,L"%s\n%llu bytes\nSHA-256: %s",reference,expected_size,expected);
 config.cbSize=sizeof(config);config.dwFlags=TDF_ALLOW_DIALOG_CANCELLATION|TDF_USE_COMMAND_LINKS|TDF_SIZE_TO_CONTENT;
 config.pszWindowTitle=title;config.pszMainIcon=error?TD_ERROR_ICON:TD_INFORMATION_ICON;
 config.pszMainInstruction=error?tr(L"This file could not be imported",L"Não foi possível importar este arquivo"):tr(L"Select the original game to start",L"Selecione o jogo original para começar");
 config.pszContent=content;config.pszExpandedInformation=requirements;config.pszExpandedControlText=tr(L"File requirements",L"Requisitos do arquivo");
 config.pszFooter=error?tr(L"Use the supported version and a writable extracted folder.",L"Use a versão compatível e extraia o launcher em uma pasta gravável."):tr(L"English / Português: follows your saved language or Windows.",L"English / Português: segue o idioma salvo ou do Windows.");
 config.cButtons=ARRAYSIZE(buttons);config.pButtons=buttons;config.nDefaultButton=100;
 HRESULT result=TaskDialogIndirect(&config,&selected,NULL,NULL);if(FAILED(result)){event("dialog.failed",(DWORD)result);return IDCANCEL;}return selected;
}
static HRESULT choose_file(BOOL probe){
 IFileOpenDialog *picker=NULL;IShellItem *item=NULL;PWSTR file=NULL;FILEOPENDIALOGOPTIONS options=0;
 COMDLG_FILTERSPEC filter={L"Java ME (*.jar)",L"*.jar"};
 HRESULT hr=CoCreateInstance(&CLSID_FileOpenDialog,NULL,CLSCTX_INPROC_SERVER,&IID_IFileOpenDialog,(void**)&picker);
 if(FAILED(hr))return hr;
 hr=IFileOpenDialog_GetOptions(picker,&options);
 if(SUCCEEDED(hr))hr=IFileOpenDialog_SetOptions(picker,options|FOS_FORCEFILESYSTEM|FOS_FILEMUSTEXIST|FOS_PATHMUSTEXIST|FOS_NOCHANGEDIR);
 if(SUCCEEDED(hr))hr=IFileOpenDialog_SetFileTypes(picker,1,&filter);
 if(SUCCEEDED(hr))hr=IFileOpenDialog_SetTitle(picker,tr(L"Select your original game JAR",L"Selecione seu JAR original do jogo"));
 if(!probe&&SUCCEEDED(hr))hr=IFileOpenDialog_Show(picker,NULL);
 if(!probe&&SUCCEEDED(hr))hr=IFileOpenDialog_GetResult(picker,&item);
 if(item&&SUCCEEDED(hr))hr=IShellItem_GetDisplayName(item,SIGDN_FILESYSPATH,&file);
 if(file&&SUCCEEDED(hr)&&!import_game(file))hr=HRESULT_FROM_WIN32(GetLastError());
 if(file)CoTaskMemFree(file);if(item)IShellItem_Release(item);IFileOpenDialog_Release(picker);return hr;
}
static BOOL load_profile(void){
 WCHAR profile[PATH_CAP],relative[PATH_CAP],bytes[80],properties[PATH_CAP],backend[32];
 DWORD count=GetModuleFileNameW(NULL,root,PATH_CAP);if(!count||count>=PATH_CAP)return FALSE;
 WCHAR *slash=wcsrchr(root,L'\\');if(!slash)return FALSE;*slash=0;
 if(!joined(app,root,L"app")||!joined(profile,app,L"launcher.ini")||!joined(logs,app,L"logs"))return FALSE;
 if(GetFileAttributesW(profile)==INVALID_FILE_ATTRIBUTES)return FALSE;
 GetPrivateProfileStringW(L"launcher",L"title",L"Legacy Mobile Forge",title,ARRAYSIZE(title),profile);
 GetPrivateProfileStringW(L"launcher",L"game_version",L"",version,ARRAYSIZE(version),profile);
 GetPrivateProfileStringW(L"launcher",L"resolution",L"",resolution,ARRAYSIZE(resolution),profile);
 GetPrivateProfileStringW(L"launcher",L"filename",L"",reference,ARRAYSIZE(reference),profile);
 GetPrivateProfileStringW(L"launcher",L"sha256",L"",expected,ARRAYSIZE(expected),profile);
 GetPrivateProfileStringW(L"launcher",L"size",L"0",bytes,ARRAYSIZE(bytes),profile);expected_size=_wcstoui64(bytes,NULL,10);
 GetPrivateProfileStringW(L"launcher",L"game_path",L"",relative,PATH_CAP,profile);
 GetPrivateProfileStringW(L"launcher",L"backend",L"java",backend,ARRAYSIZE(backend),profile);
 GetPrivateProfileStringW(L"launcher",L"entry_class",L"MeteosBackend",entry_class,ARRAYSIZE(entry_class),profile);
 GetPrivateProfileStringW(L"launcher",L"classpath",L"app\\meteos-preview.jar;app\\tools\\freej2me\\freej2me_plus.jar",classpath,ARRAYSIZE(classpath),profile);
 if(wcscmp(backend,L"java")||!safe_relative(relative)||!joined(target,app,relative)||!expected_size||wcslen(expected)!=64)return FALSE;
 if(!safe_relative(classpath)||!*entry_class)return FALSE;
 for(const WCHAR *c=entry_class;*c;c++)if(!((*c>=L'a'&&*c<=L'z')||(*c>=L'A'&&*c<=L'Z')||(*c>=L'0'&&*c<=L'9')||*c==L'.'||*c==L'_'||*c==L'$'))return FALSE;
 for(int i=0;i<64;i++)if(!((expected[i]>=L'0'&&expected[i]<=L'9')||(expected[i]>=L'a'&&expected[i]<=L'f')||(expected[i]>=L'A'&&expected[i]<=L'F')))return FALSE;
 portuguese=PRIMARYLANGID(GetUserDefaultUILanguage())==LANG_PORTUGUESE;
 if(joined(properties,app,L"video.properties")){
  FILE *f=NULL;if(!_wfopen_s(&f,properties,L"rb")&&f){char line[1024];while(fgets(line,sizeof(line),f)){if(!strncmp(line,"language=en",11))portuguese=FALSE;else if(!strncmp(line,"language=pt-BR",14))portuguese=TRUE;}fclose(f);}
 }
 return TRUE;
}
static BOOL argument(WCHAR *command,size_t capacity,const WCHAR *text){
 if(FAILED(StringCchCatW(command,capacity,*command?L" \"":L"\"")))return FALSE;
 size_t slashes=0;
 for(const WCHAR *p=text;;p++){
  if(*p==L'\\'){slashes++;continue;}
  size_t n=(*p==L'\"'||!*p)?slashes*2:slashes;
  while(n--)if(FAILED(StringCchCatW(command,capacity,L"\\")))return FALSE;
  slashes=0;if(!*p)break;if(*p==L'\"'&&FAILED(StringCchCatW(command,capacity,L"\\")))return FALSE;
  WCHAR one[2]={*p,0};if(FAILED(StringCchCatW(command,capacity,one)))return FALSE;
 }
 return SUCCEEDED(StringCchCatW(command,capacity,L"\""));
}
static DWORD start_game(BOOL smoke,BOOL reshade,BOOL backend_validate){
 WCHAR executable[PATH_CAP],library[PATH_CAP];WCHAR *command=calloc(PATH_CAP,sizeof(WCHAR));DWORD result=1;STARTUPINFOW startup={0};PROCESS_INFORMATION process={0};
 if(!command)return 1;
 if(!joined(executable,root,(smoke||backend_validate)?L"runtime\\bin\\java.exe":L"runtime\\bin\\javaw.exe"))goto done;
 if(GetFileAttributesW(executable)==INVALID_FILE_ATTRIBUTES){event("runtime.missing",ERROR_FILE_NOT_FOUND);goto done;}
 if(!argument(command,PATH_CAP,executable)||!argument(command,PATH_CAP,L"-Djpackage.app-version=0.2.2"))goto done;
 if(reshade){
  if(!joined(library,root,L"d3d9.dll")||GetFileAttributesW(library)==INVALID_FILE_ATTRIBUTES){event("reshade.missing",ERROR_FILE_NOT_FOUND);goto done;}
  if(!argument(command,PATH_CAP,L"--enable-native-access=ALL-UNNAMED")||!argument(command,PATH_CAP,L"-Dsun.java2d.d3d=true"))goto done;
  WCHAR *option=calloc(PATH_CAP,sizeof(WCHAR));if(!option)goto done;
  StringCchPrintfW(option,PATH_CAP,L"-Dforge.reshade.library=%s",library);BOOL ok=argument(command,PATH_CAP,option);
  StringCchPrintfW(option,PATH_CAP,L"-Dreshade.library=%s",library);ok=ok&&argument(command,PATH_CAP,option);free(option);
  WCHAR entry_option[300];StringCchPrintfW(entry_option,ARRAYSIZE(entry_option),L"-Dforge.entryClass=%s",entry_class);
  if(!ok||!argument(command,PATH_CAP,entry_option))goto done;
 }
 if(!argument(command,PATH_CAP,L"-cp")||!argument(command,PATH_CAP,classpath)||!argument(command,PATH_CAP,reshade?L"ReshadeBootstrap":entry_class))goto done;
 if(smoke&&!argument(command,PATH_CAP,L"--smoke"))goto done;
 if(backend_validate&&!argument(command,PATH_CAP,L"--validate"))goto done;
 startup.cb=sizeof(startup);event("backend.start",0);
 if(!CreateProcessW(executable,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,root,&startup,&process)){event("backend.failed",GetLastError());goto done;}
 WaitForSingleObject(process.hProcess,INFINITE);GetExitCodeProcess(process.hProcess,&result);CloseHandle(process.hThread);CloseHandle(process.hProcess);event("backend.exit",result);
done:
 free(command);return result;
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE previous,PWSTR command_line,int show){
 (void)instance;(void)previous;(void)command_line;(void)show;
 SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
 int argc=0;LPWSTR *argv=CommandLineToArgvW(GetCommandLineW(),&argc);if(!argv)return 1;
 BOOL validate=FALSE,import_only=FALSE,smoke=FALSE,reshade=FALSE,probe=FALSE,backend_validate=FALSE;const WCHAR *source=NULL;DWORD result=1;
 if(!load_profile()){MessageBoxW(NULL,L"Extract the entire launcher ZIP into a writable folder. Missing or invalid app/launcher.ini.",L"Legacy Mobile Forge",MB_OK|MB_ICONERROR);goto done;}
 event("start",0);
 for(int i=1;i<argc;i++){
  if(!wcscmp(argv[i],L"--import")&&i+1<argc)source=argv[++i];
  else if(!wcscmp(argv[i],L"--validate"))validate=TRUE;
  else if(!wcscmp(argv[i],L"--import-only"))import_only=TRUE;
  else if(!wcscmp(argv[i],L"--smoke"))smoke=TRUE;
  else if(!wcscmp(argv[i],L"--reshade"))reshade=TRUE;
  else if(!wcscmp(argv[i],L"--probe-dialog"))probe=TRUE;
  else if(!wcscmp(argv[i],L"--backend-validate"))backend_validate=TRUE;
  else {event("arguments.invalid",ERROR_INVALID_PARAMETER);goto done;}
 }
 if(source&&!import_game(source))goto done;
 if(validate||import_only){result=valid_game(target)?0:1;event("validation",result);goto done;}
 HRESULT initialized=CoInitializeEx(NULL,COINIT_APARTMENTTHREADED|COINIT_DISABLE_OLE1DDE);
 if(FAILED(initialized)){event("com.failed",(DWORD)initialized);goto done;}
 if(probe){HRESULT hr=choose_file(TRUE);event("dialog.probe",(DWORD)hr);result=SUCCEEDED(hr)?0:1;CoUninitialize();goto done;}
 if(!valid_game(target)){
  if(smoke||backend_validate){CoUninitialize();goto done;}
  BOOL error=FALSE;
  while(!valid_game(target)){
   int button=dialog(error);if(button==IDCANCEL){result=0;CoUninitialize();goto done;}if(button==101){open_logs();continue;}
   HRESULT chosen=choose_file(FALSE);error=FAILED(chosen)&&chosen!=HRESULT_FROM_WIN32(ERROR_CANCELLED);if(error)event("selection.failed",(DWORD)chosen);
  }
 }
 CoUninitialize();result=start_game(smoke,reshade,backend_validate);
 if(result&&!smoke&&!backend_validate){
  WCHAR dll[PATH_CAP];BOOL missing=reshade&&joined(dll,root,L"d3d9.dll")&&GetFileAttributesW(dll)==INVALID_FILE_ATTRIBUTES;
  TaskDialog(NULL,NULL,title,missing?tr(L"ReShade is not installed",L"ReShade não está instalado"):tr(L"Could not start the game",L"Não foi possível iniciar o jogo"),missing?tr(L"Obtain the official 64-bit ReShade DLL as d3d9.dll beside the launcher, or open the normal executable. Nothing is downloaded automatically.",L"Obtenha a DLL oficial 64 bits do ReShade como d3d9.dll ao lado do launcher ou abra a versão normal. Nada será baixado automaticamente."):tr(L"Keep the whole extracted folder together. Check app/logs for details.",L"Mantenha todos os arquivos extraídos juntos. Consulte app/logs para detalhes."),TDCBF_CLOSE_BUTTON,TD_ERROR_ICON,NULL);
 }
done:
 LocalFree(argv);return (int)result;
}
