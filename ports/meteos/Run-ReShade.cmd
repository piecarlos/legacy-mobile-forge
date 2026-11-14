@echo off
setlocal
cd /d "%~dp0"
if not exist "d3d9.dll" (
 echo ReShade is optional. Obtain the official 64-bit ReShade DLL and place it here as d3d9.dll.
 echo See README.md. The normal MeteosPreview.exe does not require ReShade.
 pause
 exit /b 1
)
set "ENTRY=MeteosDesktop"
if exist "MeteosLauncher.exe" set "ENTRY=MeteosLauncher"
"runtime\bin\javaw.exe" --enable-native-access=ALL-UNNAMED -Dsun.java2d.d3d=true "-Dforge.reshade.library=%~dp0d3d9.dll" "-Dreshade.library=%~dp0d3d9.dll" -Dforge.entryClass=%ENTRY% -cp "app/meteos-preview.jar;app/tools/freej2me/freej2me_plus.jar" ReshadeBootstrap
