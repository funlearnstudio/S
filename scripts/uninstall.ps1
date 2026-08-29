$ErrorActionPreference = 'Stop'
$Root = Join-Path $env:LOCALAPPDATA 'SE'
$Bin = Join-Path $Root 'bin'
$UserPath = [Environment]::GetEnvironmentVariable('Path', 'User')
$Parts = @($UserPath -split ';' | Where-Object { $_ -and $_ -ne $Bin })
[Environment]::SetEnvironmentVariable('Path', ($Parts -join ';'), 'User')
if (Test-Path $Root) { Remove-Item -Recurse -Force $Root }
Write-Host 'SE has been uninstalled.'
