# ==== Detect OS ====
UNAME_S := $(shell uname -s)

ifeq ($(OS),Windows_NT)
    IS_WINDOWS := 1
else
    IS_WINDOWS := 0
endif

# ==== Compiler ====
CXX = g++

# ==== Common flags ====
CXXFLAGS = -O2 -Wall -std=c++17

# ==== SDL flags for each OS ====
ifeq ($(UNAME_S), Linux)
    LDFLAGS = -lSDL2 -lSDL2_ttf -lSDL2_gfx
    HOME_DIR := $(shell echo $$HOME)
    XDG_DOCS := $(shell grep XDG_DOCUMENTS_DIR $$HOME/.config/user-dirs.dirs 2>/dev/null | cut -d'=' -f2 | tr -d '"')
    ifeq ($(XDG_DOCS),)
        DOCS_DIR := $(HOME_DIR)/Documents
    else
        DOCS_DIR := $(XDG_DOCS)
    endif
else
    # Windows (MSYS2 MinGW64)
    SDL2_PATH = /mingw64
    CXX = x86_64-w64-mingw32-g++
    CXXFLAGS += -I$(SDL2_PATH)/include
    LDFLAGS = -L$(SDL2_PATH)/lib \
              -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_gfx \
              -lsetupapi -limm32 -lversion -lole32 -loleaut32 \
              -lwinmm -lgdi32 -luser32 -lkernel32 -lbcrypt
    DOCS_DIR := $(subst \,/,$(USERPROFILE)/Games)
endif

# ==== Project ====
SRC = Code/main.cpp Code/pascal.cpp Code/enemy.cpp Code/graphic.cpp Code/anim.cpp Code/utils.cpp Code/display.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = Kryzo_Classic
RELEASE_DIR := $(DOCS_DIR)/Kryzo_Classic

# ==== Build object files ====
all: $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

# ==== Install (real release) ====
install: $(OBJ)
	@echo "Installing to $(RELEASE_DIR)..."
	mkdir -p "$(RELEASE_DIR)"

	# Build executable
	$(CXX) $(OBJ) -o "$(RELEASE_DIR)/$(TARGET)" $(LDFLAGS)

	# Copy assets
	cp -r Assets "$(RELEASE_DIR)/"

ifeq ($(IS_WINDOWS),1)
	# ==== Windows only: copy DLLs ====
	cp $(SDL2_PATH)/bin/SDL2.dll "$(RELEASE_DIR)/"
	cp $(SDL2_PATH)/bin/SDL2_ttf.dll "$(RELEASE_DIR)/"
	cp $(SDL2_PATH)/bin/libSDL2_gfx-1-0-0.dll "$(RELEASE_DIR)/"
	cp /mingw64/bin/libgcc_s_seh-1.dll "$(RELEASE_DIR)/"
	cp /mingw64/bin/libstdc++-6.dll "$(RELEASE_DIR)/"
	cp /mingw64/bin/libwinpthread-1.dll "$(RELEASE_DIR)/"
	cp /mingw64/bin/libfreetype-6.dll "$(RELEASE_DIR)/"
	cp /mingw64/bin/libharfbuzz-0.dll "$(RELEASE_DIR)/"
	cp /mingw64/bin/libbz2-1.dll "$(RELEASE_DIR)/"
	cp /mingw64/bin/libbrotlidec.dll "$(RELEASE_DIR)/"
	cp /mingw64/bin/libpng16-16.dll "$(RELEASE_DIR)/"
	cp /mingw64/bin/zlib1.dll "$(RELEASE_DIR)/"
	cp /mingw64/bin/libglib-2.0-0.dll "$(RELEASE_DIR)/"
	cp /mingw64/bin/libgraphite2.dll "$(RELEASE_DIR)/"
	cp /mingw64/bin/libintl-8.dll "$(RELEASE_DIR)/"
	cp /mingw64/bin/libbrotlicommon.dll "$(RELEASE_DIR)/"
	cp /mingw64/bin/libiconv-2.dll "$(RELEASE_DIR)/"
	cp /mingw64/bin/libpcre2-8-0.dll "$(RELEASE_DIR)/"
endif

	@echo "Done. Installed in $(RELEASE_DIR)"

