#include "McNavigationSystem.h"

McNavigationSystem::McNavigationSystem(Engine &engine)
    : engine(engine),
      camera(glm::vec3(0.0f, McConstants::PLAYER_HEIGHT, 0.0f)) {
    auto window = engine.getWindow().getWindow();

    // Set up mouse callbacks similar to ControlSystem
    static McNavigationSystem *activeSystem = this;

    glfwSetCursorPosCallback(window,
                             [](GLFWwindow *window, double xpos, double ypos) {
                                 static float lastX{WINDOWWIDTH / 2.0f};
                                 static float lastY{WINDOWHEIGHT / 2.0f};
                                 float dx = xpos - lastX;
                                 float dy = ypos - lastY;
                                 lastX = xpos;
                                 lastY = ypos;
                                 activeSystem->processMouseMovement(dx, dy);
                             });
}

void McNavigationSystem::update() {
    glfwPollEvents();
    peripheralsManager.updatePeripheralsOnFrame();

    float deltaTime = Time::deltaTime();
    tickAccumulator += deltaTime;

    // Process fixed updates
    while (tickAccumulator >= McConstants::TICK_TIME) {
        // Store previous state before fixed updates
        state.previousPosition = state.position;
        state.previousSneakOffset = state.currentSneakOffset;

        processFixedUpdate();
        tickAccumulator -= McConstants::TICK_TIME;
    }

    // Calculate interpolation alpha
    state.interpolationAlpha = tickAccumulator / McConstants::TICK_TIME;

    // Interpolate position
    glm::vec3 interpolatedPosition = glm::mix(
        state.previousPosition, state.position, state.interpolationAlpha);

    // Update camera with interpolated position
    camera.position =
        interpolatedPosition + glm::vec3(0.0f, state.currentSneakOffset, 0.0f);

    // Update view-related effects
    updateFovMultiplier(deltaTime);
    updateSneakOffset(deltaTime);

    // Update camera orientation
    camera.orientation.yaw = currentYaw;
    camera.orientation.pitch = currentPitch;
}

void McNavigationSystem::processFixedUpdate() {
    processJump();
    processMovementInputs();
    updateSprinting();
    updatePhysics();
}

float McNavigationSystem::getMovementMultiplier() const {
    if (state.sprinting)
        return McConstants::MOVEMENT_SPRINT;
    if (state.sneaking)
        return McConstants::MOVEMENT_SNEAK;
    return McConstants::MOVEMENT_WALK;
}

float McNavigationSystem::getSlipperiness() const {
    return state.onGround ? McConstants::SLIP_DEFAULT : McConstants::SLIP_AIR;
}

void McNavigationSystem::processMovementInputs() {
    auto window = engine.getWindow().getWindow();

    // Get movement input
    glm::vec3 moveInput{0.0f};
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        moveInput.z += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        moveInput.z -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        moveInput.x -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        moveInput.x += 1.0f;

    if (glm::length(moveInput) > 0.0f) {
        moveInput = glm::normalize(moveInput);

        float moveMultiplier = getMovementMultiplier();
        float slipperiness = getSlipperiness();

        // Calculate acceleration based on ground/air state
        float acceleration =
            state.onGround ? McConstants::GROUND_ACCELERATION * moveMultiplier *
                                 std::pow(0.6f / slipperiness, 3)
                           : McConstants::AIR_ACCELERATION * moveMultiplier;

        // Apply movement in look direction
        glm::vec3 forward = camera.calculateForwardVector();
        glm::vec3 right = camera.calculateRightVector();
        forward.y = 0.0f; // Keep movement horizontal
        right.y = 0.0f;
        forward = glm::normalize(forward);
        right = glm::normalize(right);

        glm::vec3 movement =
            (forward * moveInput.z + right * moveInput.x) * acceleration;
        state.velocity.x += movement.x;
        state.velocity.z += movement.z;
    }
}

void McNavigationSystem::updatePhysics() {
    // Apply vertical movement formula
    if (!state.onGround) {
        state.velocity.y -= McConstants::GRAVITY;
        state.velocity.y *= McConstants::DRAG;
    }

    // Apply horizontal movement formula
    float slipperiness = getSlipperiness();
    state.velocity.x *= McConstants::GROUND_DRAG * slipperiness;
    state.velocity.z *= McConstants::GROUND_DRAG * slipperiness;

    // Update position
    state.position += state.velocity;

    // Ground collision
    if (state.position.y <= McConstants::PLAYER_HEIGHT) {
        state.position.y = McConstants::PLAYER_HEIGHT;
        state.velocity.y = 0.0f;
        state.onGround = true;
    } else {
        state.onGround = false;
    }
}

void McNavigationSystem::processJump() {
    if (state.onGround && glfwGetKey(engine.getWindow().getWindow(),
                                     GLFW_KEY_SPACE) == GLFW_PRESS) {
        state.velocity.y = McConstants::JUMP_INITIAL;
        state.onGround = false;

        // Apply sprint jump boost
        if (state.sprinting) {
            glm::vec3 forward = camera.calculateForwardVector();
            forward.y = 0.0f;
            forward = glm::normalize(forward);
            state.velocity += forward * McConstants::SPRINT_JUMP_BOOST;
        }
    }
}

void McNavigationSystem::processMouseMovement(float dx, float dy) {
    // Store the orientation values directly in the class
    currentYaw += dx * 0.1f;
    currentPitch = glm::clamp(currentPitch - dy * 0.1f, -89.0f, 89.0f);
}

void McNavigationSystem::updateSprinting() {
    auto window = engine.getWindow().getWindow();
    bool wantToSprint = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
    bool movingForward = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    state.sneaking = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    // Only update sprinting state when on ground
    if (state.onGround) {
        state.sprinting = wantToSprint && movingForward && !state.sneaking;
    }
    // When airborne, sprinting state remains unchanged
}

void McNavigationSystem::updateFovMultiplier(float deltaTime) {
    auto window = engine.getWindow().getWindow();

    // Determine target FOV multiplier
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        state.targetFovMultiplier = McConstants::FOV_ZOOM_MULTIPLIER;
    } else if (state.sprinting) {
        state.targetFovMultiplier = McConstants::FOV_SPRINT_MULTIPLIER;
    } else if (state.onGround) {
        state.targetFovMultiplier = McConstants::FOV_NORMAL_MULTIPLIER;
    }

    // Smoothly interpolate current FOV multiplier
    state.currentFovMultiplier = glm::mix(
        state.currentFovMultiplier, state.targetFovMultiplier,
        1.0f - std::pow(2.0f, -deltaTime * McConstants::FOV_SMOOTH_SPEED));
}

void McNavigationSystem::updateSneakOffset(float deltaTime) {
    // Update target sneak offset based on sneaking state
    state.targetSneakOffset =
        state.sneaking ? McConstants::SNEAK_HEIGHT_OFFSET : 0.0f;

    // Smoothly interpolate current sneak offset
    state.currentSneakOffset = glm::mix(
        state.currentSneakOffset, state.targetSneakOffset,
        1.0f - std::pow(2.0f, -deltaTime * McConstants::SNEAK_SMOOTH_SPEED));
}

UniformAttachment<UniformBufferObject>
McNavigationSystem::getUniformAttachment(uint32_t binding) {
    return UniformAttachment<UniformBufferObject>(
        engine.getDevice(),
        [this](UniformBufferObject &ubo) {
            camera.setZoom(90.0 * state.currentFovMultiplier);

            auto &swapChain = engine.getGlobalResources().getSwapChain();
            float aspectRatio = swapChain.getExtent().width /
                                (float)swapChain.getExtent().height;

            ubo.view = camera.getViewMatrix();
            // Apply FOV multiplier to projection matrix
            ubo.proj = camera.getProjectionMatrix(aspectRatio);
            ubo.camPos = glm::vec4{camera.position, 0.0f};
        },
        binding);
}
