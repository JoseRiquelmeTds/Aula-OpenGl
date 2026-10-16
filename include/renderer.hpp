#ifndef AULA_OPENGL_RENDERER_HPP
#define AULA_OPENGL_RENDERER_HPP

namespace renderer {

// Estado inicial do OpenGL (cor de fundo etc.). Chamar depois de carregar o GLAD.
void init();

// Desenha um quadro. Chamado uma vez por iteração do loop principal.
void draw();

// Ajusta o viewport ao tamanho do framebuffer (em pixels, não em coordenadas de tela).
void resize(int width, int height);

}  // namespace renderer

#endif  // AULA_OPENGL_RENDERER_HPP
