#include "Engine.h"
#include "SwapChain.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

Engine::Engine() : mainCamera(glm::vec3(0.0f, 0.0f, 3.0f)) { initVulkan(); }
Engine::~Engine() { cleanup(); }

bool Engine::running() const {
    return isRunning && !glfwWindowShouldClose(appWindow.getWindow());
}

Render Engine::startRender() {
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE,
                    UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        device, swapChain->getSwapChain(), UINT64_MAX,
        imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return startRender();
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    vkResetFences(device, 1, &inFlightFences[currentFrame]);

    commandBuffers[currentFrame]->reset();
    commandBuffers[currentFrame]->begin(
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VkCommandBuffer currentCmdBuffer =
        commandBuffers[currentFrame]->getCommandBuffer();

    // Transition image layouts using SwapChain methods
    swapChain->transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED,
                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                     imageIndex, currentCmdBuffer);
    swapChain->transitionDepthImageLayout(
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, currentCmdBuffer);

    processEvents();
    peripheralsManager.updatePeripheralsOnFrame();
    processKeyboardInput();

    return Render(&appDevice, swapChain.get(),
                  commandBuffers[currentFrame]->getCommandBuffer(), imageIndex,
                  currentFrame, imageAvailableSemaphores[currentFrame],
                  renderFinishedSemaphores[currentFrame],
                  inFlightFences[currentFrame], mainCamera);
}

void Engine::finishRender(Render &render) {
    bool needsRecreation = render.finish();

    if (needsRecreation || framebufferResized) {
        framebufferResized = false;
        recreateSwapChain();
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Engine::processEvents() { glfwPollEvents(); }

Camera *Engine::getCamera() { return &mainCamera; }

void Engine::processKeyboardInput() {
    auto window = appWindow.getWindow();

    // Speed multiplier when holding Ctrl
    float speedMultiplier =
        (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) ? 25.0f
                                                                  : 1.0f;

    // Horizontal movement (WASD)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        mainCamera.ProcessKeyboard(Camera_Movement::FORWARD, Time::deltaTime(),
                                   speedMultiplier);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        mainCamera.ProcessKeyboard(Camera_Movement::BACKWARD, Time::deltaTime(),
                                   speedMultiplier);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        mainCamera.ProcessKeyboard(Camera_Movement::LEFT, Time::deltaTime(),
                                   speedMultiplier);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        mainCamera.ProcessKeyboard(Camera_Movement::RIGHT, Time::deltaTime(),
                                   speedMultiplier);
    }

    // Vertical movement (Space/Shift)
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        mainCamera.ProcessKeyboard(Camera_Movement::UP, Time::deltaTime(),
                                   speedMultiplier);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        mainCamera.ProcessKeyboard(Camera_Movement::DOWN, Time::deltaTime(),
                                   speedMultiplier);
    }
}

void Engine::initVulkan() {
    appInstance.init();
    appInstance.addComponentToCleanUp(&appInstance);

    appWindow.init(&appInstance, framebufferResizeCallback);
    appInstance.addComponentToCleanUp(&appWindow, 0);
    appInstance.setAppWindow(appWindow.getWindow());

    appDevice.init(&appWindow, &appInstance);
    appInstance.addComponentToCleanUp(&appDevice, 0);
    appInstance.setAppDevice(appDevice.getDevice());
    device = *appDevice.getDevice();

    SwapChainSupportDetails swapChainSupport =
        appDevice.querySwapChainSupportCurrent();
    swapChain = std::make_unique<SwapChain>(&appDevice, &appWindow);

    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
}

void Engine::cleanup() {
    swapChain->cleanup();

    // vkDestroyImage(device, textureImage, nullptr);
    // vkFreeMemory(device, textureImageMemory, nullptr);

    // vkDestroyBuffer(device, indexBuffer, nullptr);
    // vkFreeMemory(device, indexBufferMemory, nullptr);

    // vkDestroyBuffer(device, vertexBuffer, nullptr);
    // vkFreeMemory(device, vertexBufferMemory, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(device, commandPool, nullptr);

    appInstance.cleanUpAll();

    glfwTerminate();
}

void Engine::recreateSwapChain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(appWindow.getWindow(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(appWindow.getWindow(), &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device);

    swapChain->recreate();
}

void Engine::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices =
        appDevice.findQueueFamiliesCurrent();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics command pool!");
    }
}

void Engine::createCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        commandBuffers[i] = std::make_unique<CommandBuffer>(
            &appDevice, appDevice.getGraphicsCommandPool());
    }
}

void Engine::createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                              &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                              &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) !=
                VK_SUCCESS) {
            throw std::runtime_error(
                "failed to create synchronization objects for a frame!");
        }
    }
}

VkShaderModule Engine::createShaderModule(const std::vector<char> &code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

Pipeline Engine::createPipeline(const std::string &vertShaderPath,
                                const std::string &fragShaderPath,
                                const Model &model) {
    return Pipeline(&appDevice, vertShaderPath, fragShaderPath,
                    swapChain->getImageFormat(), swapChain->findDepthFormat(),
                    model, MAX_FRAMES_IN_FLIGHT);
}
