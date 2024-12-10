#include "ControlSystem.h"
#include "Camera.h"
#include <glm/gtx/string_cast.hpp>

ControlSystem::ControlSystem(Engine &engine)
    : engine(engine), camera(glm::vec3(0.0f, 0.0f, 3.0f)) {
    auto window = engine.getWindow().getWindow();

    // Only allowing one; TODO: use the window user pointer more carefully
    static ControlSystem *activeControlSystem = this;

    glfwSetCursorPosCallback(
        window, [](GLFWwindow *window, double xpos, double ypos) {
            static float lastCursorPosX{WINDOWWIDTH / 2.0};
            static float lastCursorPosY{WINDOWHEIGHT / 2.0};
            float dX = xpos - lastCursorPosX;
            float dY = lastCursorPosY - ypos;
            lastCursorPosX = xpos;
            lastCursorPosY = ypos;
            activeControlSystem->processMouseMovement(dX, dY);
        });
    glfwSetScrollCallback(
        window, [](GLFWwindow *window, double xoffset, double yoffset) {
            activeControlSystem->processMouseScroll(yoffset);
        });
}

UniformAttachment<UniformBufferObject>
ControlSystem::getUniformAttachment(uint32_t binding) {
    return UniformAttachment<UniformBufferObject>(
        engine.getDevice(),
        [this](UniformBufferObject &ubo) {
            auto &swapChain = engine.getGlobalResources().getSwapChain();
            VkExtent2D swapChainExtent = swapChain.getExtent();
            float aspectRatio =
                swapChainExtent.width / (float)swapChainExtent.height;

            ubo.view = camera.getViewMatrix();
            ubo.proj = camera.getProjectionMatrix(aspectRatio);
            ubo.camPos = glm::vec4{camera.position, 0.0f};
        },
        binding);
}

void ControlSystem::processMovement(Movement direction, float deltaTime,
                                    float speedMultiplier) {
    float velocity = movementSpeed * deltaTime * speedMultiplier;
    auto forward = camera.calculateForwardVector();
    auto right = camera.calculateRightVector();

    glm::vec3 translation{0.0f};
    switch (direction) {
    case Movement::FORWARD:
        translation = forward * velocity;
        break;
    case Movement::BACKWARD:
        translation = -forward * velocity;
        break;
    case Movement::LEFT:
        translation = -right * velocity;
        break;
    case Movement::RIGHT:
        translation = right * velocity;
        break;
    case Movement::UP:
        translation = glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
        break;
    case Movement::DOWN:
        translation = glm::vec3(0.0f, -1.0f, 0.0f) * velocity;
        break;
    }
    camera.translate(translation);
}

void ControlSystem::processMouseMovement(float xoffset, float yoffset) {
    camera.rotate(xoffset * mouseSensitivityX, yoffset * mouseSensitivityY);
}

void ControlSystem::processMouseScroll(float yoffset) {
    camera.setZoom(camera.zoom - yoffset);
}

void ControlSystem::update() {
    glfwPollEvents();
    peripheralsManager.updatePeripheralsOnFrame();

    auto window = engine.getWindow().getWindow();

    // Speed multiplier when holding Ctrl
    float speedMultiplier =
        (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) ? 25.0f
                                                                  : 1.0f;
    // Horizontal movement (WASD)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        processMovement(Movement::FORWARD, Time::deltaTime(), speedMultiplier);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        processMovement(Movement::BACKWARD, Time::deltaTime(), speedMultiplier);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        processMovement(Movement::LEFT, Time::deltaTime(), speedMultiplier);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        processMovement(Movement::RIGHT, Time::deltaTime(), speedMultiplier);
    // Vertical movement (Space/Shift)
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        processMovement(Movement::UP, Time::deltaTime(), speedMultiplier);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        processMovement(Movement::DOWN, Time::deltaTime(), speedMultiplier);
}
