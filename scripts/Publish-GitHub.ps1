[CmdletBinding()]
param([string]$Repository='piecarlos/legacy-mobile-forge',[switch]$Create)
$ErrorActionPreference='Stop'
if($Repository -notmatch '^piecarlos/[a-zA-Z0-9_.-]+$'){throw 'Destino deve ser um repositório da conta piecarlos, sem token na URL.'}
$root=Split-Path $PSScriptRoot
$gh=Get-Command gh.exe -ErrorAction SilentlyContinue
if(!$gh){throw 'GitHub CLI ausente. Autentique um Git/CLI adequado antes de publicar; não cole tokens em arquivos ou no chat.'}
& $gh.Source auth status
if($LASTEXITCODE -ne 0){throw 'GitHub CLI não autenticado.'}
git -C $root status --short
git -C $root ls-files
if($Create){& $gh.Source repo create $Repository --public --source $root --remote origin --push}
else{git -C $root -c credential.interactive=never push -u origin main}
if($LASTEXITCODE -ne 0){throw 'Publicação não foi concluída.'}
