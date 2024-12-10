#include "Engine.h"

#include <numeric>

#include "SwapChain.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

Engine::Engine() { initVulkan(); }

bool Engine::running() const {
    return isRunning && !glfwWindowShouldClose(appWindow.getWindow());
}

Render Engine::startRender() {
    vkWaitForFences(*appDevice.getDevice(), 1, &inFlightFences[currentFrame],
                    VK_TRUE, UINT64_MAX);

    uint32_t imageIndex = globalResources.getSwapChain().acquireNextImage(
        imageAvailableSemaphores[currentFrame]);

    vkResetFences(*appDevice.getDevice(), 1, &inFlightFences[currentFrame]);

    commandBuffers[currentFrame]->reset();
    commandBuffers[currentFrame]->begin(
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VkCommandBuffer currentCmdBuffer =
        commandBuffers[currentFrame]->getCommandBuffer();

    // Transition image layouts using SwapChain methods
    globalResources.getSwapChain().transitionImageLayout(
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        imageIndex, currentCmdBuffer);
    globalResources.getSwapChain().transitionDepthImageLayout(
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, currentCmdBuffer);

    return Render(
        &globalResources, commandBuffers[currentFrame]->getCommandBuffer(),
        imageIndex, currentFrame, imageAvailableSemaphores[currentFrame],
        renderFinishedSemaphores[currentFrame], inFlightFences[currentFrame]);
}

void Engine::finishRender(Render &render) {
    bool needsRecreation = render.finish();

    // TODO: fix semaphores staying signaled when window is resized
    if (needsRecreation || framebufferResized) {
        framebufferResized = false;
        globalResources.getSwapChain().handleResizing();
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

AppWindow &Engine::getWindow() { return appWindow; }

void Engine::initVulkan() {
    appInstance.init();

    appWindow.init(&appInstance, framebufferResizeCallback);
    appInstance.setAppWindow(appWindow.getWindow());

    appDevice.init(&appWindow, &appInstance);
    appInstance.setAppDevice(appDevice.getDevice());

    globalResources.init(&appDevice, &appWindow);

    createCommandBuffers();
    createSyncObjects();
}

void Engine::initializeEngineTeardown() {
    vkWaitForFences(*appDevice.getDevice(), MAX_FRAMES_IN_FLIGHT,
                    inFlightFences.data(), VK_TRUE, UINT64_MAX);
    vkDeviceWaitIdle(*appDevice.getDevice());

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        vkDestroyFence(*appDevice.getDevice(), inFlightFences[i], nullptr);
        vkDestroySemaphore(*appDevice.getDevice(), renderFinishedSemaphores[i],
                           nullptr);
        vkDestroySemaphore(*appDevice.getDevice(), imageAvailableSemaphores[i],
                           nullptr);
    }

    std::vector<VkCommandBuffer> buffersToFree;
    std::transform(commandBuffers.begin(), commandBuffers.end(),
                   std::back_inserter(buffersToFree),
                   [](auto &commandBufferWrapper) {
                       return commandBufferWrapper->getCommandBuffer();
                   });
    vkFreeCommandBuffers(*appDevice.getDevice(),
                         appDevice.getGraphicsCommandPool()->getCommandPool(),
                         buffersToFree.size(), buffersToFree.data());

    glfwTerminate();
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
        if (vkCreateSemaphore(*appDevice.getDevice(), &semaphoreInfo, nullptr,
                              &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(*appDevice.getDevice(), &semaphoreInfo, nullptr,
                              &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(*appDevice.getDevice(), &fenceInfo, nullptr,
                          &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error(
                "Failed to create synchronization objects for a frame!");
        }
    }
}

TextureManager Engine::createTextureManager() {
    return TextureManager(&appDevice);
}

Renderable Engine::shaded(Model &model, PipelineSettings &settings) {
    return Renderable(&globalResources, model, settings);
}
