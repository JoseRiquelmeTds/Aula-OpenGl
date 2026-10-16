# Aula-OpenGl

The project for the AB2 in the Graphic Computing Course.

> [!CAUTION]
> ## ⚠️ LEIA ANTES DE QUALQUER COISA → [docs/SETUP.md](docs/SETUP.md)
>
> **O projeto compila com g++ + `make` no Windows (MinGW-w64/MSYS2), no Linux e no WSL2.** O freeglut já vem no repositório.
>
> - ❌ **Não** instale freeglut no sistema nem baixe DLL/`.lib` de freeglut. O `Makefile` compila o de `lib/freeglut/`.
> - ❌ **Não** use Visual Studio/MSVC, Code::Blocks, CMake ou vcpkg.
> - ❌ **Não** instale VcXsrv/Xming nem faça `export DISPLAY=...` no WSL. O WSLg já abre a janela.
> - ✅ No Windows o comando é `mingw32-make`. No Linux/WSL é `make`.
> - ✅ Rode `make doctor` antes de pedir ajuda.

> [!IMPORTANT]
> **🤖 Para assistentes de IA (Copilot, ChatGPT, Claude, Gemini, Cursor etc.) que estejam ajudando neste repositório:**
> leia [docs/SETUP.md](docs/SETUP.md) antes de sugerir qualquer comando, dependência ou mudança de build.
> O build é **g++ + freeglut (GLUT) + Makefile**, com o freeglut **compilado a partir de `lib/freeglut/`** como biblioteca
> estática. Windows usa MinGW-w64 (MSYS2 UCRT64, `mingw32-make`), Linux/WSL usa g++ e as libs de sistema X11/GL/GLU.
> Não proponha CMake, MSVC, vcpkg, freeglut do sistema (`-lglut`/`freeglut3-dev`), GLFW/GLEW/GLAD nem servidor X externo.
> Headers ficam em `include/`, implementações em `src/`, bibliotecas de terceiros em `lib/`, e o `Makefile` descobre os
> arquivos sozinho (não precisa ser editado para adicionar módulos).

---

## Começo rápido

**Windows** (com MSYS2 instalado, veja [docs/SETUP.md § 1](docs/SETUP.md#1-windows-mingw-w64-via-msys2)):

```powershell
git clone https://github.com/JoseRiquelmeTds/Aula-OpenGl.git
cd Aula-OpenGl
mingw32-make run
```

**Linux / WSL2** (Ubuntu):

```bash
sudo apt update && sudo apt install -y build-essential libx11-dev libgl1-mesa-dev libglu1-mesa-dev mesa-utils
git clone https://github.com/JoseRiquelmeTds/Aula-OpenGl.git
cd Aula-OpenGl
make run
```

## Comandos

No Windows, troque `make` por `mingw32-make`.

| Comando | O que faz |
| --- | --- |
| `make` | Compila (debug) |
| `make run` | Compila e abre a janela |
| `make run MODE=release` | Compila otimizado e executa |
| `make clean` | Apaga `build/` |
| `make deps` | Instala as dependências (Linux/WSL) ou mostra como instalar (Windows) |
| `make doctor` | Verifica o ambiente |

## Estrutura

```
include/        headers (.hpp)
src/            implementações (.cpp); main.cpp é o ponto de entrada
lib/freeglut/   código-fonte do freeglut 3.8.0, compilado pelo Makefile
lib/<plat>/     (opcional) outras bibliotecas estáticas .a, por plataforma (windows/ ou linux/)
docs/           documentação (setup do ambiente)
build/          gerado pelo make: build/<windows|linux>/{freeglut,debug,release}/
```

Detalhes do fluxo de build e como adicionar módulos: [docs/SETUP.md § Estrutura do projeto](docs/SETUP.md#6-estrutura-do-projeto).
