# Setup do ambiente: Windows, Linux ou WSL2

O projeto compila com **g++ + `make`** em três ambientes, todos com o mesmo `Makefile`:

| Ambiente | Compilador | Comando do make | Janela |
| --- | --- | --- | --- |
| **Windows** (nativo) | MinGW-w64 do MSYS2 (UCRT64) | `mingw32-make` | Win32 |
| **Linux** (Ubuntu/Debian) | g++ do sistema | `make` | X11 |
| **WSL2** (Ubuntu) | g++ do sistema | `make` | WSLg |

O **freeglut já vem no repositório** (`lib/freeglut/`, código-fonte da versão 3.8.0). O `Makefile` compila ele na primeira vez
como biblioteca estática (`build/<plataforma>/freeglut/libfreeglut.a`) e linka no executável. **Não instale freeglut no sistema**,
nem baixe DLL/`.lib` de freeglut para Windows. Do sistema vêm só o OpenGL e a GLU (e o X11 no Linux).

> [!WARNING]
> **Não use** Visual Studio/MSVC, Code::Blocks, CMake, vcpkg, freeglut pré-compilado, VcXsrv/Xming nem `export DISPLAY=<ip>:0`.
> Esses caminhos aparecem em tutoriais antigos e não são o build deste repo. Se uma IA sugerir algum deles, ela está errada para este projeto.

---

## Sumário

1. [Windows (MinGW-w64 via MSYS2)](#1-windows-mingw-w64-via-msys2)
2. [Linux (Ubuntu/Debian)](#2-linux-ubuntudebian)
3. [WSL2](#3-wsl2)
4. [Verificar o ambiente](#4-verificar-o-ambiente)
5. [Compilar e rodar](#5-compilar-e-rodar)
6. [Estrutura do projeto](#6-estrutura-do-projeto)
7. [Problemas comuns](#7-problemas-comuns)

---

## 1. Windows (MinGW-w64 via MSYS2)

1. Instale o **Git for Windows** (<https://git-scm.com/download/win>) e o **MSYS2** (<https://www.msys2.org>), ou pelo PowerShell:

   ```powershell
   winget install Git.Git MSYS2.MSYS2
   ```

2. Abra o terminal **"MSYS2 UCRT64"** (menu Iniciar) e instale o compilador e o make:

   ```bash
   pacman -Syu
   pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make
   ```

   Se o `pacman -Syu` fechar o terminal, abra de novo e rode a segunda linha.

3. Adicione **`C:\msys64\ucrt64\bin`** ao `Path` do Windows (Iniciar → "variáveis de ambiente" → `Path` do usuário → Novo).
   Feche e abra o terminal depois disso.

4. No **PowerShell** (ou cmd, Git Bash, terminal do VS Code), confira e clone:

   ```powershell
   g++ --version          # deve mostrar "Built by MSYS2 project"
   mingw32-make --version
   git clone https://github.com/JoseRiquelmeTds/Aula-OpenGl.git
   cd Aula-OpenGl
   mingw32-make run
   ```

> No Windows o comando é **`mingw32-make`**, não `make`. Todos os exemplos com `make` deste repo valem trocando o nome.

O `.exe` é linkado com `-static`: só depende de DLLs do próprio Windows, então abre com dois cliques e roda em outro PC sem MSYS2.

## 2. Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install -y build-essential libx11-dev libgl1-mesa-dev libglu1-mesa-dev mesa-utils
git clone https://github.com/JoseRiquelmeTds/Aula-OpenGl.git
cd Aula-OpenGl
make run
```

Depois da primeira vez, `make deps` faz a mesma instalação do `apt`.

| Pacote | Para quê |
| --- | --- |
| `build-essential` | `g++`, `gcc`, `make`, `ar` |
| `libx11-dev` | X11 (`X11/Xlib.h`, `-lX11`), usado pelo freeglut para abrir a janela |
| `libgl1-mesa-dev` | OpenGL (`GL/gl.h`, `-lGL`) |
| `libglu1-mesa-dev` | GLU (`GL/glu.h`, `-lGLU`) |
| `mesa-utils` | `glxinfo`, usado pelo `make doctor` para conferir o driver |

Outras distros: instale os equivalentes (Fedora: `gcc-c++ make libX11-devel mesa-libGL-devel mesa-libGLU-devel glx-utils`).

## 3. WSL2

Igual ao Linux (seção 2), rodando dentro do Ubuntu do WSL2. A janela abre pelo **WSLg**, sem servidor X extra.

Requisitos: Windows 11, ou Windows 10 21H2+ (build 19044+), com WSL **versão 2**. No **PowerShell como administrador**:

```powershell
wsl --install -d Ubuntu     # primeira instalação (reinicie quando pedir)
wsl --update                # se já tinha WSL
wsl --shutdown
wsl -l -v                   # a coluna VERSION precisa ser 2
```

Dicas:

- Clone na **home do Linux** (`cd ~`). Em `/mnt/c/...` funciona, mas o build fica bem mais lento.
- Pode usar **o mesmo clone** no Windows e no WSL: o build de cada plataforma vai para uma pasta própria (`build/windows/`, `build/linux/`).
- Para editar no VS Code do Windows, instale a extensão **WSL** (Microsoft) e rode `code .` dentro da pasta.

## 4. Verificar o ambiente

```bash
make doctor            # Windows: mingw32-make doctor
```

Tudo precisa aparecer como `[ok]`. Exemplo no WSL:

```
== Sistema
  plataforma: linux (WSL)
  shell: sh
  [ok] repo no filesystem do Linux
== Ferramentas
  [ok] g++ 13.3.0 (x86_64-linux-gnu)
  [ok] ar encontrado
  make 4.3
== freeglut
  [ok] fontes em lib/freeglut
  [ok] build/linux/freeglut/libfreeglut.a compilada
== Headers do sistema
  [ok] X11/Xlib.h
  [ok] GL/gl.h
  [ok] GL/glu.h
== Display
  [ok] DISPLAY=:0
  [ok] OpenGL renderer string: D3D12 (NVIDIA GeForce ...)
```

No WSL, `D3D12 (...)` no renderer = usando a GPU. `llvmpipe` = renderização por software (funciona, mas veja a [seção 7](#7-problemas-comuns)).
A linha `libfreeglut.a ainda não compilada` antes do primeiro `make` é normal.

## 5. Compilar e rodar

```bash
make run               # Windows: mingw32-make run
```

Deve abrir uma janela **"Aula OpenGL"** com um triângulo colorido. `Esc` ou `q` fecha.
A primeira compilação demora alguns segundos a mais porque compila o freeglut. As próximas reaproveitam a biblioteca.

| Comando | O que faz |
| --- | --- |
| `make` | Compila em modo debug (`-g -O0`) |
| `make run` | Compila e executa |
| `make run MODE=release` | Compila otimizado (`-O2`) e executa |
| `make clean` | Apaga `build/` (inclusive o freeglut compilado) |
| `make doctor` | Diagnóstico do ambiente |
| `make deps` | Instala as dependências (Linux/WSL) ou mostra como instalar (Windows) |
| `make help` | Lista os alvos |

## 6. Estrutura do projeto

```
Aula-OpenGl/
├── Makefile              # build e execução (Windows, Linux e WSL)
├── include/              # headers (.hpp): só declarações
│   └── renderer.hpp
├── src/                  # implementações (.cpp)
│   ├── main.cpp          # ponto de entrada (glutInit, callbacks, main loop)
│   └── renderer.cpp
├── lib/                  # bibliotecas de TERCEIROS
│   ├── freeglut/         # código-fonte do freeglut 3.8.0 (compilado pelo Makefile)
│   ├── windows/          # (opcional) .a pré-compilados para MinGW-w64
│   └── linux/            # (opcional) .a pré-compilados para Linux x86-64
├── docs/
│   └── SETUP.md          # este arquivo
└── build/                # gerado pelo make, ignorado pelo git
    └── windows/ | linux/
        ├── freeglut/     # libfreeglut.a (compilada uma vez por plataforma)
        └── debug/ | release/
            ├── obj/      # .o e .d (dependências de header)
            ├── lib/      # libaula-opengl.a: todo src/ exceto main.cpp
            └── bin/      # aula-opengl(.exe)
```

Como o build funciona:

1. Os `.c` do freeglut (os comuns + `mswin/` no Windows ou `x11/` no Linux) viram `build/<plataforma>/freeglut/libfreeglut.a`.
   A configuração de cada plataforma fica em `lib/freeglut/config/<plataforma>/config.h`.
2. Cada `src/**/*.cpp` vira um `.o` em `build/<plataforma>/<modo>/obj/`, compilado com `-Iinclude -Ilib/freeglut/include`.
3. Todos os `.o` do projeto, **exceto o `main.o`**, são empacotados em `libaula-opengl.a`.
4. O executável é linkado como `main.o + libaula-opengl.a + lib/<plataforma>/*.a + libfreeglut.a` + bibliotecas do sistema:
   - Windows: `-lglu32 -lopengl32 -lgdi32 -lwinmm` (e `-static`)
   - Linux/WSL: `-lGLU -lGL -lX11 -lm`

**Para adicionar um módulo** (ex.: `camera`): crie `include/camera.hpp` e `src/camera.cpp`, depois `#include "camera.hpp"` onde precisar.
**Não precisa editar o Makefile.** Ele encontra os `.cpp` sozinho, inclusive em subpastas (`src/shapes/cubo.cpp` → `include/shapes/cubo.hpp`).

Nos `.cpp`, continue usando `#include <GL/glut.h>` (ou `<GL/freeglut.h>`). O `-Ilib/freeglut/include` faz esse include pegar o freeglut do repo.

## 7. Problemas comuns

<details>
<summary>Windows: <code>mingw32-make</code> ou <code>g++</code> não é reconhecido como comando</summary>

O `C:\msys64\ucrt64\bin` não está no `Path`, ou o terminal foi aberto antes de você mudar o `Path`. Refaça o passo 3 da [seção 1](#1-windows-mingw-w64-via-msys2) e abra um terminal novo.
Se instalou o pacote `make` do MSYS2 em vez do `mingw-w64-ucrt-x86_64-make`, o comando `make` só existe dentro do terminal MSYS2.
</details>

<details>
<summary>Windows: <code>undefined reference to `__imp_glut...`</code> ou erros de link com <code>glut</code></summary>

Tem outro freeglut (DLL/`.lib` baixado de tutorial) sendo usado no lugar do do repo. Não passe `-lfreeglut`/`-lglut`, não copie `freeglut.dll`
para o projeto e compile só pelo `mingw32-make`. Rode `mingw32-make clean` e compile de novo.
</details>

<details>
<summary>Linux/WSL: <code>fatal error: X11/Xlib.h</code>, <code>GL/gl.h</code> ou <code>GL/glu.h: No such file or directory</code></summary>

Faltam as dependências do sistema. Rode `make deps` ou o `apt install` da [seção 2](#2-linux-ubuntudebian).
</details>

<details>
<summary>WSL: <code>freeglut (...): failed to open display ''</code> ou a janela não aparece</summary>

O WSLg não está ativo. No PowerShell:

```powershell
wsl --update
wsl --shutdown
```

Abra o Ubuntu de novo e rode `echo $DISPLAY`. Tem que aparecer `:0`.
Se você colocou `export DISPLAY=...` no `~/.bashrc` por causa de um tutorial antigo (VcXsrv/Xming), **apague essa linha**.
</details>

<details>
<summary><code>Makefile:…: *** missing separator</code> ou <code>$'\r': command not found</code></summary>

Os arquivos estão com quebra de linha do Windows (CRLF). O `.gitattributes` do repo força LF em todo clone, então isso só acontece
se algum editor salvou o arquivo com CRLF. No VS Code, clique em `CRLF` na barra inferior, troque para `LF` e salve.
</details>

<details>
<summary>WSL: janela preta, lenta, ou <code>make doctor</code> mostra <code>llvmpipe</code></summary>

O OpenGL está rodando na CPU. Atualize o driver de vídeo **no Windows** (não instale driver dentro do Ubuntu) e rode `wsl --shutdown`.
Se a GPU der problema, force software para testar:

```bash
LIBGL_ALWAYS_SOFTWARE=1 make run
```
</details>
