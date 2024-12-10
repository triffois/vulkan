#pragma once

#define GLFW_INCLUDE_VULKAN
#include "Device.h"
#include "cstring"
#include <cstdint>
#include <string>

class Image {
  public:
    Image() = delete;

    ~Image();

    Image(const Image &) = delete;

    Image(Image &&) = delete;

    Image(Device &device) : device(device) {}

    Image &operator=(const Image &) = delete;

    Image &operator=(Image &&) = delete;

    void cleanUp();

    void createImage(uint32_t width, uint32_t height, VkFormat format,
                     VkImageTiling tiling, VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties,
                     VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY,
                     VmaAllocationCreateFlagBits allocFlagBits =
                         VMA_ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT,
                     uint32_t arrayLayers = 1);

    VkImageView
    createImageView(VkFormat format, VkImageAspectFlags aspectFlags,
                    VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D,
                    uint32_t layerCount = 1);

    void transitionImageLayout(VkFormat format, VkImageLayout oldLayout,
                               VkImageLayout newLayout);

    void createTextureImage(const std::string &texturePath);

    void createTextureImageView();

    void createTextureImageFromMemory(const unsigned char *pixels, int width,
                                      int height, int channels);

    void transitionImageLayout(VkImageLayout oldLayout,
                               VkImageLayout newLayout);

    VkImage getVkImage() const { return image; };
    VkImageView getVkImageView() const { return imageView; };

    void recordTransitionLayout(VkCommandBuffer cmdBuffer, VkFormat format,
                                VkImageLayout oldLayout,
                                VkImageLayout newLayout) const;

  private:
    Device &device;
    VkImage image = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;
    DeviceMemoryAllocationHandle imageAllocationHandle;
};
