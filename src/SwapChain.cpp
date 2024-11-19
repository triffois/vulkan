#include "SwapChain.h"
#include <algorithm>
#include <limits>
#include <stdexcept>

SwapChain::SwapChain(Device *device, AppWindow *appWindow)
    : device(device), appWindow(appWindow), image(Image(*device)) {
    init();
}

SwapChain::~SwapChain() { cleanup(); }

void SwapChain::init() {
    createExtent();
    createSwapChain();
    createImageViews();
    createDepthResources();
}

void SwapChain::cleanup() {
    cleanupDepthResources();
    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(*device->getDevice(), imageView, nullptr);
    }
    swapChainImageViews.clear();

    if (swapChain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(*device->getDevice(), swapChain, nullptr);
        swapChain = VK_NULL_HANDLE;
    }
}

void SwapChain::recreate() {
    cleanup();
    init();
}

void SwapChain::createExtent() {
    SwapChainSupportDetails swapChainSupport =
        device->querySwapChainSupportCurrent();

    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode =
        chooseSwapPresentMode(swapChainSupport.presentModes);
    windowExtent = chooseSwapExtent(swapChainSupport.capabilities);
};

void SwapChain::createSwapChain() {
    SwapChainSupportDetails swapChainSupport =
        device->querySwapChainSupportCurrent();

    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode =
        chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = appWindow->getTargetSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = device->findQueueFamiliesCurrent();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                     indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(*device->getDevice(), &createInfo, nullptr,
                             &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(*device->getDevice(), swapChain, &imageCount,
                            nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(*device->getDevice(), swapChain, &imageCount,
                            swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void SwapChain::createImageViews() {
    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews[i] =
            device->createImageView(swapChainImages[i], swapChainImageFormat,
                                    VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR SwapChain::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    int width, height;
    glfwGetFramebufferSize(appWindow->getWindow(), &width, &height);

    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
}

void SwapChain::createDepthResources() {
    VkFormat depthFormat = findDepthFormat();

    depthImageAllocation = image.createImageOut(
        swapChainExtent.width, swapChainExtent.height, depthFormat,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);
    depthImageView = device->createImageView(depthImage, depthFormat,
                                             VK_IMAGE_ASPECT_DEPTH_BIT);
}

void SwapChain::cleanupDepthResources() {
    if (depthImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(*device->getDevice(), depthImageView, nullptr);
        device->freeAllocationMemoryOnDemand(&depthImageAllocation);
        depthImageView = VK_NULL_HANDLE;
    }
}

VkFormat SwapChain::findDepthFormat() const {
    return findSupportedFormat({VK_FORMAT_D32_SFLOAT,
                                VK_FORMAT_D32_SFLOAT_S8_UINT,
                                VK_FORMAT_D24_UNORM_S8_UINT},
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat SwapChain::findSupportedFormat(const std::vector<VkFormat> &candidates,
                                        VkImageTiling tiling,
                                        VkFormatFeatureFlags features) const {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(*device->getPhysicalDevice(),
                                            format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
                   (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw std::runtime_error("Failed to find supported format!");
}

bool SwapChain::hasStencilComponent(VkFormat format) const {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
           format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void SwapChain::transitionImageLayout(VkImageLayout fromLayout,
                                      VkImageLayout toLayout,
                                      uint32_t swapchainIdx,
                                      VkCommandBuffer &bufferToRecordOn) {
    VkImageMemoryBarrier imageBarrier{};
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrier.oldLayout = fromLayout;
    imageBarrier.newLayout = toLayout;
    imageBarrier.image = swapChainImages[swapchainIdx];
    imageBarrier.subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    VkPipelineStageFlags srcMask;
    VkPipelineStageFlags dstMask;
    if (fromLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
        toLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
        srcMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dstMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    } else if (fromLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
               toLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        srcMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    } else {
        throw std::invalid_argument(
            "Unsupported swapchain image layout transition!");
    }

    vkCmdPipelineBarrier(bufferToRecordOn, srcMask, dstMask, 0, 0, nullptr, 0,
                         nullptr, 1, &imageBarrier);
}

void SwapChain::transitionDepthImageLayout(VkImageLayout fromLayout,
                                           VkImageLayout toLayout,
                                           VkCommandBuffer &bufferToRecordOn) {
    VkImageMemoryBarrier imageBarrier{};
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrier.oldLayout = fromLayout;
    imageBarrier.newLayout = toLayout;
    imageBarrier.image = depthImage;
    imageBarrier.subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    VkPipelineStageFlags srcMask;
    VkPipelineStageFlags dstMask;
    if (fromLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        toLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        srcMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        throw std::invalid_argument(
            "Unsupported depth image layout transition!");
    }

    vkCmdPipelineBarrier(bufferToRecordOn, srcMask, dstMask, 0, 0, nullptr, 0,
                         nullptr, 1, &imageBarrier);
}
