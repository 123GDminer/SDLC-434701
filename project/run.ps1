# Электронный путеводитель. Сборка и запуск в Windows.
param([switch]$BuildOnly)

$ErrorActionPreference = 'Stop'
$projectDirectory = $PSScriptRoot
$buildDirectory = Join-Path $projectDirectory 'build'
$cmakeCommand = Get-Command cmake.exe -ErrorAction SilentlyContinue
$cmake = $cmakeCommand.Source

if (-not $cmake) {
    $visualStudioInstaller = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (Test-Path -LiteralPath $visualStudioInstaller) {
        $cmake = & $visualStudioInstaller -latest -products '*' -version '[18.0,19.0)' -find 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe' |
            Select-Object -First 1
    }
}

if (-not $cmake) {
    throw 'CMake not found. Install the C++ and CMake components of Visual Studio Build Tools 2026.'
}

& $cmake -S $projectDirectory -B $buildDirectory -G 'Visual Studio 18 2026' -A x64
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& $cmake --build $buildDirectory --config Release --target ElectronicGuide
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

if (-not $BuildOnly) {
    $guide = Join-Path $buildDirectory 'bin\Release\ElectronicGuide.exe'
    & $guide
}
