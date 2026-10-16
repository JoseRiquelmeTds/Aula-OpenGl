# =============================================================================
#  Aula-OpenGl — build com Makefile (C++ + freeglut) para rodar no WSL2
#  Setup do ambiente: docs/SETUP_WSL2.md
# =============================================================================
#
#  Fluxo de compilação:
#    src/**/*.cpp (exceto main.cpp) -> build/<modo>/obj/*.o -> build/<modo>/lib/libaula-opengl.a
#    src/main.cpp                   -> build/<modo>/obj/main.o
#    main.o + libaula-opengl.a + lib/*.a + GLUT/GLU/GL -> build/<modo>/bin/aula-opengl
#
#  Uso:  make help

APP := aula-opengl

# --- Diretórios --------------------------------------------------------------
SRC_DIR := src
INC_DIR := include
LIB_DIR := lib
MODE    ?= debug
BUILD   := build/$(MODE)
OBJ_DIR := $(BUILD)/obj
OUT_LIB := $(BUILD)/lib
BIN_DIR := $(BUILD)/bin

TARGET   := $(BIN_DIR)/$(APP)
CORE_LIB := $(OUT_LIB)/lib$(APP).a

# --- Fontes ------------------------------------------------------------------
MAIN_SRC := $(SRC_DIR)/main.cpp
LIB_SRCS := $(filter-out $(MAIN_SRC),$(shell find $(SRC_DIR) -name '*.cpp'))
LIB_OBJS := $(LIB_SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
MAIN_OBJ := $(OBJ_DIR)/main.o
DEPS     := $(LIB_OBJS:.o=.d) $(MAIN_OBJ:.o=.d)

# Bibliotecas estáticas de terceiros colocadas em lib/ entram no link automaticamente
EXT_LIBS := $(wildcard $(LIB_DIR)/*.a)

# --- Ferramentas e flags -----------------------------------------------------
AR       := ar
ARFLAGS  := rcs
CPPFLAGS := -I$(INC_DIR) -MMD -MP
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic
LDLIBS   := -lglut -lGLU -lGL

ifeq ($(MODE),release)
  CXXFLAGS += -O2 -DNDEBUG
else ifeq ($(MODE),debug)
  CXXFLAGS += -g -O0
else
  $(error MODE inválido: "$(MODE)". Use MODE=debug ou MODE=release)
endif

# --- Alvos -------------------------------------------------------------------
.PHONY: all run clean deps doctor help

all: $(TARGET)

$(TARGET): $(MAIN_OBJ) $(CORE_LIB) $(EXT_LIBS)
	@mkdir -p $(@D)
	$(CXX) $(MAIN_OBJ) $(CORE_LIB) $(EXT_LIBS) $(LDLIBS) -o $@

$(CORE_LIB): $(LIB_OBJS)
	@mkdir -p $(@D)
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build

# Instala as dependências no Ubuntu do WSL2 (pede senha do sudo)
deps:
	sudo apt-get update
	sudo apt-get install -y build-essential mesa-utils libglu1-mesa-dev
	sudo apt-get install -y freeglut3-dev || sudo apt-get install -y libglut-dev

# Diagnóstico do ambiente: rode isto antes de pedir ajuda
doctor:
	@echo "== Sistema"
	@uname -r | grep -qi microsoft && echo "  [ok] rodando no WSL" || echo "  [!!] não parece WSL — veja docs/SETUP_WSL2.md"
	@case "$$(pwd)" in /mnt/*) echo "  [!!] repo em $$(pwd): clone dentro do Linux (~/), não em /mnt/c";; *) echo "  [ok] repo no filesystem do Linux";; esac
	@echo "== Ferramentas"
	@command -v $(CXX) >/dev/null && echo "  [ok] $$($(CXX) --version | head -n1)" || echo "  [!!] $(CXX) ausente — rode: make deps"
	@echo "== GLUT / OpenGL"
	@test -f /usr/include/GL/glut.h && echo "  [ok] GL/glut.h encontrado" || echo "  [!!] GL/glut.h ausente — rode: make deps"
	@test -f /usr/include/GL/glu.h  && echo "  [ok] GL/glu.h encontrado"  || echo "  [!!] GL/glu.h ausente — rode: make deps"
	@echo "== Display (WSLg)"
	@test -n "$$DISPLAY" && echo "  [ok] DISPLAY=$$DISPLAY" || echo "  [!!] DISPLAY vazio — rode 'wsl --update' e 'wsl --shutdown' no PowerShell"
	@command -v glxinfo >/dev/null && glxinfo -B 2>/dev/null | grep -E "OpenGL (renderer|version)" | sed 's/^/  /' || echo "  [!!] glxinfo indisponível ou sem display — rode: make deps"

help:
	@echo "Alvos:"
	@echo "  make deps      instala compilador, freeglut e Mesa no Ubuntu (WSL2)"
	@echo "  make doctor    verifica se o ambiente está pronto"
	@echo "  make           compila (MODE=debug por padrão)"
	@echo "  make run       compila e abre a janela"
	@echo "  make clean     apaga build/"
	@echo ""
	@echo "  make run MODE=release   compila otimizado (-O2)"

-include $(DEPS)
