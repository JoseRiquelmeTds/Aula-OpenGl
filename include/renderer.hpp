#ifndef AULA_OPENGL_RENDERER_HPP
#define AULA_OPENGL_RENDERER_HPP

namespace renderer {

// Estado inicial do OpenGL (cor de fundo etc.). Chamar depois de glutCreateWindow.
void init();

// Callback de glutDisplayFunc: desenha a cena.
void display();

// Callback de glutReshapeFunc: ajusta viewport e projeção ao tamanho da janela.
void reshape(int width, int height);

}  // namespace renderer

#endif  // AULA_OPENGL_RENDERER_HPP
