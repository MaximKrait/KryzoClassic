# ==== Detect OS ====
UNAME_S := $(shell uname -s)

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
    SDL2_PATH = /mingw64
    CXX = x86_64-w64-mingw32-g++
    CXXFLAGS += -I$(SDL2_PATH)/include
    LDFLAGS = -L$(SDL2_PATH)/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_gfx -static
    DOCS_DIR := $(shell echo %USERPROFILE%\Documents)
endif

# ==== Project ====
SRC = Code/main.cpp Code/pascal.cpp Code/enemy.cpp Code/graphic.cpp Code/anim.cpp Code/utils.cpp
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
	$(CXX) $(OBJ) -o "$(RELEASE_DIR)/$(TARGET)" $(LDFLAGS)
	cp -r Assets "$(RELEASE_DIR)/"
	@echo "Done. Installed in $(RELEASE_DIR)"

