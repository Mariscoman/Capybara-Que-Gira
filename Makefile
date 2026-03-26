# Forzamos el uso de CMD para evitar conflictos con shells de Git Bash o MSYS
SHELL := cmd.exe

# --- Compiladores ---
CXX      := g++
CC       := gcc
CXXFLAGS := -std=c++17 -Wall -Wextra -g -Iinclude
LDFLAGS  := -Llib
LDLIBS   := -lglfw3 -lopengl32 -lgdi32

# --- Directorios (Usamos / para Make, pero \ para comandos de Windows) ---
SRC_DIR  := src
OBJ_DIR  := obj
BIN_DIR  := bin
TARGET   := $(BIN_DIR)/main.exe

# --- Descubrimiento de archivos ---
SRCS_CPP := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)
SRCS_C   := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c)

# Convertir rutas a objetos
OBJS     := $(SRCS_CPP:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o) \
            $(SRCS_C:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

# Regla para C++
# Usamos $(subst) para cambiar / por \ solo en el comando mkdir
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para C (glad.c)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	@if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)
	@if exist $(BIN_DIR) rmdir /s /q $(BIN_DIR)

run: all
	.$(TARGET)