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

## Input e atualização por frame (CG-54)

`Input` (`include/input.hpp`, `src/input.cpp`) acompanha uma janela GLFW. O loop chama
`input.beginFrame()` antes de `glfwPollEvents()` e depois entrega o estado a `update` em `src/main.cpp`.
Todos os módulos podem consultar o mesmo frame, sem consumir os eventos:

| Consulta | Resultado |
| --- | --- |
| `keyDown(GLFW_KEY_...)` / `mouseDown(GLFW_MOUSE_BUTTON_...)` | Estado contínuo de tecla/botão |
| `keyPressed(...)` / `mousePressed(...)` | Pressionamento neste frame, sem repetição automática; preservado mesmo se houver soltura no mesmo poll |
| `mouseDelta()` | Movimento relativo acumulado no frame com cursor capturado; `x` para a direita, `y` para baixo |
| `setCursorCaptured(bool)` / `isCursorCaptured()` | Controle/consulta da captura (`GLFW_CURSOR_DISABLED`) |
| `isFocused()` / `focusChanged()` | Foco atual e ocorrência de transição neste frame |

As consultas de teclas/botões inválidos retornam `false`. Os eventos de pressionamento e o delta do mouse
duram até o próximo `beginFrame()`. O primeiro evento de posição após cada captura apenas estabelece a
referência, evitando saltos. A perda ou recuperação de foco limpa os estados e o movimento pendente;
o cursor volta livre e exige novo clique para capturar. Teclas mantidas antes da perda de foco exigem
novo pressionamento ao retornar.

Os atalhos ficam em `update`: clique esquerdo captura, `Esc` libera a captura ou fecha se o cursor já estiver
livre, e `Q` fecha sempre. `Input` não conhece esses atalhos. Ele usa o user pointer e os callbacks de teclado,
botões, posição e foco da janela; outros módulos devem usar sua interface sem substituir esses callbacks.
O callback de framebuffer continua independente. O objeto deve ser destruído antes da janela.

`update` recebe `deltaTime` em **segundos**, medido com `glfwGetTime()` após o poll e antes do desenho.
O primeiro frame, frames sem foco e frames com transição de foco recebem zero. Nos demais frames,
o intervalo real é limitado a `[0, 0.1]` segundo: o excesso de uma pausa longa é descartado, sem acumular
recuperação nos próximos frames. Não há passo fixo nem cálculo baseado em FPS.

## Câmera livre e inspeção (CG-55)

| Controle | Ação |
| --- | --- |
| Clique esquerdo | Captura o cursor e ativa a navegação com a janela em foco |
| `W` / `S` | Avança / recua na direção da câmera, incluindo a inclinação |
| `A` / `D` | Move para a esquerda / direita |
| `Espaço` / `Ctrl esquerdo` | Sobe / desce no eixo Y do mundo |
| Mouse capturado | Gira horizontalmente (yaw) e verticalmente (pitch limitado a ±89°) |
| `1` / `2` / `3` | Cena completa / cubo isolado / objeto articulado isolado |
| `Esc` / `Q` | Mantêm os atalhos descritos acima |

A seleção funciona com a janela em foco, mesmo com o cursor livre, e restaura um enquadramento
inicial para cada modo. Depois a câmera continua livre. A perda de foco libera o cursor e exige
novo clique para retomar a navegação; com o cursor livre não há movimento nem rotação pelo mouse.

`Camera` (`include/camera.hpp`, `src/camera.cpp`) guarda posição e orientação e calcula view/projeção
sem depender do GLFW. `update` conecta o `Input` à câmera; `renderer::draw(camera)` usa suas matrizes
com a proporção atual do framebuffer e suspende o desenho quando a largura ou altura é zero.
`movementSpeed` (padrão **2 unidades/s**) e `mouseSensitivity` (**0,1 grau/pixel**) são configuráveis
na câmera. O movimento combinado é normalizado e usa `deltaTime`; o delta do mouse **não** usa
`deltaTime`. Não há colisões, gravidade ou limite de deslocamento.
