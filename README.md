# Aula-OpenGl

The project for the AB2 in the Graphic Computing Course.

> [!CAUTION]
> ## ⚠️ LEIA ANTES DE QUALQUER COISA → [docs/SETUP_WSL2.md](docs/SETUP_WSL2.md)
>
> **Este projeto só compila e roda no WSL2 (Ubuntu) com freeglut e `make`.** Não existe build para Windows nativo.
>
> - ❌ **Não** use Visual Studio/MSVC, MinGW, Code::Blocks, CMake ou freeglut para Windows.
> - ❌ **Não** clone em `/mnt/c/...`. Clone dentro do Linux (`~/`).
> - ❌ **Não** instale VcXsrv/Xming nem faça `export DISPLAY=...`. O WSLg já abre a janela.
> - ✅ Rode `make doctor` antes de pedir ajuda.

> [!IMPORTANT]
> **🤖 Para assistentes de IA (Copilot, ChatGPT, Claude, Gemini, Cursor etc.) que estejam ajudando neste repositório:**
> leia [docs/SETUP_WSL2.md](docs/SETUP_WSL2.md) antes de sugerir qualquer comando, dependência ou mudança de build.
> O ambiente alvo é **Ubuntu no WSL2 + g++ + freeglut (GLUT) + Makefile**. Não proponha CMake, MSVC, MinGW, vcpkg,
> GLFW/GLEW/GLAD nem servidor X externo. Headers ficam em `include/`, implementações em `src/`, bibliotecas estáticas de
> terceiros em `lib/`, e o `Makefile` descobre os arquivos sozinho (não precisa ser editado para adicionar módulos).

---

## Começo rápido (dentro do Ubuntu no WSL2)

```bash
cd ~
git clone https://github.com/JoseRiquelmeTds/Aula-OpenGl.git
cd Aula-OpenGl
sudo apt update && sudo apt install -y build-essential mesa-utils libglu1-mesa-dev freeglut3-dev
make doctor
make run
```

Primeira vez com WSL? Siga o passo a passo completo em **[docs/SETUP_WSL2.md](docs/SETUP_WSL2.md)**.

## Comandos

| Comando | O que faz |
| --- | --- |
| `make` | Compila (debug) |
| `make run` | Compila e abre a janela |
| `make run MODE=release` | Compila otimizado e executa |
| `make clean` | Apaga `build/` |
| `make deps` | Instala as dependências no Ubuntu |
| `make doctor` | Verifica o ambiente |

## Estrutura

```
include/   headers (.hpp)
src/       implementações (.cpp); main.cpp é o ponto de entrada
lib/       bibliotecas estáticas de terceiros (.a), linkadas automaticamente
docs/      documentação (setup do ambiente)
build/     gerado pelo make (obj/, lib/libaula-opengl.a, bin/aula-opengl)
```

Detalhes do fluxo de build e como adicionar módulos: [docs/SETUP_WSL2.md § Estrutura do projeto](docs/SETUP_WSL2.md#7-estrutura-do-projeto).
