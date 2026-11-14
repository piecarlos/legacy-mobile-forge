function Get-ForgeJdk {
 param([string]$JdkPath)
 if($JdkPath -and (Test-Path (Join-Path $JdkPath 'bin/javac.exe'))){return (Resolve-Path $JdkPath).Path}
 if($env:JAVA_HOME -and (Test-Path (Join-Path $env:JAVA_HOME 'bin/javac.exe'))){return $env:JAVA_HOME}
 $compiler=Get-Command javac.exe -ErrorAction SilentlyContinue
 if($compiler){return (Split-Path (Split-Path $compiler.Source))}
 throw 'JDK não encontrado. Informe -JdkPath ou configure JAVA_HOME (JDK 21+).'
}
function Invoke-ForgeTool {
 param([string]$File,[string[]]$ToolArgs)
 $info=New-Object System.Diagnostics.ProcessStartInfo
 $info.FileName=$File;$info.UseShellExecute=$false;$info.CreateNoWindow=$true
 $quoted=foreach($value in $ToolArgs){'"'+[regex]::Replace([regex]::Replace($value,'(\\*)"','$1$1\"'),'(\\+)$','$1$1')+'"'}
 $info.Arguments=$quoted -join ' '
 $process=New-Object System.Diagnostics.Process;$process.StartInfo=$info
 try{[void]$process.Start();$process.WaitForExit();$code=$process.ExitCode}finally{$process.Dispose()}
 if($code -ne 0){throw "Falhou: $File (código $code)"}
}
