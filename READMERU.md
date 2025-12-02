![Logo](ForREADME/logoBig1.png)

Небольшая игра на C++, в которой нужно уворачиваться от треугольников. 

## Установка

### Linux
1. Загрузите последнюю версию tar.gz или AppImage [отсюда](https://github.com/MaximKrait/KryzoClassic/releases)

Если вы скачали tar.gz, установите зависимости:

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
2. Запустите игру, открыв файл Kryzo_Classic или Kryzo_Classic-x86_64.AppImage.

### Windows

1. Загрузите последнюю версию zip-архива [отсюда](https://github.com/MaximKrait/KryzoClassic/releases)
2. Запустите игру через Kryzo_Classic.exe.

## Сборка из исходного кода

Если вы хотите скомпилировать игру самостоятельно из исходных файлов, следуйте этим инструкциям:

### Linux

1. Получение файлов

Вы можете получить исходный код прямо [здесь](https://github.com/MaximKrait/KryzoClassic/releases).

Или вы можете сделать следующее:

```bash
git clone https://github.com/MaximKrait/KryzoClassic.git
cd KryzoClassic
```
1.1. Установите зависимости

 ```bash
chmod +x install-deps.sh
./install-deps.sh 
 ```

2. Скомпилируйте код

 ```bash
make install
 ```
Makefile установит игру в домашний каталог пользователя, а именно по следующему пути: /home/HOME_DIR/Documents/Kryzo_Classic

HOME_DIR означает имя пользователя.

3. Запустите игру
```bash
 ./Kryzo_Classic
 ```
или щелкните по исполняемому файлу

### Windows

1. Загрузите MSYS2 и откройте MINGW64, затем установите необходимые инструменты
```bash
pacman -S --needed git mingw-w64-x86_64-gcc mingw-w64-x86_64-make
```
2. Загрузите исходный код игры
```bash
git clone https://github.com/MaximKrait/KryzoClassic.git
cd KryzoClassic
```
3. Установите необходимые зависимости
```bash
./install-deps-win.bat
```
4. Скомпилируйте игру
```bash
mingw32-make install
```
Makefile установит игру в папку «Games», а именно в C:\Users\имя_пользователя\Games\Kryzo_Classic

5. Запустите игру через Kryzo_Classic.exe
