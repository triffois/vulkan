#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// courtesy - learn opengl

enum class Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 1.5f;
const float SENSITIVITYX = 0.1f;
const float SENSITIVITYY = 0.2f;
const float ZOOM = 45.0f;

class Camera {
  public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed{SPEED};
    float MouseSensitivityX{SENSITIVITYX};
    float MouseSensitivityY{SENSITIVITYY};
    float Zoom{ZOOM};

    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
                    float pitch = PITCH)
        : Position{position}, WorldUp{up}, Yaw{yaw}, Pitch{pitch} {
        updateCameraVectors();
    }
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
           float yaw, float pitch)
        : Position{posX, posY, posZ}, WorldUp{upX, upY, upZ}, Yaw{yaw},
          Pitch{pitch} {
        ;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(Camera_Movement direction, double deltaTime,
                         float speedMultiplier = 1.0f) {
        float velocity = MovementSpeed * deltaTime * speedMultiplier;
        glm::vec3 horizontalFront =
            glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));
        glm::vec3 horizontalRight =
            glm::normalize(glm::vec3(Right.x, 0.0f, Right.z));

        switch (direction) {
        case Camera_Movement::FORWARD:
            Position += horizontalFront * velocity;
            break;
        case Camera_Movement::BACKWARD:
            Position -= horizontalFront * velocity;
            break;
        case Camera_Movement::LEFT:
            Position -= horizontalRight * velocity;
            break;
        case Camera_Movement::RIGHT:
            Position += horizontalRight * velocity;
            break;
        case Camera_Movement::UP:
            Position += WorldUp * velocity;
            break;
        case Camera_Movement::DOWN:
            Position -= WorldUp * velocity;
            break;
        }
    }

    void ProcessMouseMovement(float xoffset, float yoffset) {
        xoffset *= MouseSensitivityX;
        yoffset *= MouseSensitivityY;

        Yaw += xoffset;
        Pitch += yoffset;

        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;

        updateCameraVectors();
    }

    void ProcessMouseScroll(float yoffset) {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    float getZoom() const { return Zoom; }

  private:
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
