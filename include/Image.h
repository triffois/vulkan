#pragma once

#define GLFW_INCLUDE_VULKAN
#include "Device.h"
#include "INeedCleanUp.h"
#include "cstring"
#include <cstdint>
#include <string>

class Image : public INeedCleanUp {
  public:
    Image() = default;
    ~Image() = default;

    Image(const Image &) = delete;
    Image(Image &&) = delete;
    Image(Device &device) : device(device) {}

    Image &operator=(const Image &) = delete;
    Image &operator=(Image &&) = delete;

    void cleanUp(const AppContext &context) override;
    void createImage(uint32_t width, uint32_t height, VkFormat format,
                     VkImageTiling tiling, VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties,
                     VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY,
                     VmaAllocationCreateFlagBits allocFlagBits =
                         VMA_ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT);
    VkImageView createImageView(VkFormat format,
                                VkImageAspectFlags aspectFlags);
    void transitionImageLayout(VkFormat format, VkImageLayout oldLayout,
                               VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);
    void createTextureImage(const std::string &texturePath);
    void createTextureImageView();
    void createTextureImageFromMemory(const unsigned char* pixels, 
                                    int width, 
                                    int height, 
                                    int channels);
    void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);

    VkImage getVkImage() const { return image; };
    VkImageView getVkImageView() const { return imageView; };

  private:
    Device &device;
    VkImage image;
    VkImageView imageView;
    VmaAllocation imageAllocation;
    VmaAllocator allocator;
};
