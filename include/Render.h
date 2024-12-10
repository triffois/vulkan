#pragma once

#include "Camera.h"
#include "GlobalResources.h"
#include "Renderable.h"
#include <vulkan/vulkan.h>

class Render {
  public:
    Render(GlobalResources *globalResources, VkCommandBuffer commandBuffer,
           uint32_t imageIndex, uint32_t currentFrame,
           VkSemaphore imageAvailableSemaphore,
           VkSemaphore renderFinishedSemaphore, VkFence inFlightFence);
    ~Render() = default;

    // Prevent copying
    Render(const Render &) = delete;
    Render &operator=(const Render &) = delete;

    void submit(Renderable &renderable);
    bool finish();

  private:
    GlobalResources *globalResources;
    VkCommandBuffer commandBuffer;
    uint32_t imageIndex;
    uint32_t currentFrame;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
    bool isFinished = false;

    void recordRenderingCommands(Renderable &scene);
    void recordRenderingCommands(RenderPass &pass);

    void submitCommandBuffer();
};
