#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class Device;

class CommandPool {
public:
    CommandPool(Device* device, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
    ~CommandPool();

    CommandPool(const CommandPool&) = delete;
    CommandPool& operator=(const CommandPool&) = delete;

    VkCommandPool getCommandPool() const { return commandPool; }
    
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    
    std::vector<VkCommandBuffer> allocateCommandBuffers(uint32_t count);

private:
    Device* device;
    VkCommandPool commandPool;
}; 