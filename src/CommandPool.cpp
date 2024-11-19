#include "CommandPool.h"
#include "Device.h"
#include <stdexcept>

CommandPool::CommandPool(Device *device, uint32_t queueFamilyIndex,
                         VkCommandPoolCreateFlags flags)
    : device(device) {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = flags;
    poolInfo.queueFamilyIndex = queueFamilyIndex;

    if (vkCreateCommandPool(*device->getDevice(), &poolInfo, nullptr,
                            &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }
}

CommandPool::~CommandPool() {
    if (commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(*device->getDevice(), commandPool, nullptr);
    }
}

std::vector<VkCommandBuffer>
CommandPool::allocateCommandBuffers(uint32_t count) {
    std::vector<VkCommandBuffer> commandBuffers(count);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    if (vkAllocateCommandBuffers(*device->getDevice(), &allocInfo,
                                 commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers!");
    }

    return commandBuffers;
}
