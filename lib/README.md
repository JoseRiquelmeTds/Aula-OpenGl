# lib/ — bibliotecas de terceiros

| Pasta | Conteúdo |
| --- | --- |
| `freeglut/` | Código-fonte do freeglut 3.8.0. O `Makefile` compila e linka sozinho (veja [freeglut/README.md](freeglut/README.md)). |
| `windows/` | (opcional) bibliotecas estáticas `.a` compiladas com **MinGW-w64** |
| `linux/` | (opcional) bibliotecas estáticas `.a` compiladas para **Linux x86-64** |

- O `Makefile` linka automaticamente todo `lib/<plataforma>/*.a` da plataforma atual. Os headers dessas libs vão em `include/`.
- `.lib`/`.dll` do MSVC **não funcionam**: o compilador do Windows aqui é o MinGW-w64, que usa `.a`.
- OpenGL e GLU **não** ficam aqui. Eles vêm do sistema (veja [docs/SETUP.md](../docs/SETUP.md)).
- As bibliotecas geradas pelo build (`libfreeglut.a`, `libaula-opengl.a`) vão para `build/`, nunca para esta pasta.
