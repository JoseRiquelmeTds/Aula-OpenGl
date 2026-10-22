#include "scene/classroom.hpp"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <cstddef>

#include "core/camera.hpp"
#include "core/input.hpp"
#include "gfx/primitives.hpp"

namespace scene {

namespace {

// Enquadramento da sala inteira (tecla 1); é também o inicial da Camera.
constexpr glm::vec3 kSceneEye(2.0f, 2.0f, 3.0f);
constexpr glm::vec3 kSceneCenter(0.0f, 0.4f, 0.0f);

}  // namespace

Classroom::Classroom()
    // Piso de 4 x 4 m com a textura repetindo a cada 1 m (com kDebugMode = 2, a UV vai de 0 a 4).
    : floorMesh_(primitives::plane(4.0f, 4.0f, 1.0f)),
      cubeMesh_(primitives::box({0.6f, 0.6f, 0.6f})),
      baseMesh_(primitives::cylinder(0.25f, 0.05f)),
      rodMesh_(primitives::cylinder(0.025f, 1.0f, 16)),
      armMesh_(primitives::box({0.5f, 0.04f, 0.04f})),
      testTexture_("assets/textures/uv_teste.png") {
    root_.addChild(&floorMesh_);

    // Peça simples: model = T · R (sem escala, o mesh já foi criado no tamanho real).
    glm::mat4 cubeTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-0.8f, 0.0f, 0.0f));
    cubeTransform = glm::rotate(cubeTransform, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const SceneNode& cube = root_.addChild(&cubeMesh_, cubeTransform);
    targets_.push_back({"cubo", &cube, {0.2f, 1.0f, 1.5f}, {-0.8f, 0.3f, 0.0f}});

    // Objeto composto: base -> haste -> articulação (sem mesh) -> braço.
    // Mover a base leva todo o resto junto; girar a articulação só mexe no braço.
    SceneNode& base = root_.addChild(&baseMesh_, glm::translate(glm::mat4(1.0f), glm::vec3(0.8f, 0.0f, 0.0f)));
    SceneNode& rod = base.addChild(&rodMesh_, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.05f, 0.0f)));

    // Pivô no topo da haste (y = 1 no espaço da haste), girado 45° em Y e 20° para cima em Z.
    glm::mat4 jointTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    jointTransform = glm::rotate(jointTransform, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    jointTransform = glm::rotate(jointTransform, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    SceneNode& joint = rod.addChild(nullptr, jointTransform);

    // O braço tem o pivô na base e centrado em x; deslocado 0,25 m para começar no pivô
    // em vez de atravessá-lo, e 0,02 m para baixo para o eixo do braço passar pelo pivô.
    joint.addChild(&armMesh_, glm::translate(glm::mat4(1.0f), glm::vec3(0.25f, -0.02f, 0.0f)));
    targets_.push_back({"objeto articulado", &base, {2.0f, 1.4f, 2.0f}, {0.9f, 0.6f, 0.0f}});

    // Carteira (CG-62) em posição provisória, sem rotação: a frente (lado do aluno) fica para +Z.
    const glm::mat4 studentDeskPlacement = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 1.2f));
    const SceneNode& studentDesk = studentDesk_.addTo(root_, studentDeskPlacement);
    targets_.push_back({"carteira", &studentDesk, {1.7f, 1.1f, 2.3f}, {1.0f, 0.5f, 1.2f}});  // tecla 4
}

void Classroom::update(const Input& /*input*/, float /*deltaTime*/) {
    // Interações (CG-73, CG-75, CG-76, CG-77) e animações entram aqui.
}

bool Classroom::handleInspection(const Input& input, Camera& camera) {
    // GLFW_KEY_1 a GLFW_KEY_9 são consecutivos. 1 = sala inteira, 2 = primeiro alvo, ...
    for (int digit = 1; digit <= 9; ++digit) {
        if (input.keyPressed(GLFW_KEY_0 + digit)) {
            return selectInspection(digit == 1 ? kWholeScene : digit - 2, camera);
        }
    }
    return false;
}

bool Classroom::selectInspection(int index, Camera& camera) {
    if (index == kWholeScene) {
        for (std::size_t i = 0; i < root_.childCount(); ++i) {
            root_.child(i).visible = true;
        }
        camera.lookAt(kSceneEye, kSceneCenter);
        return true;
    }
    if (index < 0 || index >= static_cast<int>(targets_.size())) {
        return false;  // tecla sem alvo: mantém a visão atual
    }
    const InspectionTarget& target = targets_[static_cast<std::size_t>(index)];
    for (std::size_t i = 0; i < root_.childCount(); ++i) {
        SceneNode& node = root_.child(i);
        node.visible = &node == target.node;
    }
    camera.lookAt(target.eye, target.center);
    return true;
}

}  // namespace scene
