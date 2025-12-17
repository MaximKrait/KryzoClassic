#!/usr/bin/env bash
set -e

APP=KryzoClassic
APPDIR=AppImage/${APP}.AppDir
BUILD=build
BIN=${APP}
APPIMAGE_TOOL=AppImage/appimagetool-x86_64.AppImage
APPIMAGE_TOOL_URL="https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"

echo "[0/7] Ensure appimagetool"
if [ ! -f "$APPIMAGE_TOOL" ]; then
    echo "Downloading appimagetool..."
    mkdir -p "$(dirname "$APPIMAGE_TOOL")"
    wget -q "$APPIMAGE_TOOL_URL" -O "$APPIMAGE_TOOL"
    chmod +x "$APPIMAGE_TOOL"
fi

echo "[1/7] Clean"
rm -rf "$APPDIR" "$BUILD"
mkdir -p "$BUILD"

echo "[2/7] Build binary"
make clean
make
g++ Code/*.o -o "$BUILD/$BIN" -lSDL2 -lSDL2_ttf -lSDL2_gfx -lSDL2_image -lSDL2_mixer

echo "[3/7] Create AppDir structure"
mkdir -p "$APPDIR/usr/bin" "$APPDIR/usr/lib" "$APPDIR/usr/share/applications" "$APPDIR/usr/share/icons/hicolor/256x256/apps"

cp "$BUILD/$BIN" "$APPDIR/usr/bin/"
cp -r Assets "$APPDIR/"

echo "[4/7] Desktop entry"
cat > "$APPDIR/usr/share/applications/kryzo-classic.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=Kryzo Classic
Exec=$BIN
Icon=kryzo-classic
Terminal=false
Categories=Game;
EOF

cp Assets/Icons/kryzo-classic.png "$APPDIR/usr/share/icons/hicolor/256x256/apps/"

ln -sf usr/share/applications/kryzo-classic.desktop "$APPDIR/"
ln -sf usr/share/icons/hicolor/256x256/apps/kryzo-classic.png "$APPDIR/"

echo "[5/7] AppRun"
cat > "$APPDIR/AppRun" <<EOF
#!/bin/sh
HERE="\$(dirname "\$(readlink -f "\$0")")"
cd "\$HERE"
exec ./usr/bin/KryzoClassic "\$@"
EOF
chmod +x "$APPDIR/AppRun"

echo "[6/7] Copy SDL libs"

DEPS=$(ldd "$BUILD/$BIN" | grep -E 'SDL|freetype|harfbuzz|png|z|bz2|glib-2.0' | awk '{print $3}' | sort -u)
for lib in $DEPS; do
    if [ -f "$lib" ]; then
        cp -v "$lib" "$APPDIR/usr/lib/"
    fi
done

echo "[7/7] Build AppImage"
chmod +x "$APPIMAGE_TOOL"
"$APPIMAGE_TOOL" "$APPDIR"

echo "[OK] Done. AppImage should be ready."
