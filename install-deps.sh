#!/usr/bin/env bash
set -e

echo "[CHECK] Detecting system package manager..."

PM=""
if command -v apt >/dev/null 2>&1; then
    PM="apt"
elif command -v dnf >/dev/null 2>&1; then
    PM="dnf"
elif command -v pacman >/dev/null 2>&1; then
    PM="pacman"
elif command -v zypper >/dev/null 2>&1; then
    PM="zypper"
fi

check_lib() {
    echo -n "Checking $1... "
    if pkg-config --exists "$1"; then
        echo "FOUND"
        return 0
    fi
    echo "MISSING"
    return 1
}

install_pkg() {
    case "$PM" in
        apt)
            sudo apt update
            sudo apt install -y libsdl2-dev libsdl2-ttf-dev libsdl2-gfx-dev libsdl2-image-dev libsdl2-mixer-dev
            ;;
        dnf)
            sudo dnf update
            sudo dnf install -y SDL2-devel SDL2_ttf-devel SDL2_gfx-devel SDL2_image-devel SDL2_mixer-devel
            ;;
        pacman)
            sudo pacman -Sy --noconfirm sdl2 sdl2_ttf sdl2_gfx sdl2_image sdl2_mixer
            ;;
        zypper)
            sudo zypper install -y libSDL2-devel libSDL2_ttf-devel SDL2_gfx-devel SDL2_image-devel SDL2_mixer-devel
            ;;
        *)
            return 1
            ;;
    esac
}

echo "[CHECK] Checking for SDL2 dependencies..."

NEED_INSTALL=0
check_lib sdl2        || NEED_INSTALL=1
check_lib SDL2_ttf    || NEED_INSTALL=1
check_lib SDL2_gfx    || NEED_INSTALL=1

if [ $NEED_INSTALL -eq 0 ]; then
    echo "[OK] All dependencies already installed."
    exit 0
fi

if [ -z "$PM" ]; then
    echo "[WARN] Unknown distro. Building SDL2 locally in ./libs/"

    mkdir -p libs
    cd libs

    if [ ! -d SDL ]; then
        echo "[DL] Downloading SDL2..."
        git clone --depth 1 https://github.com/libsdl-org/SDL.git
        mkdir SDL/build && cd SDL/build
        cmake .. && make -j$(nproc)
        cd ../../
    fi

    if [ ! -d SDL_ttf ]; then
        echo "[DL] Downloading SDL2_ttf..."
        git clone --depth 1 https://github.com/libsdl-org/SDL_ttf.git
        mkdir SDL_ttf/build && cd SDL_ttf/build
        cmake .. && make -j$(nproc)
        cd ../../
    fi

    if [ ! -d SDL2_gfx ]; then
        echo "[DL] Downloading SDL2_gfx..."
        git clone --depth 1 https://github.com/ferzkopp/SDL_gfx.git SDL2_gfx
        mkdir SDL2_gfx/build && cd SDL2_gfx/build
        cmake .. && make -j$(nproc)
        cd ../../
    fi

    echo "[OK] Local SDL2 build finished."
    exit 0
fi

echo "[INSTALL] Installing dependencies via $PM..."
install_pkg

echo "[OK] Done."

