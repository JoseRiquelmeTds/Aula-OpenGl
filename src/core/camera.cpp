#include "core/camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>

namespace {

constexpr glm::vec3 kWorldUp(0.0f, 1.0f, 0.0f);
constexpr float kMaxPitch = 89.0f;

}  // namespace

Camera::Camera() {
    lookAt({2.0f, 2.0f, 3.0f}, {0.0f, 0.4f, 0.0f});
}

void Camera::lookAt(const glm::vec3& position, const glm::vec3& target) {
    position_ = position;
    const glm::vec3 direction = target - position;
    if (glm::dot(direction, direction) > 0.0f) {
        const glm::vec3 unit = glm::normalize(direction);
        yaw_ = glm::degrees(std::atan2(unit.z, unit.x));
        pitch_ = std::clamp(glm::degrees(std::asin(std::clamp(unit.y, -1.0f, 1.0f))),
                            -kMaxPitch, kMaxPitch);
    }
}

glm::vec3 Camera::forward() const {
    const float yaw = glm::radians(yaw_);
    const float pitch = glm::radians(pitch_);
    return glm::normalize(glm::vec3(std::cos(yaw) * std::cos(pitch), std::sin(pitch),
                                    std::sin(yaw) * std::cos(pitch)));
}

glm::vec3 Camera::right() const {
    return glm::normalize(glm::cross(forward(), kWorldUp));
}

void Camera::move(const glm::vec3& direction, float deltaTime) {
    if (deltaTime <= 0.0f) {
        return;
    }
    const glm::vec3 displacement = right() * direction.x + kWorldUp * direction.y + forward() * direction.z;
    if (glm::dot(displacement, displacement) > 0.0f) {
        position_ += glm::normalize(displacement) * movementSpeed * deltaTime;
    }
}

void Camera::rotate(float deltaX, float deltaY) {
    yaw_ = std::remainder(yaw_ + deltaX * mouseSensitivity, 360.0f);
    pitch_ = std::clamp(pitch_ - deltaY * mouseSensitivity, -kMaxPitch, kMaxPitch);
}

glm::mat4 Camera::view() const {
    return glm::lookAt(position_, position_ + forward(), kWorldUp);
}

glm::mat4 Camera::projection(float aspectRatio) const {
    // Fallback também protege chamadas sem um framebuffer válido.
    if (!(aspectRatio > 0.0f) || !std::isfinite(aspectRatio)) {
        aspectRatio = 1.0f;
    }
    return glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 100.0f);
}
