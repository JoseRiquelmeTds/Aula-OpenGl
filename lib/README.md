# lib/ — bibliotecas estáticas de terceiros

Coloque aqui arquivos `.a` (bibliotecas estáticas compiladas **para Linux x86-64**) que o projeto precisar.
O `Makefile` linka automaticamente todo `lib/*.a`, e os headers delas vão em `include/`.

- `.lib` e `.dll` do Windows **não funcionam** aqui. O build roda no WSL2 (Linux).
- GLUT, GLU e OpenGL **não** ficam aqui. Eles vêm do sistema via `make deps` (veja [docs/SETUP_WSL2.md](../docs/SETUP_WSL2.md)).
- A biblioteca estática do próprio projeto (`libaula-opengl.a`) é gerada em `build/<modo>/lib/`, nunca nesta pasta.
