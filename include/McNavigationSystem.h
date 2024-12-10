#pragma once

#include "Camera.h"
#include "Engine.h"
#include "EnginePeripherals.h"

namespace McConstants {
// Core movement constants
constexpr float TICK_RATE = 20.0f;
constexpr float TICK_TIME = 1.0f / TICK_RATE;
constexpr float PLAYER_HEIGHT = 1.8f;

// Movement multipliers
constexpr float MOVEMENT_SPRINT = 1.3f;
constexpr float MOVEMENT_WALK = 1.0f;
constexpr float MOVEMENT_SNEAK = 0.3f;
constexpr float MOVEMENT_STOP = 0.0f;

// Physics constants
constexpr float JUMP_INITIAL = 0.42f;
constexpr float GRAVITY = 0.08f;
constexpr float DRAG = 0.98f;
constexpr float GROUND_DRAG = 0.91f;

// Ground movement
constexpr float GROUND_ACCELERATION = 0.1f;
constexpr float AIR_ACCELERATION = 0.02f;
constexpr float SPRINT_JUMP_BOOST = 0.2f;

// Slipperiness values
constexpr float SLIP_DEFAULT = 0.6f;
constexpr float SLIP_AIR = 1.0f;

// New constants
constexpr float FOV_SPRINT_MULTIPLIER = 1.1f;
constexpr float FOV_ZOOM_MULTIPLIER = 0.3f;
constexpr float FOV_NORMAL_MULTIPLIER = 1.0f;
constexpr float FOV_SMOOTH_SPEED = 16.0f;

constexpr float SNEAK_HEIGHT_OFFSET = -0.2f;
constexpr float SNEAK_SMOOTH_SPEED = 16.0f;
} // namespace McConstants

class McNavigationSystem {
  public:
    McNavigationSystem(Engine &engine);
    void update();
    UniformAttachment<UniformBufferObject>
    getUniformAttachment(uint32_t binding);

  private:
    Engine &engine;
    Camera camera;
    EnginePeripheralsManager peripheralsManager;

    struct PlayerState {
        glm::vec3 position{0.0f, McConstants::PLAYER_HEIGHT, 0.0f};
        glm::vec3 velocity{0.0f};
        bool onGround{true};
        bool sprinting{false};
        bool sneaking{false};

        float currentFovMultiplier{1.0f};
        float targetFovMultiplier{1.0f};
        float currentSneakOffset{0.0f};
        float targetSneakOffset{0.0f};

        // Add previous state for interpolation
        glm::vec3 previousPosition{0.0f, McConstants::PLAYER_HEIGHT, 0.0f};
        float previousSneakOffset{0.0f};

        // Add alpha for interpolation
        float interpolationAlpha{0.0f};
    } state;

    float tickAccumulator{0.0f};

    void processFixedUpdate();
    void processMovementInputs();
    void updatePhysics();
    void processMouseMovement(float dx, float dy);
    void processJump();
    void updateSprinting();

    float getMovementMultiplier() const;
    float getSlipperiness() const;

    // Add these new members for mouse look
    float currentYaw{0.0f};
    float currentPitch{0.0f};

    // Add these new method declarations
    void updateFovMultiplier(float deltaTime);
    void updateSneakOffset(float deltaTime);
};
