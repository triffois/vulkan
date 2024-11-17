#include "Buffer.h"
#include <stdexcept>

Buffer::Buffer(Device* device, VkDeviceSize size, VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties, VmaMemoryUsage memoryUsage,
               VmaAllocationCreateFlagBits allocBits) 
    : device(device) {
    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(*device->getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(*device->getDevice(), buffer, &memRequirements);

    auto suitableHeapMemoryType = findMemoryType(memRequirements.memoryTypeBits, properties);

    VmaAllocationCreateInfo vmaInfo{};
    vmaInfo.memoryTypeBits = 1u << suitableHeapMemoryType;
    vmaInfo.usage = memoryUsage;
    vmaInfo.flags = allocBits;
    vmaInfo.pool = VK_NULL_HANDLE;

    if (device->allocateBufferMemory(&bufferInfo, &vmaInfo, &buffer, &allocation) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate memory for buffer!");
    }
}

Buffer::~Buffer() {
    if (buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(*device->getDevice(), buffer, nullptr);
        device->freeAllocationMemoryOnDemand(&allocation);
    }
}

void Buffer::copyFrom(Buffer& srcBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = device->beginSingleTimeCommands();
    
    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer.getBuffer(), buffer, 1, &copyRegion);
    
    device->endSingleTimeCommands(commandBuffer);
}

void Buffer::map(void** data) {
    if (device->mapMemory(&allocation, data) != VK_SUCCESS) {
        throw std::runtime_error("Failed to map buffer memory");
    }
}

void Buffer::unmap() {
    device->unmapMemory(&allocation);
}

uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(*device->getPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
} 