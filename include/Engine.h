#pragma once

#include "Buffer.h"
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
#include "Device.h"
#include "EnginePeripherals.h"

class Engine {
  public:
    Engine();
    ~Engine();

    Engine(const Engine &) = delete;
    Engine &operator=(const Engine &) = delete;

    void run();

    Camera *getCamera();
    void addPipeline(const std::string &vertShaderPath,
                     const std::string &fragShaderPath, const Model &model);

  private:
    AppInstance appInstance;
    AppWindow appWindow;
    Device appDevice;
    VkDevice device;

    std::vector<Pipeline> pipelines;

    EnginePeripheralsManager peripheralsManager;
    Camera mainCamera{0, 0, 2, 0, 1, 0, 90, 0};

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;

    VkCommandPool commandPool;

    VkImage depthImage;
    VkImageView depthImageView;
    DeviceMemoryAllocationHandle depthImageAllocation;

    VkImage textureImage;

    VkImageView textureImageView;
    VkSampler textureSampler;

    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;

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

    void cleanupSwapChain();

    void cleanup();

    void recreateSwapChain();

    void createSwapChain();

    void createImageViews();

    void createCommandPool();

    void createDepthResources();

    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                                 VkImageTiling tiling,
                                 VkFormatFeatureFlags features);

    VkFormat findDepthFormat();

    bool hasStencilComponent(VkFormat format);

    void createTextureImage();

    void createTextureImageView();

    void createTextureSampler();

    VkImageView createImageView(VkImage image, VkFormat format,
                                VkImageAspectFlags aspectFlags);

    DeviceMemoryAllocationHandle createImage(
        uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
        VkImage &image,
        VmaMemoryUsage memoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
        VmaAllocationCreateFlagBits allocFlagBits =
            VMA_ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT);

    void transitionImageLayout(VkImage image, VkFormat format,
                               VkImageLayout oldLayout,
                               VkImageLayout newLayout);

    void copyBufferToImage(Buffer &buffer, VkImage image, uint32_t width,
                           uint32_t height);

    void createVertexBuffer();

    void createIndexBuffer();

    void createUniformBuffers();

    VkCommandBuffer beginSingleTimeCommands();

    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    void createCommandBuffers();

    void transitionDepthImageLayout(VkImageLayout fromLayout,
                                    VkImageLayout toLayout, VkImage &depthImage,
                                    VkCommandBuffer &bufferToRecordOn);

    void transitionSwapchainImageLayout(VkImageLayout fromLayout,
                                        VkImageLayout toLayout,
                                        uint32_t swapchainIdx,
                                        VkCommandBuffer &bufferToRecordOn);

    void recordCommandBuffer(VkCommandBuffer commandBuffer,
                             uint32_t imageIndex);

    void createSyncObjects();

    void drawFrame();

    VkShaderModule createShaderModule(const std::vector<char> &code);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
};
