#pragma once

#include "Camera.h"
#include "Device.h"
#include "Pipeline.h"
#include "SwapChain.h"
#include <vulkan/vulkan.h>

class Render {
  public:
    Render(Device *device, SwapChain *swapChain, VkCommandBuffer commandBuffer,
           uint32_t imageIndex, uint32_t currentFrame,
           VkSemaphore imageAvailableSemaphore,
           VkSemaphore renderFinishedSemaphore, VkFence inFlightFence,
           Camera &camera);
    ~Render() = default;

    // Prevent copying
    Render(const Render &) = delete;
    Render &operator=(const Render &) = delete;

    void submit(const Pipeline &pipeline);
    bool finish();

  private:
    Device *device;
    SwapChain *swapChain;
    VkCommandBuffer commandBuffer;
    uint32_t imageIndex;
    uint32_t currentFrame;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
    Camera &camera;
    bool isFinished = false;

    void recordRenderingCommands(const Pipeline &pipeline);
    void submitCommandBuffer();
};
