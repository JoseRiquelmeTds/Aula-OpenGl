# =============================================================================
#  Aula-OpenGl — build com Makefile (C++ + freeglut embutido no repositório)
#  Roda no Windows (MinGW-w64 via MSYS2), no Linux e no WSL2.
#  Setup do ambiente: docs/SETUP.md
# =============================================================================
#
#  Fluxo de compilação (<plat> = windows | linux, <modo> = debug | release):
#    lib/freeglut/src/**/*.c        -> build/<plat>/freeglut/obj/*.o -> build/<plat>/freeglut/libfreeglut.a
#    src/**/*.cpp (exceto main.cpp) -> build/<plat>/<modo>/obj/*.o   -> build/<plat>/<modo>/lib/libaula-opengl.a
#    src/main.cpp                   -> build/<plat>/<modo>/obj/main.o
#    main.o + libaula-opengl.a + lib/<plat>/*.a + libfreeglut.a + OpenGL do sistema
#                                   -> build/<plat>/<modo>/bin/aula-opengl[.exe]
#
#  Uso:  make help          (no Windows: mingw32-make help)

APP := aula-opengl

# --- Plataforma --------------------------------------------------------------
ifeq ($(OS),Windows_NT)
  PLATFORM := windows
  EXE      := .exe
else
  PLATFORM := linux
  EXE      :=
  IS_WSL   := $(findstring microsoft,$(shell uname -r | tr A-Z a-z))
endif

# No Windows, dentro do Git Bash/MSYS2 (MSYSTEM definido) as receitas rodam no sh;
# no PowerShell/cmd rodam no cmd.exe. Os comandos de arquivo abaixo cobrem os dois casos.
ifeq ($(PLATFORM),windows)
  ifeq ($(MSYSTEM),)
    SHELL     := cmd.exe
    CMD_SHELL := 1
  else
    SHELL     := sh.exe
  endif
endif

ifdef CMD_SHELL
  mkdir_p = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
  rm_rf   = if exist "$(subst /,\,$(1))" rmdir /s /q "$(subst /,\,$(1))"
  exec    = $(subst /,\,$(1))
  which   = $(shell where $(1) 2>nul)
else
  mkdir_p = mkdir -p $(1)
  rm_rf   = rm -rf $(1)
  exec    = ./$(1)
  which   = $(shell command -v $(1) 2>/dev/null)
endif

# Busca recursiva de arquivos sem depender de find (que não existe no cmd)
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$(d),$(2)) $(filter $(subst *,%,$(2)),$(d)))

# --- Diretórios --------------------------------------------------------------
SRC_DIR := src
INC_DIR := include
LIB_DIR := lib
MODE    ?= debug
BUILD   := build/$(PLATFORM)/$(MODE)
OBJ_DIR := $(BUILD)/obj
OUT_LIB := $(BUILD)/lib
BIN_DIR := $(BUILD)/bin

TARGET   := $(BIN_DIR)/$(APP)$(EXE)
CORE_LIB := $(OUT_LIB)/lib$(APP).a

# --- Fontes do projeto -------------------------------------------------------
MAIN_SRC := $(SRC_DIR)/main.cpp
LIB_SRCS := $(filter-out $(MAIN_SRC),$(call rwildcard,$(SRC_DIR),*.cpp))
LIB_OBJS := $(LIB_SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
MAIN_OBJ := $(OBJ_DIR)/main.o
DEPS     := $(LIB_OBJS:.o=.d) $(MAIN_OBJ:.o=.d)

# Bibliotecas estáticas pré-compiladas de terceiros, separadas por plataforma
EXT_LIBS := $(wildcard $(LIB_DIR)/$(PLATFORM)/*.a)

# --- freeglut (código-fonte em lib/freeglut, compilado como biblioteca estática)
FG_DIR   := $(LIB_DIR)/freeglut
FG_BUILD := build/$(PLATFORM)/freeglut
FG_LIB   := $(FG_BUILD)/libfreeglut.a
FG_SRCS  := $(wildcard $(FG_DIR)/src/*.c)
ifeq ($(PLATFORM),windows)
  FG_SRCS += $(wildcard $(FG_DIR)/src/mswin/*.c) $(FG_DIR)/src/util/xparsegeometry_repl.c
else
  FG_SRCS += $(wildcard $(FG_DIR)/src/x11/*.c)
endif
FG_OBJS  := $(FG_SRCS:$(FG_DIR)/src/%.c=$(FG_BUILD)/obj/%.o)
FG_DEPS  := $(FG_OBJS:.o=.d)

# --- Ferramentas e flags -----------------------------------------------------
ifeq ($(origin CC),default)
  CC := gcc
endif
AR       := ar
ARFLAGS  := rcs

FG_CPPFLAGS := -I$(FG_DIR)/include -I$(FG_DIR)/src -I$(FG_DIR)/config/$(PLATFORM) \
               -DHAVE_CONFIG_H -DFREEGLUT_STATIC -MMD -MP
FG_CFLAGS   := -O2 -w

CPPFLAGS := -I$(INC_DIR) -I$(FG_DIR)/include -DFREEGLUT_STATIC -MMD -MP
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic

ifeq ($(PLATFORM),windows)
  FG_CPPFLAGS += -D_WIN32_WINNT=0x0601 -DWINVER=0x0601 -D_CRT_SECURE_NO_WARNINGS
  # -static: o .exe não depende das DLLs do MinGW e abre com dois cliques
  LDFLAGS := -static
  LDLIBS  := -lglu32 -lopengl32 -lgdi32 -lwinmm
else
  LDFLAGS :=
  LDLIBS  := -lGLU -lGL -lX11 -lm
endif

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

$(TARGET): $(MAIN_OBJ) $(CORE_LIB) $(EXT_LIBS) $(FG_LIB)
	@$(call mkdir_p,$(@D))
	$(CXX) $(LDFLAGS) $(MAIN_OBJ) $(CORE_LIB) $(EXT_LIBS) $(FG_LIB) $(LDLIBS) -o $@

$(CORE_LIB): $(LIB_OBJS)
	@$(call mkdir_p,$(@D))
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(call mkdir_p,$(@D))
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(FG_LIB): $(FG_OBJS)
	@$(call mkdir_p,$(@D))
	@echo Empacotando $@
	@$(AR) $(ARFLAGS) $@ $^

# freeglut é código de terceiros: compila em silêncio e sem warnings
$(FG_BUILD)/obj/%.o: $(FG_DIR)/src/%.c
	@$(call mkdir_p,$(@D))
	@echo freeglut: $(notdir $<)
	@$(CC) $(FG_CPPFLAGS) $(FG_CFLAGS) -c $< -o $@

run: $(TARGET)
	$(call exec,$(TARGET))

clean:
	@$(call rm_rf,build)

# --- Ambiente ----------------------------------------------------------------
ifeq ($(PLATFORM),windows)
deps:
	$(info O compilador do Windows vem do MSYS2 e não dá para instalar pelo make.)
	$(info Siga docs/SETUP.md seção Windows. Resumo no terminal "MSYS2 UCRT64":)
	$(info $(pad)pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make)
	$(info e adicione C:\msys64\ucrt64\bin ao PATH do Windows.)
	@exit 0
else
# Instala as dependências no Ubuntu/Debian (pede senha do sudo)
deps:
	sudo apt-get update
	sudo apt-get install -y build-essential libx11-dev libgl1-mesa-dev libglu1-mesa-dev mesa-utils
endif

# Os alvos abaixo imprimem com $(info), que funciona igual no cmd e no sh. O make tira
# espaços do início do texto, então a indentação vem de $(pad). "@exit 0" é um comando
# vazio válido nos dois shells e evita o aviso "Nothing to be done".
empty :=
pad   := $(empty)  $(empty)

# check(condição, mensagem ok, mensagem de erro) — mensagens sem vírgula
check = $(info $(pad)$(if $(strip $(1)),[ok] $(2),[!!] $(3)))

# Diagnóstico do ambiente: rode isto antes de pedir ajuda
doctor:
	$(info == Sistema)
	$(info $(pad)plataforma: $(PLATFORM)$(if $(IS_WSL), (WSL)))
	$(if $(CMD_SHELL),$(info $(pad)shell: cmd.exe),$(info $(pad)shell: sh))
	$(if $(IS_WSL),$(call check,$(filter-out /mnt/%,$(CURDIR)),repo no filesystem do Linux,repo em /mnt: o build fica lento. Prefira clonar em ~/ dentro do WSL))
	$(info == Ferramentas)
	$(call check,$(call which,$(CXX)),$(CXX) $(shell $(CXX) -dumpfullversion) ($(shell $(CXX) -dumpmachine)),$(CXX) não encontrado: veja docs/SETUP.md)
	$(call check,$(call which,$(AR)),$(AR) encontrado,$(AR) não encontrado: veja docs/SETUP.md)
	$(info $(pad)make $(MAKE_VERSION))
	$(info == freeglut)
	$(call check,$(wildcard $(FG_DIR)/src/fg_init.c),fontes em $(FG_DIR),$(FG_DIR) ausente: o clone está incompleto)
	$(call check,$(wildcard $(FG_LIB)),$(FG_LIB) compilada,$(FG_LIB) ainda não compilada: rode make)
ifeq ($(PLATFORM),linux)
	$(info == Headers do sistema)
	$(call check,$(wildcard /usr/include/X11/Xlib.h),X11/Xlib.h,X11/Xlib.h ausente: rode make deps)
	$(call check,$(wildcard /usr/include/GL/gl.h),GL/gl.h,GL/gl.h ausente: rode make deps)
	$(call check,$(wildcard /usr/include/GL/glu.h),GL/glu.h,GL/glu.h ausente: rode make deps)
	$(info == Display)
	$(call check,$(DISPLAY),DISPLAY=$(DISPLAY),DISPLAY vazio: no WSL rode wsl --update e wsl --shutdown no PowerShell)
	$(call check,$(call which,glxinfo),$(shell glxinfo -B 2>/dev/null | grep "OpenGL renderer"),glxinfo indisponível: rode make deps)
endif
	@exit 0

help:
	$(info Alvos (no Windows troque make por mingw32-make):)
	$(info $(pad)make           compila (MODE=debug por padrão))
	$(info $(pad)make run       compila e abre a janela)
	$(info $(pad)make clean     apaga build/)
	$(info $(pad)make doctor    verifica se o ambiente está pronto)
	$(info $(pad)make deps      instala as dependências (Linux/WSL) ou mostra como instalar (Windows))
	$(info )
	$(info $(pad)make run MODE=release   compila otimizado (-O2))
	@exit 0

-include $(DEPS) $(FG_DEPS)
