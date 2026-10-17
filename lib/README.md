# lib/: bibliotecas de terceiros

Todas vêm **embutidas no repositório** (código-fonte) e são compiladas pelo CMake junto com o projeto.
Nenhuma precisa ser instalada no sistema. As licenças permitem redistribuir o código, desde que o arquivo de licença
fique junto, então **não apague** os arquivos `LICENSE*`/`copying.txt`.

| Pasta | Biblioteca | Versão | Para quê | Licença |
| --- | --- | --- | --- | --- |
| `glfw/` | [GLFW](https://www.glfw.org) | 3.5.1 | janela, contexto OpenGL, teclado e mouse | zlib ([LICENSE.md](glfw/LICENSE.md)) |
| `glad/` | [GLAD](https://github.com/Dav1dde/glad) | gerador 2.0.8 | carrega as funções do **OpenGL 3.3 core** | código gerado: CC0/WTFPL + Apache-2.0 (especificação Khronos); `khrplatform.h`: MIT |
| `glm/` | [GLM](https://github.com/g-truc/glm) | 1.0.3 | matemática: vetores, matrizes, transformações | MIT / Happy Bunny ([copying.txt](glm/copying.txt)) |
| `stb/` | [stb_image](https://github.com/nothings/stb) | commit `2c980bb` | leitura de PNG/JPG para texturas | MIT ou domínio público ([LICENSE](stb/LICENSE)) |

Os alvos CMake estão em [CMakeLists.txt](CMakeLists.txt): `glfw`, `glad`, `glm` e `stb_image`. O projeto já linka os quatro,
então no código basta incluir:

```cpp
#include <glad/gl.h>       // sempre antes de qualquer outro header de OpenGL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb_image.h>
```

## Origem e o que foi mantido

| Pasta | Origem | Mantido | Removido |
| --- | --- | --- | --- |
| `glfw/` | `glfw-3.5.1.zip` do release oficial (SHA-256 `ea79bc5feffc254c87291980c2d0bce9acebb68c4983b79f961dcd2cb8a611a0`) | `CMakeLists.txt`, `CMake/`, `include/`, `src/`, `LICENSE.md` | `docs/`, `examples/`, `tests/`, `deps/`, `README.md`, `CONTRIBUTORS.md` |
| `glad/` | gerado com `glad --api gl:core=3.3 --extensions= --reproducible c` (pacote `glad2==2.0.8`) | tudo o que o gerador produziu | nada |
| `glm/` | `glm-1.0.3.zip` do release oficial (SHA-256 `1c0a0fced9b0d87c7b7bc94e40be490cff6d4c83c25db8488d8f33754e7fdeb2`) | `glm/` (headers), `copying.txt` | `CMakeLists.txt` da raiz, `test/`, `doc/`, `util/` |
| `stb/` | `stb_image.h` e `LICENSE` do commit [`2c980bb`](https://github.com/nothings/stb/tree/2c980bb59875b0d32144a71867fbdebb2f77cd20) (SHA-256 do header `594c2fe35d49488b4382dbfaec8f98366defca819d916ac95becf3e75f4200b3`) | os dois arquivos | o resto do repositório stb |

Nenhum arquivo de terceiros foi modificado. Os arquivos **nossos** em `lib/` são só este `README.md`, o `CMakeLists.txt`
e `stb/stb_image.c`, que compila a implementação do stb_image uma única vez. Nos `.cpp` do projeto,
**não** defina `STB_IMAGE_IMPLEMENTATION`.

No Linux o GLFW é compilado só com o backend X11 (`GLFW_BUILD_WAYLAND OFF`). O WSLg e os desktops Wayland abrem janelas X11
pelo XWayland, e assim o build não depende dos pacotes de desenvolvimento do Wayland.

## Atualizar uma biblioteca

1. Baixe a versão nova do release oficial e confira o SHA-256.
2. Substitua os arquivos seguindo a tabela acima (para o GLAD, gere de novo com o mesmo comando, mudando só a versão do gerador).
3. Apague `build/` e compile no Windows e no Linux (`cmake --preset ...` e `cmake --build --preset ... --target run`).
4. Atualize a versão e o SHA-256 neste arquivo.
