param([string]$ArduinoCli = 'arduino-cli')
$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    if (-not (Get-Command $ArduinoCli -ErrorAction SilentlyContinue)) {
        $candidate = Join-Path $env:LOCALAPPDATA 'Programs\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe'
        if (-not (Test-Path -LiteralPath $candidate)) { throw 'Install Arduino CLI or pass -ArduinoCli with its path.' }
        $ArduinoCli = $candidate
    }
    $secretPath = Join-Path $PSScriptRoot 'compact_qc_tester\ota_secrets.h'
    if (-not (Test-Path -LiteralPath $secretPath)) {
        $bytes = New-Object byte[] 24
        $rng = [System.Security.Cryptography.RandomNumberGenerator]::Create()
        try { $rng.GetBytes($bytes) } finally { $rng.Dispose() }
        $password = [Convert]::ToBase64String($bytes)
        $header = "#pragma once`nconstexpr char OTA_USERNAME[] = `"admin`";`nconstexpr char OTA_PASSWORD[] = `"$password`";`n"
        [System.IO.File]::WriteAllText($secretPath,$header)
        [System.IO.File]::WriteAllText((Join-Path $PSScriptRoot 'ota-access.txt'),"OTA username: admin`nOTA password: $password`nURL: /update`nKeep this file private.`n")
        Write-Host 'Created ignored local OTA credentials in ota-access.txt; no password printed.'
    }
    & $ArduinoCli compile --jobs 4 --fqbn esp32:esp32:esp32:FlashSize=16M --build-property upload.maximum_size=2097152 --output-dir build compact_qc_tester
    if ($LASTEXITCODE -ne 0) { throw 'Firmware build failed.' }
    $binary = Join-Path $PSScriptRoot 'build\compact_qc_tester.ino.bin'
    if ((Get-Item -LiteralPath $binary).Length -ge 2097152) { throw 'Application exceeds 2MB OTA slot.' }
    Write-Host 'Build passed. No upload was performed. Follow 04_Dokumen/TEAM_HANDOFF.md.'
} finally { Pop-Location }
