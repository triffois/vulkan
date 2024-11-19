#pragma once

#include "AppWindow.h"
#include "Device.h"
#include "Image.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

class SwapChain {
  public:
    SwapChain(Device *device, AppWindow *appWindow);
    ~SwapChain();

    SwapChain(const SwapChain &) = delete;
    SwapChain &operator=(const SwapChain &) = delete;

    VkSwapchainKHR getSwapChain() const { return swapChain; }
    VkFormat getImageFormat() const { return swapChainImageFormat; }
    VkExtent2D getExtent() const { return swapChainExtent; }
    uint32_t getImageCount() const {
        return static_cast<uint32_t>(swapChainImages.size());
    }
    VkImageView getImageView(size_t index) const {
        return swapChainImageViews[index];
    }
    VkImage getImage(size_t index) const { return swapChainImages[index]; }
    VkImageView getDepthImageView() const;
    VkImage getDepthImage() const;
    VkFormat findDepthFormat() const;

    void recreate();
    void cleanup();

    void transitionImageLayout(VkImageLayout fromLayout, VkImageLayout toLayout,
                               uint32_t swapchainIdx,
                               VkCommandBuffer &bufferToRecordOn);

    void transitionDepthImageLayout(VkImageLayout fromLayout,
                                    VkImageLayout toLayout,
                                    VkCommandBuffer &bufferToRecordOn);

  private:
    void init();
    void createExtent();
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void cleanupDepthResources();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                                 VkImageTiling tiling,
                                 VkFormatFeatureFlags features) const;
    bool hasStencilComponent(VkFormat format) const;

    Device *device;
    AppWindow *appWindow;
    VkExtent2D windowExtent;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    std::unique_ptr<Image> depthImage;
    VkImageView depthImageView;
    Image image;
};
