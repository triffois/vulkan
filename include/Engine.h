#pragma once

#include "Pipeline.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "AppInstance.h"
#include "AppWindow.h"
#include "Camera.h"
#include "CommandBuffer.h"
#include "Device.h"
#include "EnginePeripherals.h"
#include "Pipeline.h"
#include "Render.h"
#include "SwapChain.h"

class Engine {
  public:
    Engine();
    ~Engine();

    Engine(const Engine &) = delete;
    Engine &operator=(const Engine &) = delete;

    bool running() const;
    Render startRender();
    void finishRender(Render &render);
    Camera *getCamera();

    Device *getDevice() { return &appDevice; }
    Pipeline createPipeline(const std::string &vertShaderPath,
                            const std::string &fragShaderPath,
                            const Model &model);

  private:
    AppInstance appInstance;
    AppWindow appWindow;
    Device appDevice;
    VkDevice device;
    std::unique_ptr<SwapChain> swapChain;
    std::vector<Pipeline> pipelines;

    EnginePeripheralsManager peripheralsManager;
    Camera mainCamera{0, 0, 2, 0, 1, 0, 90, 0};

    VkCommandPool commandPool;

    std::vector<std::unique_ptr<CommandBuffer>> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;
    bool isRunning = true;

    static void framebufferResizeCallback(GLFWwindow *window, int width,
                                          int height) {
        auto app = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    static std::vector<char> readFile(const std::string &filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    void processKeyboardInput();

    void initVulkan();

    void mainLoop();

    void cleanup();

    void recreateSwapChain();

    void createSwapChain();

    void createImageViews();

    void createCommandPool();

    void createCommandBuffers();

    void createSyncObjects();

    void drawFrame();

    VkShaderModule createShaderModule(const std::vector<char> &code);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    void processEvents();
};
