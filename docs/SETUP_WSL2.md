# Setup do ambiente: WSL2 + Ubuntu + freeglut

Este é **o único ambiente suportado** pelo projeto. Todos compilam e rodam do mesmo jeito:
**Ubuntu no WSL2 → `make` → janela GLUT aberta pelo WSLg**.

> [!WARNING]
> **Não use** Visual Studio/MSVC, MinGW, Code::Blocks, freeglut para Windows, CMake, VcXsrv/Xming nem `export DISPLAY=<ip>:0`.
> Esses caminhos aparecem em tutoriais antigos e quebram o build ou a janela. Se uma IA sugerir algum deles, ela está errada para este repo.

---

## Sumário

1. [Requisitos](#1-requisitos)
2. [Instalar o WSL2 com Ubuntu](#2-instalar-o-wsl2-com-ubuntu)
3. [Clonar o repositório dentro do Linux](#3-clonar-o-repositório-dentro-do-linux)
4. [Instalar as dependências](#4-instalar-as-dependências)
5. [Verificar o ambiente](#5-verificar-o-ambiente)
6. [Compilar e rodar](#6-compilar-e-rodar)
7. [Estrutura do projeto](#7-estrutura-do-projeto)
8. [Problemas comuns](#8-problemas-comuns)

---

## 1. Requisitos

| Item | Versão |
| --- | --- |
| Windows | 11, ou 10 versão 21H2+ (build 19044+). É o que traz o **WSLg** (janelas Linux no Windows). |
| WSL | **versão 2** (a 1 não tem WSLg) |
| Distro | Ubuntu 22.04 ou 24.04 |
| Driver de vídeo | Atualizado (NVIDIA/AMD/Intel). Dá aceleração de GPU no WSL. Sem ele funciona, só que mais lento. |

## 2. Instalar o WSL2 com Ubuntu

No **PowerShell como administrador**:

```powershell
wsl --install -d Ubuntu
```

Reinicie o PC quando pedir. Na primeira abertura do Ubuntu, crie usuário e senha (a senha é a do `sudo`).

**Já tinha WSL instalado?** Atualize e confira a versão:

```powershell
wsl --update
wsl --shutdown
wsl -l -v
```

A coluna `VERSION` do Ubuntu precisa ser **2**. Se for 1:

```powershell
wsl --set-version Ubuntu 2
```

## 3. Clonar o repositório dentro do Linux

> [!IMPORTANT]
> Clone na **home do Linux** (`~`), **nunca** em `/mnt/c/...`.
> Em `/mnt/c` o build fica muito lento e o Git do Windows converte as quebras de linha para CRLF, o que quebra o `Makefile`.

Dentro do terminal do Ubuntu:

```bash
cd ~
git clone https://github.com/JoseRiquelmeTds/Aula-OpenGl.git
cd Aula-OpenGl
```

Para editar no VS Code do Windows, instale a extensão **WSL** (Microsoft) e rode `code .` dentro dessa pasta. O editor abre conectado ao Linux.

## 4. Instalar as dependências

Num Ubuntu recém-instalado nem o `make` existe ainda, então rode o `apt` direto:

```bash
sudo apt update
sudo apt install -y build-essential mesa-utils libglu1-mesa-dev freeglut3-dev
```

> Se `freeglut3-dev` não for encontrado (algumas versões renomearam o pacote), troque por `libglut-dev`.

O que cada pacote faz:

| Pacote | Para quê |
| --- | --- |
| `build-essential` | `g++`, `make`, `ar` |
| `freeglut3-dev` / `libglut-dev` | GLUT (`GL/glut.h`, `-lglut`) |
| `libglu1-mesa-dev` | GLU (`GL/glu.h`, `-lGLU`), que traz o OpenGL (`-lGL`) junto |
| `mesa-utils` | `glxinfo`, usado para conferir o driver |

Depois disso, `make deps` faz a mesma instalação (útil para reinstalar).

## 5. Verificar o ambiente

```bash
make doctor
```

Tudo precisa aparecer como `[ok]`. Exemplo:

```
== Sistema
  [ok] rodando no WSL
  [ok] repo no filesystem do Linux
== Ferramentas
  [ok] g++ (Ubuntu 13.2.0-23ubuntu4) 13.2.0
== GLUT / OpenGL
  [ok] GL/glut.h encontrado
  [ok] GL/glu.h encontrado
== Display (WSLg)
  [ok] DISPLAY=:0
  OpenGL renderer string: D3D12 (NVIDIA GeForce ...)
  OpenGL version string: 4.6 (Compatibility Profile) Mesa ...
```

`D3D12 (...)` no renderer = usando a GPU. `llvmpipe` = renderização por software (funciona, mas veja a [seção 8](#8-problemas-comuns)).

## 6. Compilar e rodar

```bash
make run
```

Deve abrir uma janela **"Aula OpenGL"** com um triângulo colorido. `Esc` ou `q` fecha.

| Comando | O que faz |
| --- | --- |
| `make` | Compila em modo debug (`-g -O0`) |
| `make run` | Compila e executa |
| `make run MODE=release` | Compila otimizado (`-O2`) e executa |
| `make clean` | Apaga `build/` |
| `make doctor` | Diagnóstico do ambiente |
| `make help` | Lista os alvos |

## 7. Estrutura do projeto

```
Aula-OpenGl/
├── Makefile              # build e execução
├── include/              # headers (.hpp): só declarações
│   └── renderer.hpp
├── src/                  # implementações (.cpp)
│   ├── main.cpp          # ponto de entrada (glutInit, callbacks, main loop)
│   └── renderer.cpp
├── lib/                  # bibliotecas estáticas de TERCEIROS (.a para Linux)
├── docs/
│   └── SETUP_WSL2.md     # este arquivo
└── build/                # gerado pelo make, ignorado pelo git
    └── debug/ | release/
        ├── obj/          # .o e .d (dependências de header)
        ├── lib/          # libaula-opengl.a: todo src/ exceto main.cpp
        └── bin/          # aula-opengl (executável)
```

Como o build funciona:

1. Cada `src/**/*.cpp` vira um `.o` em `build/<modo>/obj/`, compilado com `-Iinclude`.
2. Todos os `.o`, **exceto o `main.o`**, são empacotados na biblioteca estática `build/<modo>/lib/libaula-opengl.a`.
3. O executável é linkado como `main.o + libaula-opengl.a + lib/*.a + -lglut -lGLU -lGL`.

**Para adicionar um módulo** (ex.: `camera`): crie `include/camera.hpp` e `src/camera.cpp`, depois `#include "camera.hpp"` onde precisar.
**Não precisa editar o Makefile.** Ele encontra os `.cpp` sozinho, inclusive em subpastas (`src/shapes/cubo.cpp` → `include/shapes/cubo.hpp`).

## 8. Problemas comuns

<details>
<summary><code>freeglut (...): failed to open display ''</code> ou a janela não aparece</summary>

O WSLg não está ativo. No PowerShell:

```powershell
wsl --update
wsl --shutdown
```

Abra o Ubuntu de novo e rode `echo $DISPLAY`. Tem que aparecer `:0`.
Se você colocou `export DISPLAY=...` no `~/.bashrc` por causa de um tutorial antigo (VcXsrv/Xming), **apague essa linha**.
</details>

<details>
<summary><code>fatal error: GL/glut.h: No such file or directory</code> ou <code>cannot find -lglut</code></summary>

Faltam as dependências. Rode a [seção 4](#4-instalar-as-dependências) (ou `make deps`).
</details>

<details>
<summary><code>Makefile:…: *** missing separator</code> ou <code>$'\r': command not found</code></summary>

Os arquivos estão com quebra de linha do Windows (CRLF). Quase sempre é porque o repo foi clonado em `/mnt/c` ou pelo Git do Windows.
Apague e clone de novo **dentro do Ubuntu** (`cd ~ && git clone ...`).
O `.gitattributes` do repo força LF nos clones feitos no Linux.
</details>

<details>
<summary>Janela preta, lenta, ou <code>glxinfo -B</code> mostra <code>llvmpipe</code></summary>

O OpenGL está rodando na CPU. Atualize o driver de vídeo **no Windows** (não instale driver dentro do Ubuntu) e rode `wsl --shutdown`.
Se a GPU der problema, force software para testar:

```bash
LIBGL_ALWAYS_SOFTWARE=1 make run
```
</details>

<details>
<summary><code>make: command not found</code></summary>

Instale o `build-essential` (seção 4).
</details>
