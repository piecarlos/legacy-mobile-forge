[CmdletBinding()]
param([string]$JdkPath,[string]$OutputPath)
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'Toolchain.ps1')
$root=Split-Path $PSScriptRoot;$jdk=Get-ForgeJdk $JdkPath
if(!$OutputPath){$OutputPath=Join-Path $root ('build/launcher-'+(Get-Date -Format 'yyyyMMdd-HHmmss'))}
if(Test-Path -LiteralPath $OutputPath){throw 'Output exists. Choose a fresh folder to preserve data.'}
$null=New-Item -ItemType Directory -Path $OutputPath
$output=(Resolve-Path -LiteralPath $OutputPath).Path
$classes=Join-Path $output 'classes';$stage=Join-Path $output 'app-input';$temp=Join-Path $output 'temp'
foreach($dir in @($classes,$stage,$temp,(Join-Path $stage 'tools/freej2me'))){$null=New-Item -ItemType Directory -Path $dir -Force}
$backend=Join-Path $root 'dependencies/freej2me/freej2me_plus.jar'
$lock=Get-Content -LiteralPath (Join-Path $root 'dependencies.lock.json') -Raw|ConvertFrom-Json
$package=$lock.packages|Where-Object id -eq 'freej2me'
foreach($file in $package.files){$path=Join-Path $root ('dependencies/'+$file.to);if(!(Test-Path -LiteralPath $path) -or (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -ne $file.sha256){throw 'Run Setup-Dependencies.ps1: backend missing or hash mismatch.'}}
$sources=@(Get-ChildItem (Join-Path $root 'ports/meteos/src') -Filter '*.java'|ForEach-Object FullName)
$sources+=Join-Path $root 'engine/src/ReshadeBootstrap.java'
Invoke-ForgeTool (Join-Path $jdk 'bin/javac.exe') (@('--release','21','-encoding','UTF-8','-cp',$backend,'-d',$classes)+$sources)
foreach($file in $package.files){Copy-Item -LiteralPath (Join-Path $root ('dependencies/'+$file.to)) -Destination (Join-Path $stage ('tools/freej2me/'+$file.from))}
Copy-Item -LiteralPath (Join-Path $root 'dependencies/freej2me/LICENSE') -Destination (Join-Path $stage 'tools/freej2me/LICENSE')
$manifest=Join-Path $output 'MANIFEST.MF'
"Manifest-Version: 1.0`r`nMain-Class: MeteosBackend`r`nClass-Path: tools/freej2me/freej2me_plus.jar`r`n`r`n"|Set-Content -LiteralPath $manifest -Encoding ascii
Invoke-ForgeTool (Join-Path $jdk 'bin/jar.exe') @("-J-Djava.io.tmpdir=$temp",'--create','--file',(Join-Path $stage 'meteos-preview.jar'),'--manifest',$manifest,'-C',$classes,'.')
Copy-Item -LiteralPath (Join-Path $root 'LICENSE') -Destination $stage
$app=Join-Path $output 'dist/MeteosLauncher'
$null=New-Item -ItemType Directory -Path $app -Force
Copy-Item -LiteralPath $stage -Destination (Join-Path $app 'app') -Recurse
$runtime=Join-Path $app 'runtime'
Invoke-ForgeTool (Join-Path $jdk 'bin/jlink.exe') @('--add-modules','java.desktop,java.logging,java.xml,java.naming,java.management,java.scripting,jdk.charsets,jdk.unsupported','--strip-debug','--no-man-pages','--no-header-files','--compress=zip-6','--output',$runtime)
& (Join-Path $PSScriptRoot 'Build-NativeLauncher.ps1') -OutputPath (Join-Path $output 'native/MeteosLauncher.exe')
Copy-Item -LiteralPath (Join-Path $output 'native/MeteosLauncher.exe') -Destination $app
$spec=Get-Content -LiteralPath (Join-Path $root 'ports/meteos/game.json') -Raw|ConvertFrom-Json
@"
[launcher]
backend=java
title=$($spec.title)
game_version=$($spec.midlet_version)
resolution=$($spec.logical_resolution)
filename=$($spec.reference_filename)
size=$($spec.size_bytes)
sha256=$($spec.sha256)
game_path=game\meteos.jar
entry_class=MeteosBackend
classpath=app\meteos-preview.jar;app\tools\freej2me\freej2me_plus.jar
"@|Set-Content -LiteralPath (Join-Path $app 'app/launcher.ini') -Encoding unicode
Copy-Item -LiteralPath (Join-Path $root 'ports/meteos/Run-ReShade.cmd') -Destination $app
Copy-Item -LiteralPath (Join-Path $root 'ports/meteos/LAUNCHER-README.md') -Destination (Join-Path $app 'README.md')
Copy-Item -LiteralPath (Join-Path $root 'THIRD-PARTY.md') -Destination $app
$freeSource='https://github.com/TASEmulators/freej2me-plus/tree/087fe2899579e1f439655c0fc3444cbc3afeea2e'
"FreeJ2ME-Plus snapshot: 087fe2899579e1f439655c0fc3444cbc3afeea2e`r`nBinary SHA-256: $((Get-FileHash -LiteralPath $backend -Algorithm SHA256).Hash)`r`nCorresponding source: $freeSource`r`nJava runtime: see runtime/release and runtime/legal. Obtain matching OpenJDK sources from the listed vendor.`r`nLauncher source: https://github.com/piecarlos/legacy-mobile-forge`r`n"|Set-Content -LiteralPath (Join-Path $app 'SOURCES.txt') -Encoding utf8
if(Test-Path -LiteralPath (Join-Path $app 'app/game/meteos.jar')){throw 'Public launcher must not contain the game.'}
$zip=Join-Path $output 'MeteosLauncher-0.2.2-preview-windows-x64.zip'
Compress-Archive -LiteralPath $app -DestinationPath $zip -CompressionLevel Optimal
Write-Output "Game-free ZIP: $zip"
