#pragma once

#include "Camera.h"
#include "Device.h"
#include "Pipeline.h"
#include <vulkan/vulkan.h>

class Render {
  public:
    Render(Device *device, VkSwapchainKHR swapChain,
           std::vector<VkImage> &swapChainImages,
           std::vector<VkImageView> &swapChainImageViews,
           VkImageView depthAttachment, VkCommandBuffer commandBuffer,
           uint32_t imageIndex, uint32_t currentFrame,
           VkSemaphore imageAvailableSemaphore,
           VkSemaphore renderFinishedSemaphore, VkFence inFlightFence,
           VkExtent2D swapChainExtent, Camera &camera);
    ~Render() = default;

    // Prevent copying
    Render(const Render &) = delete;
    Render &operator=(const Render &) = delete;

    void submit(const Pipeline &pipeline);
    bool finish(); // Returns true if swapchain needs recreation

  private:
    Device *device;
    VkSwapchainKHR swapChain;
    VkCommandBuffer commandBuffer;
    uint32_t imageIndex;
    uint32_t currentFrame;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
    VkExtent2D swapChainExtent;
    Camera &camera;
    bool isFinished = false;
    std::vector<VkImage> &swapChainImages;

    void recordRenderingCommands(const Pipeline &pipeline);
    void submitCommandBuffer();
    bool presentFrame();
};
