#ifndef AULA_OPENGL_CAMERA_HPP
#define AULA_OPENGL_CAMERA_HPP

#include <glm/glm.hpp>

// Câmera livre com Y vertical. Não depende de janela, Input ou callbacks.
class Camera {
public:
    Camera();

    void lookAt(const glm::vec3& position, const glm::vec3& target);
    // Direção local: x = direita, y = Y do mundo, z = frente (inclui o pitch).
    // O vetor combinado é normalizado para manter a velocidade nas diagonais.
    void move(const glm::vec3& direction, float deltaTime);
    // Deslocamento do mouse em pixels: x para a direita, y para baixo; sem dt.
    void rotate(float deltaX, float deltaY);

    glm::vec3 position() const { return position_; }
    glm::vec3 forward() const;
    glm::vec3 right() const;
    glm::mat4 view() const;
    glm::mat4 projection(float aspectRatio) const;

    float movementSpeed = 2.0f;     // unidades por segundo
    float mouseSensitivity = 0.1f;  // graus por pixel

private:
    glm::vec3 position_{};
    float yaw_ = -90.0f;
    float pitch_ = 0.0f;
};

#endif  // AULA_OPENGL_CAMERA_HPP
