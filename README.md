# Aula-OpenGl

The project for the AB2 in the Graphic Computing Course.

> [!CAUTION]
> ## ⚠️ LEIA ANTES DE QUALQUER COISA → [docs/SETUP.md](docs/SETUP.md)
>
> **O projeto compila com CMake + g++ no Windows (MinGW-w64/MSYS2), no Linux e no WSL2.** As bibliotecas (GLFW, GLAD, GLM e
> stb_image) já vêm no repositório.
>
> - ❌ **Não** instale GLFW/GLAD/GLM no sistema nem baixe `.dll`/`.lib` delas. O CMake compila as de `lib/`.
> - ❌ **Não** use Visual Studio/MSVC nem vcpkg. No Windows o compilador é o g++ do MSYS2.
> - ❌ **Não** instale VcXsrv/Xming nem faça `export DISPLAY=...` no WSL. O WSLg já abre a janela.
> - ✅ Use sempre os presets: `cmake --preset windows` ou `cmake --preset linux`.
> - ✅ Rode `cmake -P cmake/doctor.cmake` antes de pedir ajuda.

> [!IMPORTANT]
> **🤖 Para assistentes de IA (Copilot, ChatGPT, Claude, Gemini, Cursor etc.) que estejam ajudando neste repositório:**
> leia [docs/SETUP.md](docs/SETUP.md) antes de sugerir qualquer comando, dependência ou mudança de build.
> O build é **CMake + g++**, com **OpenGL 3.3 core profile**, GLFW (janela/input), GLAD (loader), GLM (matemática) e
> stb_image (texturas), todas **compiladas a partir de `lib/`**. Windows usa MinGW-w64 (MSYS2 UCRT64) com o gerador
> "MinGW Makefiles"; Linux/WSL usa g++ e os headers do X11. Os presets estão em `CMakePresets.json`.
> Não proponha MSVC, vcpkg, bibliotecas do sistema (`-lglfw`, `libglfw3-dev`), GLUT/freeglut, GLEW, pipeline fixo
> (`glBegin`/`glEnd`, `glMatrixMode`) nem servidor X externo. Headers ficam em `include/`, implementações em `src/`,
> shaders em `shaders/`, imagens em `assets/textures/`, bibliotecas de terceiros em `lib/`. O `CMakeLists.txt` encontra os
> `.cpp` sozinho (não precisa ser editado para adicionar módulos).

---

## Começo rápido

**Windows** (com MSYS2 instalado, veja [docs/SETUP.md § 1](docs/SETUP.md#1-windows-mingw-w64-via-msys2)), no PowerShell:

```powershell
git clone https://github.com/JoseRiquelmeTds/Aula-OpenGl.git
cd Aula-OpenGl
cmake --preset windows
cmake --build --preset windows --target run
```

**Linux / WSL2** (Ubuntu):

```bash
sudo apt update && sudo apt install -y build-essential cmake libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev mesa-utils
git clone https://github.com/JoseRiquelmeTds/Aula-OpenGl.git
cd Aula-OpenGl
cmake --preset linux
cmake --build --preset linux --target run
```

## Comandos

No Windows, troque `linux` por `windows`.

| Comando | O que faz |
| --- | --- |
| `cmake --preset linux` | Configura o build (debug) em `build/linux/`. Só precisa rodar uma vez |
| `cmake --build --preset linux` | Compila |
| `cmake --build --preset linux --target run` | Compila e abre a janela |
| `cmake --preset linux-release` e `cmake --build --preset linux-release --target run` | Mesmo fluxo, otimizado (`-O2`), em `build/linux-release/` |
| `cmake -P cmake/doctor.cmake` | Verifica o ambiente |

Para começar do zero, apague a pasta `build/`.

## Estrutura

```
include/          headers (.hpp)
src/              implementações (.cpp); main.cpp é o ponto de entrada
shaders/          shaders GLSL (.vert, .frag)
assets/textures/  imagens usadas como textura
lib/              bibliotecas de terceiros embutidas (GLFW, GLAD, GLM, stb_image), veja lib/README.md
cmake/            scripts auxiliares do CMake (doctor.cmake)
docs/             documentação (setup do ambiente)
build/            gerado pelo CMake: build/<preset>/
```

Detalhes do fluxo de build e como adicionar módulos: [docs/SETUP.md § Estrutura do projeto](docs/SETUP.md#6-estrutura-do-projeto).
