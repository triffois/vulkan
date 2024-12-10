#pragma once

#include "Camera.h"
#include "Engine.h"
#include "EnginePeripherals.h"

enum class Movement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

class ControlSystem {
  public:
    ControlSystem(Engine &engine);

    void update();

    UniformAttachment<UniformBufferObject>
    getUniformAttachment(uint32_t binding);

  private:
    Engine &engine;
    Camera camera;
    EnginePeripheralsManager peripheralsManager;

    // Camera control parameters
    float movementSpeed{1.5f};
    float mouseSensitivityX{0.1f};
    float mouseSensitivityY{0.2f};

    void processMovement(Movement direction, float deltaTime,
                         float speedMultiplier);
    void processMouseMovement(float xoffset, float yoffset);
    void processMouseScroll(float yoffset);
};
