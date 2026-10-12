param([int]$Port = 8770)
$ErrorActionPreference = 'Stop'
$taskTool = Join-Path $PSScriptRoot 'tools\laptop-qc'
$taskPython = Join-Path $taskTool '.venv\Scripts\python.exe'
if (-not (Test-Path -LiteralPath $taskPython)) {
    & python -m venv (Join-Path $taskTool '.venv')
    if ($LASTEXITCODE -ne 0) { throw 'Python3 / venv unavailable.' }
}
& $taskPython -m pip install -r (Join-Path $taskTool 'requirements.txt')
if ($LASTEXITCODE -ne 0) { throw 'Dependency installation failed.' }
Write-Host "Open http://127.0.0.1:$Port/ . Keep this terminal running; Ctrl+C stops the backend."
& $taskPython (Join-Path $taskTool 'server.py') --port $Port
