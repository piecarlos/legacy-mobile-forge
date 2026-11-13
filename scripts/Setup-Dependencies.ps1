[CmdletBinding()]
param()
$ErrorActionPreference='Stop'
$forgeRoot=Split-Path $PSScriptRoot
$lock=Get-Content -LiteralPath (Join-Path $forgeRoot 'dependencies.lock.json') -Raw|ConvertFrom-Json
$deps=Join-Path $forgeRoot 'dependencies'
New-Item -ItemType Directory -Path $deps -Force|Out-Null
foreach($package in $lock.packages){
 $ready=$true
 foreach($file in $package.files){$path=Join-Path $deps $file.to;if(!(Test-Path $path) -or (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -ne $file.sha256){$ready=$false}}
 if($ready){continue}
 $cache=Join-Path $deps ('cache-'+$package.id+'-'+[guid]::NewGuid());New-Item -ItemType Directory -Path $cache|Out-Null
 $headers=@{};if($package.headers){foreach($entry in $package.headers.psobject.properties){$headers[$entry.Name]=$entry.Value}}
 $archive=Join-Path $cache 'download.zip';Invoke-WebRequest -Uri $package.url -Headers $headers -OutFile $archive -UseBasicParsing
 if((Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash -ne $package.sha256){throw "Checksum diferente em $($package.id). Não usar automaticamente uma nightly nova; revisar o lock explicitamente."}
 $unpacked=Join-Path $cache 'unpacked';Expand-Archive -LiteralPath $archive -DestinationPath $unpacked
 foreach($file in $package.files){$source=Join-Path $unpacked $file.from;$dest=Join-Path $deps $file.to;if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ne $file.sha256){throw "Arquivo diferente: $($file.from)"};New-Item -ItemType Directory -Path (Split-Path $dest) -Force|Out-Null;Copy-Item -LiteralPath $source -Destination $dest}
}
foreach($file in $lock.files){
 $dest=Join-Path $deps $file.to
 if((Test-Path $dest) -and (Get-FileHash -LiteralPath $dest -Algorithm SHA256).Hash -eq $file.sha256){continue}
 Invoke-WebRequest -Uri $file.url -OutFile $dest -UseBasicParsing
 if((Get-FileHash -LiteralPath $dest -Algorithm SHA256).Hash -ne $file.sha256){throw "Checksum diferente: $($file.to)"}
}
$compiler=Join-Path $env:WINDIR 'Microsoft.NET/Framework64/v4.0.30319/csc.exe'
if(!(Test-Path $compiler)){throw '.NET Framework 4.x compiler não encontrado. Compile engine/native/SdlPadBridge.cs antes de usar SDL.'}
& $compiler /nologo /platform:x64 "/out:$deps/controllers/SdlPadBridge.exe" (Join-Path $forgeRoot 'engine/native/SdlPadBridge.cs')
if($LASTEXITCODE -ne 0){throw 'Falha ao compilar bridge SDL3.'}
Write-Output 'Dependências verificadas; bridge SDL3 compilado.'
