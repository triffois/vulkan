#pragma once

#include "Device.h"
#include "DeviceMemoryAllocation.h"
#include <vulkan/vulkan.h>

class Buffer {
  public:
    Buffer(Device *device, VkDeviceSize size, VkBufferUsageFlags usage,
           VkMemoryPropertyFlags properties,
           VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO,
           VmaAllocationCreateFlagBits allocBits =
               VMA_ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT);
    ~Buffer();

    Buffer(const Buffer &) = delete;
    Buffer &operator=(const Buffer &) = delete;

    VkBuffer getBuffer() const { return buffer; }

    void copyFrom(Buffer &srcBuffer, VkDeviceSize size);
    void copyToImage(VkImage image, uint32_t width, uint32_t height);
    void copyToImage(VkImage image,
                     const std::vector<VkBufferImageCopy> &regions);

    void map(void **data);
    void unmap();

    void recordCopyTo(VkCommandBuffer cmdBuffer, Buffer &dstBuffer,
                      VkDeviceSize size) const;
    void recordCopyToImage(VkCommandBuffer cmdBuffer, VkImage image,
                           uint32_t width, uint32_t height) const;

  private:
    Device *device;
    VkBuffer buffer;
    DeviceMemoryAllocationHandle allocation;
};
