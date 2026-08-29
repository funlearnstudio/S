$ErrorActionPreference = 'Stop'
$Repo = 'funlearnstudio/S'
$Root = Join-Path $env:LOCALAPPDATA 'SE'
$Asset = 'se-windows-x86_64.zip'
$Url = "https://github.com/$Repo/releases/latest/download/$Asset"
$Temp = Join-Path $env:TEMP ('se-install-' + [guid]::NewGuid())
New-Item -ItemType Directory -Force -Path $Temp | Out-Null
try {
  $Zip = Join-Path $Temp $Asset
  Invoke-WebRequest -Uri $Url -OutFile $Zip -UseBasicParsing
  if (Test-Path $Root) { Remove-Item -Recurse -Force $Root }
  New-Item -ItemType Directory -Force -Path $Root | Out-Null
  Expand-Archive -Path $Zip -DestinationPath $Root -Force
  $Bin = Join-Path $Root 'bin'
  $UserPath = [Environment]::GetEnvironmentVariable('Path', 'User')
  $Parts = @($UserPath -split ';' | Where-Object { $_ -and $_ -ne $Bin })
  [Environment]::SetEnvironmentVariable('Path', (($Parts + $Bin) -join ';'), 'User')
  Write-Host "Installed SE to $Root"
  Write-Host 'Open a new terminal and run: se --version'
} finally {
  Remove-Item -Recurse -Force $Temp -ErrorAction SilentlyContinue
}
