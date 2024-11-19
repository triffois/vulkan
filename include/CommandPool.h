#pragma once

#include <vector>
#include <vulkan/vulkan.h>

class Device;

class CommandPool {
  public:
    CommandPool(Device *device, uint32_t queueFamilyIndex,
                VkCommandPoolCreateFlags flags = 0);
    ~CommandPool();

    CommandPool(const CommandPool &) = delete;
    CommandPool &operator=(const CommandPool &) = delete;

    VkCommandPool getCommandPool() const { return commandPool; }

    std::vector<VkCommandBuffer> allocateCommandBuffers(uint32_t count);

  private:
    Device *device;
    VkCommandPool commandPool;
};
