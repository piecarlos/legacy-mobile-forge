[CmdletBinding()]
param([string]$JdkPath)
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'Toolchain.ps1')
$root=Split-Path $PSScriptRoot;$jdk=Get-ForgeJdk $JdkPath
$classes=Join-Path $root 'build/demo-classes';$temp=Join-Path $root 'build/temp'
New-Item -ItemType Directory -Path $classes,$temp -Force|Out-Null
Invoke-ForgeTool (Join-Path $jdk 'bin/javac.exe') @('--release','8','-cp',(Join-Path $root 'dependencies/freej2me/freej2me_plus.jar'),'-d',$classes,(Join-Path $root 'fixtures/demo/DemoMIDlet.java'))
Invoke-ForgeTool (Join-Path $jdk 'bin/jar.exe') @("-J-Djava.io.tmpdir=$temp",'--create','--file',(Join-Path $root 'build/forge-demo.jar'),'--manifest',(Join-Path $root 'fixtures/demo/MANIFEST.MF'),'-C',$classes,'.')
Write-Output (Join-Path $root 'build/forge-demo.jar')
