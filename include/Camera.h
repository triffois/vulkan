#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct CameraOrientation {
    float yaw;
    float pitch;
};

class Camera {
  public:
    glm::vec3 position;
    CameraOrientation orientation;
    float zoom{45.0f};

    explicit Camera(glm::vec3 position = glm::vec3(0.0f),
                    CameraOrientation orientation = {-90.0f, 0.0f})
        : position{position}, orientation{orientation} {}

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + calculateFrontVector(),
                           glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::mat4 getProjectionMatrix(float aspectRatio) const {
        auto proj =
            glm::perspective(glm::radians(zoom), aspectRatio, 0.1f, 1000.0f);
        proj[1][1] *= -1; // Vulkan correction
        return proj;
    }

    void translate(const glm::vec3 &translation) { position += translation; }

    void rotate(float yawOffset, float pitchOffset) {
        orientation.yaw += yawOffset;
        orientation.pitch += pitchOffset;
        orientation.pitch = glm::clamp(orientation.pitch, -89.0f, 89.0f);
    }

    void setZoom(float newZoom) { zoom = glm::clamp(newZoom, 1.0f, 179.0f); }

    // Calculate vectors on demand
    glm::vec3 calculateFrontVector() const {
        glm::vec3 front;
        front.x = cos(glm::radians(orientation.yaw)) *
                  cos(glm::radians(orientation.pitch));
        front.y = sin(glm::radians(orientation.pitch));
        front.z = sin(glm::radians(orientation.yaw)) *
                  cos(glm::radians(orientation.pitch));
        return glm::normalize(front);
    }

    glm::vec3 calculateForwardVector() const {
        auto front = calculateFrontVector();
        return glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    }

    glm::vec3 calculateRightVector() const {
        return glm::normalize(glm::vec3(-sin(glm::radians(orientation.yaw)),
                                        0.0f,
                                        cos(glm::radians(orientation.yaw))));
    }

    glm::vec3 calculateUpVector() const {
        return glm::normalize(
            glm::cross(calculateRightVector(), calculateFrontVector()));
    }
};
