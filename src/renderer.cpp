#include "renderer.hpp"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

#include "mesh.hpp"
#include "primitives.hpp"
#include "scene_node.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace renderer {

namespace {

// Modo do fragment shader básico: 0 = cor sólida, 1 = normal como cor, 2 = UV como cor,
// 3 = textura.
constexpr int kDebugMode = 3;

std::unique_ptr<Shader> basicShader;
float aspectRatio = 1.0f;

// --- Exemplo de uso do Mesh, das primitivas e do SceneNode --------------------
// Cena provisória só para conferir a estrutura de mesh. Sai daqui quando a montagem da
// cena (sala + 9 objetos) existir.
//
// Os Mesh ficam aqui (são os donos dos buffers) e as peças só apontam para eles.
std::unique_ptr<Mesh> floorMesh;
std::unique_ptr<Mesh> cubeMesh;
std::unique_ptr<Mesh> baseMesh;
std::unique_ptr<Mesh> rodMesh;
std::unique_ptr<Mesh> armMesh;
std::unique_ptr<SceneNode> scene;

// Textura de teste: o vermelho cresce com u (para a direita) e o verde com v (para cima),
// as mesmas cores do kDebugMode = 2. Se aparecer verde embaixo, a imagem está de cabeça
// para baixo. Por enquanto vale para a cena toda; textura por objeto vem com os materiais.
std::unique_ptr<Texture> testTexture;

void createExampleScene() {
    // Piso de 4 x 4 m com a textura repetindo a cada 1 m (com kDebugMode = 2, a UV vai de 0 a 4).
    floorMesh = std::make_unique<Mesh>(primitives::plane(4.0f, 4.0f, 1.0f));
    cubeMesh = std::make_unique<Mesh>(primitives::box({0.6f, 0.6f, 0.6f}));
    baseMesh = std::make_unique<Mesh>(primitives::cylinder(0.25f, 0.05f));
    rodMesh = std::make_unique<Mesh>(primitives::cylinder(0.025f, 1.0f, 16));
    armMesh = std::make_unique<Mesh>(primitives::box({0.5f, 0.04f, 0.04f}));

    // Raiz vazia: a cena inteira é um objeto composto.
    scene = std::make_unique<SceneNode>();
    scene->addChild(floorMesh.get());

    // Peça simples: model = T · R (sem escala, o mesh já foi criado no tamanho real).
    glm::mat4 cubeTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-0.8f, 0.0f, 0.0f));
    cubeTransform = glm::rotate(cubeTransform, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    scene->addChild(cubeMesh.get(), cubeTransform);

    // Objeto composto: base -> haste -> articulação (sem mesh) -> braço.
    // Mover a base leva todo o resto junto; girar a articulação só mexe no braço.
    SceneNode& base = scene->addChild(baseMesh.get(), glm::translate(glm::mat4(1.0f), glm::vec3(0.8f, 0.0f, 0.0f)));
    SceneNode& rod = base.addChild(rodMesh.get(), glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.05f, 0.0f)));

    // Pivô no topo da haste (y = 1 no espaço da haste), girado 45° em Y e 20° para cima em Z.
    glm::mat4 jointTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    jointTransform = glm::rotate(jointTransform, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    jointTransform = glm::rotate(jointTransform, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    SceneNode& joint = rod.addChild(nullptr, jointTransform);

    // O braço tem o pivô na base e centrado em x; deslocado 0,25 m para começar no pivô
    // em vez de atravessá-lo, e 0,02 m para baixo para o eixo do braço passar pelo pivô.
    joint.addChild(armMesh.get(), glm::translate(glm::mat4(1.0f), glm::vec3(0.25f, -0.02f, 0.0f)));

    testTexture = std::make_unique<Texture>("assets/textures/uv_teste.png");
}

void destroyExampleScene() {
    testTexture.reset();
    // As peças primeiro: elas apontam para os meshes.
    scene.reset();
    armMesh.reset();
    rodMesh.reset();
    baseMesh.reset();
    cubeMesh.reset();
    floorMesh.reset();
}

}  // namespace

void init() {
    glClearColor(0.10f, 0.10f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    basicShader = std::make_unique<Shader>("shaders/basic.vert", "shaders/basic.frag");
    createExampleScene();
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!basicShader->isValid()) {
        return;  // o erro de compilação/link já foi impresso no console
    }

    // Câmera fixa provisória (a câmera livre é outra task): olhando a cena de exemplo de cima.
    const glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 3.0f), glm::vec3(0.0f, 0.4f, 0.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 100.0f);

    basicShader->use();
    basicShader->setMat4("uView", view);
    basicShader->setMat4("uProjection", projection);
    basicShader->setInt("uDebugMode", kDebugMode);
    basicShader->setVec3("uColor", glm::vec3(0.8f, 0.5f, 0.2f));

    // Liga a textura na unidade 0 e diz ao sampler2D para ler dessa unidade.
    testTexture->bind(0);
    basicShader->setInt("uTexture", 0);

    // Cada peça define o próprio uModel antes de desenhar.
    scene->draw(*basicShader);
}

void resize(int width, int height) {
    glViewport(0, 0, width, height);
    if (height > 0) {  // janela minimizada tem altura 0
        aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }
}

void shutdown() {
    destroyExampleScene();
    basicShader.reset();
}

}  // namespace renderer
