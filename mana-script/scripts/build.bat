@echo off
REM Build script for Manascript compiler on Windows

REM Create build directory
if not exist build mkdir build

REM Configure and build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

REM Run tests
ctest -C Release

echo.
echo Build completed. The manascript executable is in build\Release\
echo.