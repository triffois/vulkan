#include "Engine.h"
#include <algorithm>

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
        device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame],
        VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return startRender(); // Retry with new swapchain
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    vkResetFences(device, 1, &inFlightFences[currentFrame]);

    commandBuffers[currentFrame]->reset();
    commandBuffers[currentFrame]->begin(
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VkCommandBuffer currentCmdBuffer =
        commandBuffers[currentFrame]->getCommandBuffer();

    // Transition image layouts
    transitionSwapchainImageLayout(VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   imageIndex, currentCmdBuffer);
    transitionDepthImageLayout(VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                               depthImage, currentCmdBuffer);

    processEvents();
    peripheralsManager.updatePeripheralsOnFrame();
    processKeyboardInput();

    return Render(&appDevice, swapChain, swapChainImages, swapChainImageViews,
                  depthImageView, currentCmdBuffer, imageIndex, currentFrame,
                  imageAvailableSemaphores[currentFrame],
                  renderFinishedSemaphores[currentFrame],
                  inFlightFences[currentFrame], swapChainExtent, mainCamera);
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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            mainCamera.ProcessKeyboard(Camera_Movement::NW, Time::deltaTime());
        } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            mainCamera.ProcessKeyboard(Camera_Movement::NE, Time::deltaTime());
        } else {
            mainCamera.ProcessKeyboard(Camera_Movement::FORWARD,
                                       Time::deltaTime());
        }
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            mainCamera.ProcessKeyboard(Camera_Movement::SW, Time::deltaTime());
        } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            mainCamera.ProcessKeyboard(Camera_Movement::SE, Time::deltaTime());
        } else {
            mainCamera.ProcessKeyboard(Camera_Movement::BACKWARD,
                                       Time::deltaTime());
        }
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        mainCamera.ProcessKeyboard(Camera_Movement::LEFT, Time::deltaTime());
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        mainCamera.ProcessKeyboard(Camera_Movement::RIGHT, Time::deltaTime());
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

    // device -> instance -> window

    createSwapChain();
    createImageViews();

    createCommandPool();

    createDepthResources();

    createCommandBuffers();
    createSyncObjects();
}

void Engine::cleanupSwapChain() {
    vkDestroyImageView(device, depthImageView, nullptr);

    // vkDestroyImage(device, depthImage, nullptr);
    // vkFreeMemory(device, depthImageMemory, nullptr);
    appDevice.freeAllocationMemoryOnDemand(&depthImageAllocation);

    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(device, swapChain, nullptr);
}

void Engine::cleanup() {
    cleanupSwapChain();

    for (auto &pipeline : pipelines) {
        pipeline.cleanup();
    }
    pipelines.clear();

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

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createDepthResources();
}

void Engine::createSwapChain() {
    SwapChainSupportDetails swapChainSupport =
        appDevice.querySwapChainSupportCurrent();

    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode =
        chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = appWindow.getTargetSurface();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = appDevice.findQueueFamiliesCurrent();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                     indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount,
                            swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void Engine::createImageViews() {
    swapChainImageViews.resize(swapChainImages.size());

    for (uint32_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews[i] =
            appDevice.createImageView(swapChainImages[i], swapChainImageFormat,
                                      VK_IMAGE_ASPECT_COLOR_BIT);
    }
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

void Engine::createDepthResources() {
    VkFormat depthFormat = findDepthFormat();

    depthImageAllocation = image.createImageOut(
        swapChainExtent.width, swapChainExtent.height, depthFormat,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);
    depthImageView = appDevice.createImageView(depthImage, depthFormat,
                                               VK_IMAGE_ASPECT_DEPTH_BIT);
}

VkFormat Engine::findSupportedFormat(const std::vector<VkFormat> &candidates,
                                     VkImageTiling tiling,
                                     VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(*appDevice.getPhysicalDevice(),
                                            format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
                   (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

VkFormat Engine::findDepthFormat() {
    return findSupportedFormat({VK_FORMAT_D32_SFLOAT,
                                VK_FORMAT_D32_SFLOAT_S8_UINT,
                                VK_FORMAT_D24_UNORM_S8_UINT},
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool Engine::hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
           format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void Engine::createCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        commandBuffers[i] = std::make_unique<CommandBuffer>(
            &appDevice, appDevice.getGraphicsCommandPool());
    }
}

void Engine::transitionDepthImageLayout(VkImageLayout fromLayout,
                                        VkImageLayout toLayout,
                                        VkImage &depthImage,
                                        VkCommandBuffer &bufferToRecordOn) {
    VkImageMemoryBarrier imageBarrier{};
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrier.oldLayout = fromLayout;
    imageBarrier.newLayout = toLayout;
    imageBarrier.image = depthImage;
    imageBarrier.subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    VkPipelineStageFlags srcMask;
    VkPipelineStageFlags dstMask;
    if (fromLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        toLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        srcMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        throw std::invalid_argument(
            "Unsupported swapchain image layout transition!");
    }

    vkCmdPipelineBarrier(bufferToRecordOn, srcMask, dstMask, 0, 0, nullptr, 0,
                         nullptr, 1, &imageBarrier);
}

void Engine::transitionSwapchainImageLayout(VkImageLayout fromLayout,
                                            VkImageLayout toLayout,
                                            uint32_t swapchainIdx,
                                            VkCommandBuffer &bufferToRecordOn) {
    VkImageMemoryBarrier imageBarrier{};
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrier.oldLayout = fromLayout;
    imageBarrier.newLayout = toLayout;
    imageBarrier.image = swapChainImages[swapchainIdx];
    imageBarrier.subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    VkPipelineStageFlags srcMask;
    VkPipelineStageFlags dstMask;
    if (fromLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
        toLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
        srcMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dstMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    } else if (fromLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
               toLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        srcMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    } else {
        throw std::invalid_argument(
            "Unsupported swapchain image layout transition!");
    }

    vkCmdPipelineBarrier(bufferToRecordOn, srcMask, dstMask, 0, 0, nullptr, 0,
                         nullptr, 1, &imageBarrier);
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

VkSurfaceFormatKHR Engine::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR Engine::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
Engine::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(appWindow.getWindow(), &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                                   static_cast<uint32_t>(height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                       capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                       capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

Pipeline Engine::createPipeline(const std::string &vertShaderPath,
                                const std::string &fragShaderPath,
                                const Model &model) {
    return Pipeline(&appDevice, vertShaderPath, fragShaderPath,
                    swapChainImageFormat, findDepthFormat(), model,
                    MAX_FRAMES_IN_FLIGHT);
}
