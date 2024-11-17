#include "Buffer.h"
#include <stdexcept>

Buffer::Buffer(Device *device, VkDeviceSize size, VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties, VmaMemoryUsage memoryUsage,
               VmaAllocationCreateFlagBits allocBits)
    : device(device) {

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(*device->getDevice(), &bufferInfo, nullptr, &buffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(*device->getDevice(), buffer,
                                  &memRequirements);

    auto suitableHeapMemoryType =
        device->findMemoryType(memRequirements.memoryTypeBits, properties);

    VmaAllocationCreateInfo vmaInfo{};
    vmaInfo.memoryTypeBits = 1u << suitableHeapMemoryType;
    vmaInfo.usage = memoryUsage;
    vmaInfo.flags = allocBits;
    vmaInfo.pool = VK_NULL_HANDLE;

    if (device->allocateBufferMemory(&bufferInfo, &vmaInfo, &buffer,
                                     &allocation) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate memory for buffer!");
    }
}

Buffer::~Buffer() {
    if (buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(*device->getDevice(), buffer, nullptr);
        device->freeAllocationMemoryOnDemand(&allocation);
    }
}

void Buffer::copyFrom(Buffer &srcBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer =
        device->getGraphicsCommandPool()->beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer.getBuffer(), buffer, 1,
                    &copyRegion);

    device->getGraphicsCommandPool()->endSingleTimeCommands(commandBuffer);
}

void Buffer::map(void **data) {
    if (device->mapMemory(&allocation, data) != VK_SUCCESS) {
        throw std::runtime_error("Failed to map buffer memory");
    }
}

void Buffer::unmap() { device->unmapMemory(&allocation); }

void Buffer::copyToImage(VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer =
        device->getGraphicsCommandPool()->beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(commandBuffer, getBuffer(), image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    device->getGraphicsCommandPool()->endSingleTimeCommands(commandBuffer);
}