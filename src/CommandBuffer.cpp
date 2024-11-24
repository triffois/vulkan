#include "CommandBuffer.h"
#include <stdexcept>

CommandBuffer::CommandBuffer(Device *device, CommandPool *commandPool,
                             bool isPrimary)
    : device(device), commandPool(commandPool) {

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool->getCommandPool();
    allocInfo.level = isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY
                                : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(*device->getDevice(), &allocInfo,
                                 &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffer!");
    }
}

void CommandBuffer::begin(VkCommandBufferUsageFlags flags) {
    if (isRecording) {
        throw std::runtime_error(
            "Command buffer is already in recording state!");
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flags;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }
    isRecording = true;
}

void CommandBuffer::end() {
    if (!isRecording) {
        throw std::runtime_error("Command buffer is not in recording state!");
    }

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to end recording command buffer!");
    }
    isRecording = false;
}

void CommandBuffer::reset() {
    vkResetCommandBuffer(commandBuffer, 0);
    isRecording = false;
}

void CommandBuffer::submit(VkFence fence, bool waitIdle) {
    if (isRecording) {
        throw std::runtime_error(
            "Cannot submit command buffer while still recording!");
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    device->submitToAvailableGraphicsQueue(&submitInfo, fence, waitIdle);
}
