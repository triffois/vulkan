#pragma once

#include "CommandPool.h"
#include "Device.h"
#include <vulkan/vulkan.h>

class CommandBuffer {
  public:
    CommandBuffer(Device *device, CommandPool *commandPool,
                  bool isPrimary = true);
    ~CommandBuffer() = default;

    CommandBuffer(const CommandBuffer &) = delete;
    CommandBuffer &operator=(const CommandBuffer &) = delete;

    // Core command buffer operations
    void begin(VkCommandBufferUsageFlags flags = 0);
    void end();
    void reset();
    void submit(VkFence fence = VK_NULL_HANDLE, bool waitIdle = false);

    // Access to underlying command buffer for recording commands
    operator VkCommandBuffer() const { return commandBuffer; }
    VkCommandBuffer getCommandBuffer() const { return commandBuffer; }

  private:
    Device *device;
    CommandPool *commandPool;
    VkCommandBuffer commandBuffer;
    bool isRecording = false;
};
