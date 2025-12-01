@echo off
echo Checking MSYS2...

where pacman >nul 2>nul
if %errorlevel% neq 0 (
    echo ERROR: MSYS2 not found. Install from https://www.msys2.org/
    pause
    exit /b 1
)

echo Checking SDL2...

set NEED=0

pkg-config --exists sdl2
if %errorlevel% neq 0 set NEED=1

pkg-config --exists SDL2_ttf
if %errorlevel% neq 0 set NEED=1

pkg-config --exists SDL2_gfx
if %errorlevel% neq 0 set NEED=1

if %NEED%==0 (
    echo All dependencies already installed.
    exit /b 0
)

echo Installing SDL2 packages...

pacman -S --noconfirm mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_gfx

echo OK
exit /b 0

