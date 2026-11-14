[CmdletBinding()]
param()
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot
$lock=Get-Content -LiteralPath (Join-Path $root 'dependencies.lock.json') -Raw|ConvertFrom-Json
$package=$lock.packages|Where-Object id -eq 'freej2me'
$deps=Join-Path $root 'dependencies'
$null=New-Item -ItemType Directory -Path (Join-Path $deps 'freej2me') -Force
$ready=$true
foreach($file in $package.files){$path=Join-Path $deps $file.to;if(!(Test-Path -LiteralPath $path) -or (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -ne $file.sha256){$ready=$false}}
if(!$ready){
 $cache=Join-Path $deps ('cache-meteos-'+[guid]::NewGuid());$null=New-Item -ItemType Directory -Path $cache
 $headers=@{};foreach($entry in $package.headers.psobject.properties){$headers[$entry.Name]=$entry.Value}
 $archive=Join-Path $cache 'runtime.zip'
 Invoke-WebRequest -Uri $package.url -Headers $headers -OutFile $archive -UseBasicParsing
 if((Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash -ne $package.sha256){throw 'Locked runtime archive changed; do not substitute another nightly.'}
 Expand-Archive -LiteralPath $archive -DestinationPath (Join-Path $cache 'files')
 foreach($file in $package.files){$source=Join-Path $cache ('files/'+$file.from);if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ne $file.sha256){throw 'Locked runtime file mismatch.'};Copy-Item -LiteralPath $source -Destination (Join-Path $deps $file.to)}
}
$license=$lock.files|Where-Object to -eq 'freej2me/LICENSE'
$dest=Join-Path $deps $license.to
if(!(Test-Path -LiteralPath $dest) -or (Get-FileHash -LiteralPath $dest -Algorithm SHA256).Hash -ne $license.sha256){Invoke-WebRequest -Uri $license.url -OutFile $dest -UseBasicParsing}
if((Get-FileHash -LiteralPath $dest -Algorithm SHA256).Hash -ne $license.sha256){throw 'Runtime license mismatch.'}
Write-Output 'Meteos runtime and license verified. No game downloaded.'
