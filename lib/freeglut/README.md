# freeglut 3.8.0 (embutido)

Cópia do código-fonte do [freeglut](https://github.com/freeglut/freeglut) **3.8.0**, do release oficial
`freeglut-3.8.0.tar.gz` (SHA-256 `674dcaff25010e09e450aec458b8870d9e98c46f99538db457ab659b321d9989`).
Licença MIT/X11 em [COPYING](COPYING).

O `Makefile` da raiz compila estes arquivos como biblioteca estática (`build/<plataforma>/freeglut/libfreeglut.a`)
com `-DFREEGLUT_STATIC`, sem CMake.

## O que foi mantido

| Caminho | Uso |
| --- | --- |
| `include/GL/` | headers públicos (`glut.h`, `freeglut.h`, ...) |
| `src/*.c`, `src/*.h` | código comum a todas as plataformas (sem `gles_stubs.c`, que é só para OpenGL ES) |
| `src/mswin/` | backend do Windows (Win32 + WGL) |
| `src/x11/` | backend do Linux (X11 + GLX) |
| `src/egl/*.h` | só os headers, porque o backend X11 inclui eles mesmo sem EGL |
| `src/util/` | `XParseGeometry` para o Windows, que não tem X11 |
| `config/<plataforma>/config.h` | substitui o `config.h` que o CMake geraria (**arquivos nossos**, não vêm do freeglut) |

Ficaram de fora os backends que o projeto não usa (Android, BlackBerry, Cocoa, Wayland, EGL/GLES, Wii/GameCube),
o build CMake, documentação, exemplos e páginas de manual.

No Linux, as extensões opcionais do X11 (XRandR, XF86VidMode, XInput2) estão desligadas em `config/linux/config.h`
para o build só precisar de `libX11`. Elas servem para trocar a resolução do monitor no game mode e para joystick/multitouch.

## Atualizar a versão

1. Baixe o `.tar.gz` novo em <https://github.com/freeglut/freeglut/releases>.
2. Substitua `include/GL/`, `src/` e `COPYING` pelos arquivos equivalentes, seguindo a tabela acima.
3. Compare `config.h.in` e as listas de fontes do `CMakeLists.txt` novo com `config/*/config.h` e o `Makefile`.
4. `make clean && make run` no Windows e no Linux.
5. Atualize a versão e o SHA-256 neste arquivo.
