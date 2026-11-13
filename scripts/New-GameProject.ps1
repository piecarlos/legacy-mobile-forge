[CmdletBinding()]
param([Parameter(Mandatory)][string]$GamePath,[Parameter(Mandatory)][ValidatePattern('^[a-z0-9][a-z0-9-]{0,60}$')][string]$Id,[string]$Title,[int]$Width=240,[int]$Height=320,[string]$CraftPath)
$ErrorActionPreference='Stop'
$forgeRoot=Split-Path $PSScriptRoot
$inputFile=Get-Item -LiteralPath $GamePath
if($inputFile.PSIsContainer){throw 'Informe um arquivo JAR, JAD, SIS ou SISX.'}
$case=Join-Path $forgeRoot "games/$Id"
if(Test-Path -LiteralPath $case){throw 'Este projeto já existe; use a pasta existente para preservar progresso.'}
foreach($folder in @('input','analysis','output','assets')){New-Item -ItemType Directory -Path (Join-Path $case $folder) -Force|Out-Null}
Copy-Item -LiteralPath $inputFile.FullName -Destination (Join-Path $case 'input')
$format=$inputFile.Extension.ToLowerInvariant()
$platform=if($format -in @('.jar','.jad')){'j2me'}elseif($format -in @('.sis','.sisx')){'symbian-native'}else{'unknown'}
if(!$Title){$Title=$inputFile.BaseName}
[ordered]@{id=$Id;title=$Title;platform=$platform;input=$inputFile.Name;sha256=(Get-FileHash -LiteralPath $inputFile.FullName -Algorithm SHA256).Hash;width=$Width;height=$Height;status='created';controllerValidation=@{gameSir='not-tested-for-this-game';xbox='not-tested';dualshock='not-tested';dualsense='not-tested'}}|ConvertTo-Json -Depth 5|Set-Content -LiteralPath (Join-Path $case 'project.json') -Encoding utf8
"title=$Title`nwidth=$Width`nheight=$Height`ncontroller.backend=sdl`ncontroller.slot=0"|Set-Content -LiteralPath (Join-Path $case 'profile.properties') -Encoding utf8
if(!$CraftPath){$CraftPath=Join-Path $env:ProgramFiles 'Craft Anything/craft-anything-cli.exe'}
if(Test-Path -LiteralPath $CraftPath){
 & $CraftPath --deep triage $inputFile.FullName | Set-Content -LiteralPath (Join-Path $case 'analysis/craft-triage.json') -Encoding utf8
 if($LASTEXITCODE -ne 0){Write-Warning 'Craft triage falhou; consulte o relatório e continue a classificação manual.'}
 if($format -eq '.jar'){& $CraftPath --deep java-archive-show --path $inputFile.FullName | Set-Content -LiteralPath (Join-Path $case 'analysis/craft-java-archive.json') -Encoding utf8}
}else{Write-Warning 'Craft Anything não encontrado; instale-o ou informe -CraftPath.'}
@"
# $Title

Leia ../../AGENTS.md e project.json. Entrada privada em input/. Plataforma detectada: $platform.
Use Craft Anything para verificar o formato antes de escolher o backend.
JAR/J2ME pode usar Build-GameProject.ps1. JAD requer localizar o JAR associado.
SIS/SISX exige investigação de Symbian nativo/EKA2L1, não recompilar como Java.
Não substituir saves nem declarar controles não testados como compatíveis.
"@|Set-Content -LiteralPath (Join-Path $case 'AGENTS.md') -Encoding utf8
Write-Output "Projeto criado: $case"
