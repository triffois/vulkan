#pragma once

#include "Camera.h"
#include "GlobalResources.h"
#include "Scene.h"
#include "SwapChain.h"
#include <vulkan/vulkan.h>

class Render {
  public:
    Render(GlobalResources *globalResources, SwapChain *swapChain,
           VkCommandBuffer commandBuffer, uint32_t imageIndex,
           uint32_t currentFrame, VkSemaphore imageAvailableSemaphore,
           VkSemaphore renderFinishedSemaphore, VkFence inFlightFence,
           Camera &camera);
    ~Render() = default;

    // Prevent copying
    Render(const Render &) = delete;
    Render &operator=(const Render &) = delete;

    void submit(Scene &scene);
    bool finish();

  private:
    GlobalResources *globalResources;
    SwapChain *swapChain;
    VkCommandBuffer commandBuffer;
    uint32_t imageIndex;
    uint32_t currentFrame;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
    Camera &camera;
    bool isFinished = false;

    void recordRenderingCommands(Scene &scene);
    void recordRenderingCommands(RenderPass &pass);

    void submitCommandBuffer();
};
