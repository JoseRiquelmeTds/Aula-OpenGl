#include "scene_node.hpp"

SceneNode::SceneNode(const Mesh* mesh, const glm::mat4& transform) : mesh(mesh), transform(transform) {}

SceneNode& SceneNode::addChild(const Mesh* mesh, const glm::mat4& transform) {
    children_.push_back(std::make_unique<SceneNode>(mesh, transform));
    return *children_.back();
}

void SceneNode::draw(const Shader& shader, const glm::mat4& parentModel) const {
    if (!visible) {
        return;
    }

    // Leva o vértice do espaço desta peça para o do pai e, pela parentModel, até o mundo.
    const glm::mat4 model = parentModel * transform;

    if (mesh != nullptr) {
        shader.setMat4("uModel", model);
        mesh->draw();
    }

    // Os filhos recebem a model desta peça como "pai": a hierarquia se acumula.
    for (const auto& child : children_) {
        child->draw(shader, model);
    }
}
