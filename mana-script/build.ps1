# Create build directory if it doesn't exist
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build"
}

# Configure and build
cmake -B build -S .
cmake --build build --config Release

# Create bin directory if it doesn't exist
if (-not (Test-Path "bin")) {
    New-Item -ItemType Directory -Path "bin"
}

# Copy executable to bin directory
Copy-Item "build/Release/manascript.exe" -Destination "bin/manascript.exe" -Force

Write-Host "Build completed successfully!"
Write-Host "The executable is located at: $(Resolve-Path 'bin/manascript.exe')"
Write-Host "`nTo use manascript, you can:"
Write-Host "1. Add the bin directory to your PATH, or"
Write-Host "2. Run it directly using: .\bin\manascript.exe [options]"

$userPath = [Environment]::GetEnvironmentVariable("PATH", "User")
$binPath = (Resolve-Path "bin").Path
$newPath = ($userPath.Split(';') | Where-Object { $_ -ne $binPath }) -join ';'
[Environment]::SetEnvironmentVariable("PATH", $newPath, "User") 