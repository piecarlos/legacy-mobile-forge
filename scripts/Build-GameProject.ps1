[CmdletBinding()]
param([Parameter(Mandatory)][string]$ProjectPath,[string]$JdkPath,[switch]$Package)
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'Toolchain.ps1')
$forgeRoot=Split-Path $PSScriptRoot
$case=(Resolve-Path -LiteralPath $ProjectPath).Path
$gameInfo=Get-Content -LiteralPath (Join-Path $case 'project.json') -Raw|ConvertFrom-Json
if($gameInfo.platform -ne 'j2me' -or [IO.Path]::GetExtension($gameInfo.input) -ne '.jar'){throw 'Build disponível para JAR/J2ME. Classifique outros formatos antes de adaptar.'}
Add-Type -AssemblyName System.IO.Compression.FileSystem
$sourceJar=Join-Path $case "input/$($gameInfo.input)"
$archive=[IO.Compression.ZipFile]::OpenRead($sourceJar)
try{
 $entry=$archive.GetEntry('META-INF/MANIFEST.MF')
 if(!$entry){throw 'JAR sem manifesto de MIDlet; classifique o formato com Craft Anything.'}
 $reader=New-Object IO.StreamReader($entry.Open())
 try{$manifestText=$reader.ReadToEnd()}finally{$reader.Dispose()}
 if($manifestText -notmatch '(?m)^MIDlet-1:\s*.+'){throw 'JAR sem MIDlet-1. Pode não ser um jogo Java ME executável.'}
}finally{$archive.Dispose()}
$jdk=Get-ForgeJdk $JdkPath
$out=Join-Path $case 'output'
$classes=Join-Path $out 'classes';$stage=Join-Path $out 'app-input';$temp=Join-Path $out 'temp'
foreach($dir in @($classes,$stage,$temp,(Join-Path $stage 'game'),(Join-Path $stage 'tools/freej2me'),(Join-Path $stage 'tools/controllers'))){New-Item -ItemType Directory -Path $dir -Force|Out-Null}
$runtimeJar=Join-Path $forgeRoot 'dependencies/freej2me/freej2me_plus.jar'
if(!(Test-Path $runtimeJar)){throw 'Dependências ausentes; execute Setup-Dependencies.ps1.'}
$sources=Get-ChildItem (Join-Path $forgeRoot 'engine/src') -Filter '*.java'|Select-Object -ExpandProperty FullName
Invoke-ForgeTool (Join-Path $jdk 'bin/javac.exe') (@('--release','21','-encoding','UTF-8','-cp',$runtimeJar,'-d',$classes)+$sources)
Copy-Item -LiteralPath (Join-Path $case "input/$($gameInfo.input)") -Destination (Join-Path $stage 'game/game.jar')
Copy-Item -LiteralPath (Join-Path $case 'profile.properties') -Destination $stage
Copy-Item -Path (Join-Path $forgeRoot 'dependencies/freej2me/*') -Destination (Join-Path $stage 'tools/freej2me') -Force
Copy-Item -Path (Join-Path $forgeRoot 'dependencies/controllers/*') -Destination (Join-Path $stage 'tools/controllers') -Force
if(Test-Path (Join-Path $case 'assets/bezel.png')){New-Item -ItemType Directory -Path (Join-Path $stage 'assets') -Force|Out-Null;Copy-Item -LiteralPath (Join-Path $case 'assets/bezel.png') -Destination (Join-Path $stage 'assets/bezel.png')}
$manifest=Join-Path $out 'MANIFEST.MF'
"Manifest-Version: 1.0`r`nMain-Class: MobileGameDesktop`r`nClass-Path: tools/freej2me/freej2me_plus.jar`r`n`r`n"|Set-Content -LiteralPath $manifest -Encoding ascii
Invoke-ForgeTool (Join-Path $jdk 'bin/jar.exe') @("-J-Djava.io.tmpdir=$temp",'--create','--file',(Join-Path $stage 'legacy-mobile.jar'),'--manifest',$manifest,'-C',$classes,'.')
@'
@echo off
cd /d "%~dp0"
javaw -jar legacy-mobile.jar
'@|Set-Content -LiteralPath (Join-Path $stage 'Jogar.cmd') -Encoding ascii
if($Package){
 $runtime=Join-Path $out 'runtime'
 if(!(Test-Path $runtime)){Invoke-ForgeTool (Join-Path $jdk 'bin/jlink.exe') @('--add-modules','java.desktop,java.logging,java.xml,java.naming,java.management,java.scripting,jdk.charsets,jdk.unsupported','--strip-debug','--no-man-pages','--no-header-files','--compress=zip-6','--output',$runtime)}
 $dist=Join-Path $out 'dist';New-Item -ItemType Directory -Path $dist -Force|Out-Null
 $target=Join-Path $dist $gameInfo.id
 if(Test-Path $target){throw 'Pacote já existe. Preserve saves e escolha uma nova saída antes de substituir.'}
 Invoke-ForgeTool (Join-Path $jdk 'bin/jpackage.exe') @('--type','app-image','--input',$stage,'--main-jar','legacy-mobile.jar','--main-class','MobileGameDesktop','--name',$gameInfo.id,'--app-version','0.1.0','--dest',$dist,'--runtime-image',$runtime,'--temp',(Join-Path $temp ('jpackage-'+[guid]::NewGuid())))
 Write-Output "Executável portátil: $target"
}
Write-Output "Build concluído: $stage"
