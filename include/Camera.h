#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//courtesy - learn opengl

enum class Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    NW,
    NE,
    SW,
    SE
};

const float YAW          = -90.0f;
const float PITCH        =  0.0f;
const float SPEED        =  1.5f;
const float SENSITIVITYX =  0.1f;
const float SENSITIVITYY =  0.2f;
const float ZOOM         =  45.0f;

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

    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH):Position{position},WorldUp{up},Yaw{yaw},Pitch{pitch}
    {
        updateCameraVectors();
    }
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch): Position{posX, posY, posZ},WorldUp{upX, upY, upZ},Yaw{yaw},Pitch{pitch}
    {;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(Camera_Movement direction, double deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;

        if (direction == Camera_Movement::FORWARD)
            Position += Front * velocity;
        if (direction == Camera_Movement::BACKWARD)
            Position -= Front * velocity;
        if (direction == Camera_Movement::LEFT)
            Position -= Right * velocity;
        if (direction == Camera_Movement::RIGHT)
            Position += Right * velocity;
        if (direction == Camera_Movement::NW)
            Position += glm::normalize((Front - Right)) * velocity;
        if (direction == Camera_Movement::NE)
            Position += glm::normalize((Front + Right)) * velocity;
        if (direction == Camera_Movement::SW)
            Position += glm::normalize((-Front - Right)) * velocity;
        if (direction == Camera_Movement::SE)
            Position += glm::normalize((-Front + Right)) * velocity;
    }

    void ProcessMouseMovement(float xoffset, float yoffset)
    {
        xoffset *= MouseSensitivityX;
        yoffset *= MouseSensitivityY;

        Yaw   += xoffset;
        Pitch += yoffset;

        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;

        updateCameraVectors();
    }

    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    float getZoom() {
        return Zoom;
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};