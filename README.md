![Logo](ForREADME/logoBig1.png)

A small C++ game where you have to dodge triangles. 

[README in russian | README на русском](https://github.com/MaximKrait/KryzoClassic/blob/main/READMERU.md)

## Downloading

### Linux
1. Download the latest tar.gz or AppImage [from here](https://github.com/MaximKrait/KryzoClassic/releases)

If you downloaded tar.gz, please install the dependencies:

- Fedora
  
 ```bash
 sudo dnf install g++ SDL2-devel SDL2_ttf-devel SDL2_gfx-devel
 ```
- Ubuntu/Debian

 ```bash
 sudo apt install g++ libsdl2-dev libsdl2-ttf-dev libsdl2-gfx-dev
 ```

- Arch Linux

 ```bash
 sudo pacman -S base-devel sdl2 sdl2_ttf sdl2_gfx
 ```
2. Play by launching either Kryzo_Classic or Kryzo_Classic-x86_64.AppImage.

### Windows

1. Download the latest zip archive [from here](https://github.com/MaximKrait/KryzoClassic/releases)
2. Play by launching Kryzo_Classic.exe

## Building

If you want to compile the game yourself from the source files, then proceed according to these instructions:

### Linux

1. File retrieval

You can get the source code directly from [here](https://github.com/MaximKrait/KryzoClassic/releases)

Or you can do the following:

 ```bash
 git clone https://github.com/MaximKrait/KryzoClassic.git
cd KryzoCLassic
 ```
1.1. Installing dependencies.

 ```bash
./install-deps.sh 
 ```

2. Compile the code

 ```bash
make install
 ```
The Makefile will install the game in the user's home directory, specifically at the following path: /home/HOME_DIR/Documents/Kryzo_Classic

HOME_DIR refers to the user's name.

3. Start game
 ```bash
 ./Kryzo_Classic
 ```
or click on the executable file

### Windows

1. Download MSYS2 and open MINGW64, then install the necessary tools
```bash
pacman -S --needed git mingw-w64-x86_64-gcc mingw-w64-x86_64-make
```
2. Download the game source code
```bash
git clone https://github.com/MaximKrait/KryzoClassic.git
cd KryzoClassic
```
3. Install the necessary dependencies
```bash
./install-deps-win.bat
```
4. Compile the game
```bash
mingw32-make install
```
Makefile will install the game in the Games folder, specifically in C:\Users\username\Games\Kryzo_Classic

5. Run the game via Kryzo_Classic.exe
