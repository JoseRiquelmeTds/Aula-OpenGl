#ifndef AULA_OPENGL_GFX_RENDERER_HPP
#define AULA_OPENGL_GFX_RENDERER_HPP

class Camera;

namespace renderer {

enum class InspectionMode { Scene, Cube, Articulated };

// Seleciona a visibilidade e reenquadra a câmera; depois ela continua livre.
// Chamar entre init() e shutdown().
void selectInspection(InspectionMode mode, Camera& camera);

// Estado inicial do OpenGL (cor de fundo etc.). Chamar depois de carregar o GLAD.
void init();

// Desenha um quadro. Chamado uma vez por iteração do loop principal.
void draw(const Camera& camera);

// Ajusta o viewport ao tamanho do framebuffer (em pixels, não em coordenadas de tela).
void resize(int width, int height);

// Libera shaders e buffers. Chamar antes de destruir a janela, enquanto o contexto existe.
void shutdown();

}  // namespace renderer

#endif  // AULA_OPENGL_GFX_RENDERER_HPP
