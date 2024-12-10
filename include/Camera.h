#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// courtesy - learn opengl

const float YAW = -90.0f;
const float PITCH = 0.0f;

struct cameraOrientation {
    float yaw;
    float pitch;
};

class Camera {
  public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    cameraOrientation Orientation;

    cameraOrientation getOrientation() const { return Orientation; }

    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                    cameraOrientation Orientation = {YAW, PITCH})
        : Position{position}, WorldUp{up}, Orientation{Orientation} {
        updateCameraVectors();
    }
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
           float yaw, float pitch)
        : Position{posX, posY, posZ}, WorldUp{upX, upY, upZ},
          Orientation{yaw, pitch} {
        ;
        updateCameraVectors();
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
           cameraOrientation orientation)
        : Position{posX, posY, posZ}, WorldUp{upX, upY, upZ},
          Orientation{orientation} {
        ;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void SetPosition(glm::vec3 position) {
        Position = position;
        updateCameraVectors();
    }

    void SetOrientation(float yaw, float pitch) {
        Orientation.yaw = yaw;
        Orientation.pitch = pitch;

        updateCameraVectors();
    }

    void SetOrientation(cameraOrientation orientation) {
        Orientation = orientation;
        updateCameraVectors();
    }

    void setViewDirection(glm::vec3 viewDirection) {
        Front = glm::normalize(viewDirection);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

  private:
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Orientation.yaw)) * cos(glm::radians(Orientation.pitch));
        front.y = sin(glm::radians(Orientation.pitch));
        front.z = sin(glm::radians(Orientation.yaw)) * cos(glm::radians(Orientation.pitch));

        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
