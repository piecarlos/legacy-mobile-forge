[CmdletBinding()]
param([Parameter(Mandatory)][string]$GamePath,[string]$JdkPath,[string]$OutputPath,[string]$CraftPath,[switch]$Package)
$ErrorActionPreference='Stop'
if(!$env:PATHEXT){$env:PATHEXT='.COM;.EXE;.BAT;.CMD'}
. (Join-Path $PSScriptRoot 'Toolchain.ps1')
$root=Split-Path $PSScriptRoot
$spec=Get-Content -LiteralPath (Join-Path $root 'ports/meteos/game.json') -Raw|ConvertFrom-Json
$game=Get-Item -LiteralPath $GamePath
if($game.PSIsContainer -or $game.Length -ne $spec.size_bytes -or (Get-FileHash -LiteralPath $game.FullName -Algorithm SHA256).Hash -ne $spec.sha256){
 throw "Unsupported game file. Requires Meteos Astro Blocks 1.0.4, 240x320, $($spec.reference_filename), SHA-256 $($spec.sha256). Renaming another JAR will not work."
}
$jdk=Get-ForgeJdk $JdkPath
if(!$OutputPath){$OutputPath=Join-Path $root ('build/meteos-preview-'+(Get-Date -Format 'yyyyMMdd-HHmmss'))}
if(Test-Path -LiteralPath $OutputPath){throw 'Output already exists. Choose a new -OutputPath to preserve saves.'}
$null=New-Item -ItemType Directory -Path $OutputPath
$output=(Resolve-Path -LiteralPath $OutputPath).Path
$classes=Join-Path $output 'classes';$stage=Join-Path $output 'app-input';$temp=Join-Path $output 'temp'
foreach($dir in @($classes,$stage,$temp,(Join-Path $stage 'game'),(Join-Path $stage 'tools/freej2me'))){$null=New-Item -ItemType Directory -Path $dir -Force}
$craft=$CraftPath
if(!$craft){$programs=[Environment]::GetFolderPath('ProgramFiles');if(!$programs){$programs='C:\Program Files'};$craft=Join-Path $programs 'Craft Anything/craft-anything-cli.exe'}
if(Test-Path -LiteralPath $craft){
 foreach($job in @(@{args='--deep triage';file='craft-triage.json'},@{args='--deep java-archive-show --path';file='craft-java-archive.json'})){
  $info=New-Object System.Diagnostics.ProcessStartInfo
  $info.FileName=$craft;$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.RedirectStandardOutput=$true
  $info.Arguments=$job.args+' "'+$game.FullName+'"'
  $process=New-Object System.Diagnostics.Process;$process.StartInfo=$info
  try{[void]$process.Start();$text=$process.StandardOutput.ReadToEnd();$process.WaitForExit();if($process.ExitCode -ne 0){throw 'Craft Anything analysis failed.'};$text|Set-Content -LiteralPath (Join-Path $output $job.file) -Encoding utf8}finally{$process.Dispose()}
 }
}else{Write-Warning 'Craft Anything is not installed. Exact JAR hash was verified; deeper triage skipped.'}
$backend=Join-Path $root 'dependencies/freej2me/freej2me_plus.jar'
if(!(Test-Path -LiteralPath $backend)){throw 'Run scripts/Setup-Dependencies.ps1 first.'}
$lock=Get-Content -LiteralPath (Join-Path $root 'dependencies.lock.json') -Raw|ConvertFrom-Json
$expected=($lock.packages|Where-Object id -eq 'freej2me').files|Where-Object to -eq 'freej2me/freej2me_plus.jar'
if((Get-FileHash -LiteralPath $backend -Algorithm SHA256).Hash -ne $expected.sha256){throw 'Backend checksum mismatch; run Setup-Dependencies.ps1. Do not substitute another nightly.'}
$sources=@(Get-ChildItem (Join-Path $root 'ports/meteos/src') -Filter '*.java'|ForEach-Object FullName)
$sources+=Join-Path $root 'engine/src/ReshadeBootstrap.java'
Invoke-ForgeTool (Join-Path $jdk 'bin/javac.exe') (@('--release','21','-encoding','UTF-8','-cp',$backend,'-d',$classes)+$sources)
Copy-Item -LiteralPath $game.FullName -Destination (Join-Path $stage 'game/meteos.jar')
Copy-Item -Path (Join-Path $root 'dependencies/freej2me/*') -Destination (Join-Path $stage 'tools/freej2me') -Force
$manifest=Join-Path $output 'MANIFEST.MF'
"Manifest-Version: 1.0`r`nMain-Class: MeteosDesktop`r`nClass-Path: tools/freej2me/freej2me_plus.jar`r`n`r`n"|Set-Content -LiteralPath $manifest -Encoding ascii
Invoke-ForgeTool (Join-Path $jdk 'bin/jar.exe') @("-J-Djava.io.tmpdir=$temp",'--create','--file',(Join-Path $stage 'meteos-preview.jar'),'--manifest',$manifest,'-C',$classes,'.')
Copy-Item -LiteralPath (Join-Path $root 'ports/meteos/README.md') -Destination (Join-Path $stage 'README.md')
Copy-Item -LiteralPath (Join-Path $root 'LICENSE') -Destination (Join-Path $stage 'LICENSE')
if($Package){
 $runtime=Join-Path $output 'runtime'
 Invoke-ForgeTool (Join-Path $jdk 'bin/jlink.exe') @('--add-modules','java.desktop,java.logging,java.xml,java.naming,java.management,java.scripting,jdk.charsets,jdk.unsupported','--strip-debug','--no-man-pages','--no-header-files','--compress=zip-6','--output',$runtime)
 Invoke-ForgeTool (Join-Path $jdk 'bin/jpackage.exe') @('--type','app-image','--input',$stage,'--main-jar','meteos-preview.jar','--main-class','MeteosDesktop','--name','MeteosPreview','--app-version','0.2.0','--dest',(Join-Path $output 'dist'),'--runtime-image',$runtime,'--temp',(Join-Path $temp 'jpackage'))
 $app=Join-Path $output 'dist/MeteosPreview'
 Copy-Item -LiteralPath (Join-Path $root 'ports/meteos/Run-ReShade.cmd') -Destination $app
 Copy-Item -LiteralPath (Join-Path $root 'ports/meteos/README.md') -Destination $app
 Write-Output "Portable preview: $app"
}
Write-Output "Private game build (DO NOT publish this folder): $output"
