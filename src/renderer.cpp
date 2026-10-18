#include "renderer.hpp"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

#include "shader.hpp"
#include "vertex.hpp"

namespace renderer {

namespace {

// Modo do fragment shader básico: 0 = cor sólida, 1 = normal como cor, 2 = UV como cor.
constexpr int kDebugMode = 1;

std::unique_ptr<Shader> basicShader;
float aspectRatio = 1.0f;

// --- Cubo de teste TEMPORÁRIO ------------------------------------------------
// Só para conferir o shader e o formato de vértice. Sai daqui quando a estrutura de
// mesh (VAO/VBO/EBO) da equipe existir: aí o cubo vira um Mesh como os outros.
GLuint testVao = 0;
GLuint testVbo = 0;
GLuint testEbo = 0;
GLsizei testIndexCount = 0;

void createTestCube() {
    // Cubo de 1 m com a base em y = 0 (pivô na base). 4 vértices por face, cada um com a
    // normal da sua face, para as arestas ficarem "duras" quando houver iluminação.
    const Vertex vertices[] = {
        // +Z (frente)
        {{-0.5f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{0.5f, 1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        // -Z (trás)
        {{0.5f, 0.0f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
        {{-0.5f, 0.0f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
        {{-0.5f, 1.0f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
        {{0.5f, 1.0f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
        // +X (direita)
        {{0.5f, 0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 1.0f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{0.5f, 1.0f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        // -X (esquerda)
        {{-0.5f, 0.0f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{-0.5f, 0.0f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f, 1.0f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f, 1.0f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        // +Y (topo)
        {{-0.5f, 1.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 1.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 1.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f, 1.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        // -Y (base)
        {{-0.5f, 0.0f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.0f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.0f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.0f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
    };

    // Dois triângulos por face, anti-horários vistos de fora.
    GLuint indices[36];
    for (GLuint face = 0; face < 6; ++face) {
        const GLuint base = face * 4;
        const GLuint quad[6] = {base, base + 1, base + 2, base + 2, base + 3, base};
        for (int i = 0; i < 6; ++i) {
            indices[face * 6 + i] = quad[i];
        }
    }
    testIndexCount = 36;

    glGenVertexArrays(1, &testVao);
    glGenBuffers(1, &testVbo);
    glGenBuffers(1, &testEbo);

    glBindVertexArray(testVao);
    glBindBuffer(GL_ARRAY_BUFFER, testVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, testEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    configureVertexAttributes();
    glBindVertexArray(0);
}

void destroyTestCube() {
    glDeleteBuffers(1, &testEbo);
    glDeleteBuffers(1, &testVbo);
    glDeleteVertexArrays(1, &testVao);
}

}  // namespace

void init() {
    glClearColor(0.10f, 0.10f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    basicShader = std::make_unique<Shader>("shaders/basic.vert", "shaders/basic.frag");
    createTestCube();
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!basicShader->isValid()) {
        return;  // o erro de compilação/link já foi impresso no console
    }

    // Câmera fixa provisória (a câmera livre é outra task): 1,6 m de altura, olhando o cubo.
    const glm::mat4 view = glm::lookAt(glm::vec3(1.8f, 1.6f, 2.5f), glm::vec3(0.0f, 0.5f, 0.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 100.0f);

    // Model = T * R * S: escala, depois gira 30° em Y, depois leva para a posição.
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

    basicShader->use();
    basicShader->setMat4("uModel", model);
    basicShader->setMat4("uView", view);
    basicShader->setMat4("uProjection", projection);
    basicShader->setInt("uDebugMode", kDebugMode);
    basicShader->setVec3("uColor", glm::vec3(0.8f, 0.5f, 0.2f));

    glBindVertexArray(testVao);
    glDrawElements(GL_TRIANGLES, testIndexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void resize(int width, int height) {
    glViewport(0, 0, width, height);
    if (height > 0) {  // janela minimizada tem altura 0
        aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }
}

void shutdown() {
    destroyTestCube();
    basicShader.reset();
}

}  // namespace renderer
