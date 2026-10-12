param([string]$ArduinoCli = 'arduino-cli', [switch]$PublicLabRelease)
$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    if (-not (Get-Command $ArduinoCli -ErrorAction SilentlyContinue)) {
        $candidate = Join-Path $env:LOCALAPPDATA 'Programs\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe'
        if (-not (Test-Path -LiteralPath $candidate)) { throw 'Install Arduino CLI or pass -ArduinoCli with its path.' }
        $ArduinoCli = $candidate
    }
    $secretPath = Join-Path $PSScriptRoot 'compact_qc_tester\ota_secrets.h'
    if ($PublicLabRelease -and (Test-Path -LiteralPath $secretPath)) {
        throw 'Public release refuses a private ota_secrets.h override. Build in a clean checkout without that file.'
    }
    Write-Host 'Without a private override, firmware uses the intentional public LAB default.'
    $cores = & $ArduinoCli core list --format json | ConvertFrom-Json
    if (-not ($cores.platforms | Where-Object { $_.id -eq 'esp32:esp32' -and $_.installed_version -eq '3.3.10' })) { throw 'Required Arduino ESP32 core 3.3.10 not installed.' }
    python scripts/firmware_metadata.py generate
    if ($LASTEXITCODE -ne 0) { throw 'Metadata generation failed.' }
    & $ArduinoCli compile --jobs 4 --fqbn esp32:esp32:esp32:FlashSize=16M --build-property upload.maximum_size=2097152 --output-dir build compact_qc_tester
    if ($LASTEXITCODE -ne 0) { throw 'Firmware build failed.' }
    $binary = Join-Path $PSScriptRoot 'build\compact_qc_tester.ino.bin'
    if ((Get-Item -LiteralPath $binary).Length -ge 2097152) { throw 'Application exceeds 2MB OTA slot.' }
    python scripts/firmware_metadata.py verify
    if ($LASTEXITCODE -ne 0) { throw 'Binary identity verification failed.' }
    python scripts/verify_partitions.py
    if ($LASTEXITCODE -ne 0) { throw 'Legacy partition verification failed.' }
    if ($PublicLabRelease) {
        & node tools/portal-package.cjs
        if ($LASTEXITCODE -ne 0) { throw 'Portal artifact packaging failed.' }
    }
    Write-Host 'Build passed. No upload was performed. Follow 04_Dokumen/TEAM_HANDOFF.md.'
} finally { Pop-Location }
