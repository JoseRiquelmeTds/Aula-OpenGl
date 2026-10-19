#include "gfx/renderer.hpp"

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <memory>

#include "core/camera.hpp"
#include "gfx/scene_node.hpp"
#include "gfx/shader.hpp"
#include "gfx/texture.hpp"

namespace renderer {

namespace {

// Modo do fragment shader básico: 0 = cor sólida, 1 = normal como cor, 2 = UV como cor,
// 3 = textura.
constexpr int kDebugMode = 3;

std::unique_ptr<Shader> basicShader;
float aspectRatio = 1.0f;
bool drawable = false;

}  // namespace

void init() {
    glClearColor(0.10f, 0.10f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    basicShader = std::make_unique<Shader>("shaders/basic.vert", "shaders/basic.frag");
}

void draw(const Camera& camera, const SceneNode& root, const Texture* texture) {
    if (!drawable) {
        return;  // framebuffer minimizado ou ainda sem tamanho válido
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!basicShader->isValid()) {
        return;  // o erro de compilação/link já foi impresso no console
    }

    basicShader->use();
    basicShader->setMat4("uView", camera.view());
    basicShader->setMat4("uProjection", camera.projection(aspectRatio));
    basicShader->setInt("uDebugMode", kDebugMode);
    basicShader->setVec3("uColor", glm::vec3(0.8f, 0.5f, 0.2f));

    // Liga a textura na unidade 0 e diz ao sampler2D para ler dessa unidade.
    if (texture != nullptr) {
        texture->bind(0);
    }
    basicShader->setInt("uTexture", 0);

    // Cada peça define o próprio uModel antes de desenhar.
    root.draw(*basicShader);
}

void resize(int width, int height) {
    glViewport(0, 0, width, height);
    drawable = width > 0 && height > 0;
    if (drawable) {
        aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }
}

void shutdown() {
    basicShader.reset();
}

}  // namespace renderer
