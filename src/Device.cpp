#include "Device.h"
#include <algorithm>
#include <set>
#include <stdexcept>
#include <variant>
#include <vector>

#include "ValidationLayersInfo.h"

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME};

void Device::init(const AppWindow *appWindow, const AppInstance *appInstance) {
    this->appWindow = appWindow;
    this->appInstance = appInstance;

    pickPhysicalDevice();
    createLogicalDevice();
    createAllocator();

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    graphicsCommandPool =
        new CommandPool(this, indices.graphicsFamily.value(),
                        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
}

Device::~Device() {
    delete graphicsCommandPool;

    std::for_each(allocations.begin(), allocations.end(),
                  [this](auto &allocationToCleanUp) {
                      freeAllocation(&allocationToCleanUp.second);
                  });

    vmaDestroyAllocator(deviceMemoryAllocator);
    vkDestroyDevice(device, nullptr);
}

const VkDevice *Device::getDevice() const { return &device; }

const VkPhysicalDevice *Device::getPhysicalDevice() const {
    return &physicalDevice;
}

QueueFamilyIndices Device::findQueueFamiliesCurrent() {
    return findQueueFamilies(physicalDevice);
}

SwapChainSupportDetails Device::querySwapChainSupportCurrent() {
    return querySwapChainSupport(physicalDevice);
}

void Device::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(*appInstance->getInstance(), &deviceCount,
                               nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(*appInstance->getInstance(), &deviceCount,
                               devices.data());

    for (const auto &device : devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void Device::createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                              indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature{};
    dynamicRenderingFeature.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamicRenderingFeature.dynamicRendering = VK_TRUE;

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &dynamicRenderingFeature;

    createInfo.queueCreateInfoCount =
        static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount =
        static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount =
            static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device!");
    }

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void Device::createAllocator() {
    VmaAllocatorCreateInfo info{};
    info.device = device;
    info.physicalDevice = physicalDevice;
    info.instance = *appInstance->getInstance();
    info.vulkanApiVersion = VK_API_VERSION_1_3;

    if (vmaCreateAllocator(&info, &deviceMemoryAllocator) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a VMA allocator.");
    }
}

bool Device::isDeviceSuitable(VkPhysicalDevice device) {
    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport =
            querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() &&
                            !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return indices.isComplete() && extensionsSupported && swapChainAdequate &&
           supportedFeatures.samplerAnisotropy;
}

bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                             deviceExtensions.end());

    for (const auto &extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            device, i, appWindow->getTargetSurface(), &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;
    const auto surface = appWindow->getTargetSurface();

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                              &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                             details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                              &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

void Device::freeAllocation(DeviceMemoryAllocation *allocationToFree) {
    VmaAllocationInfo info;
    vmaGetAllocationInfo(deviceMemoryAllocator, allocationToFree->allocation,
                         &info);

    if (allocationToFree->allocatedObject.index() == 0) {
#ifndef NDEBUG
        std::cout << "Destroying buffer." << std::endl;
#endif

        auto bufferToFree = std::get<0>(allocationToFree->allocatedObject);
        vmaDestroyBuffer(deviceMemoryAllocator, bufferToFree,
                         allocationToFree->allocation);
    } else {
#ifndef NDEBUG
        std::cout << "Destroying image." << std::endl;
#endif

        auto imageToFree = std::get<1>(allocationToFree->allocatedObject);

        vmaDestroyImage(deviceMemoryAllocator, imageToFree,
                        allocationToFree->allocation);
    }
}

AllocationIdentifier Device::generateNewAllocationId() {
    auto newId = AllocationIdentifier();
    newId.id = allocationIdCounter++;

    return newId;
}

VkResult Device::submitToAvailableGraphicsQueue(const VkSubmitInfo *info,
                                                VkFence submitFence,
                                                bool ifWaitIdle) const {
    auto res = vkQueueSubmit(graphicsQueue, 1, info, submitFence);
    if (ifWaitIdle)
        vkQueueWaitIdle(graphicsQueue);
    return res;
}

VkResult
Device::submitToAvailablePresentQueue(const VkPresentInfoKHR *info) const {
    return vkQueuePresentKHR(graphicsQueue, info);
}

VkResult
Device::allocateBufferMemory(const VkBufferCreateInfo *bufCreateInfo,
                             const VmaAllocationCreateInfo *allocCreateInfo,
                             VkBuffer *targetBuf,
                             DeviceMemoryAllocationHandle *allocationInfo) {
    assert(allocationInfo != nullptr);

    DeviceMemoryAllocation newAllocationInfo{};
    auto res =
        vmaCreateBuffer(deviceMemoryAllocator, bufCreateInfo, allocCreateInfo,
                        targetBuf, &newAllocationInfo.allocation, nullptr);
    newAllocationInfo.allocatedObject.emplace<0>(*targetBuf);

    if (res == VK_SUCCESS) {
        auto newId = generateNewAllocationId();

        allocations.insert(std::make_pair(newId, newAllocationInfo));
        allocationInfo->identifier = newId;

#ifndef NDEBUG
        std::cout << "ID for new buffer allocation : " << newId.id << std::endl;
#endif
    }

    return res;
}

VkResult
Device::allocateImageMemory(const VkImageCreateInfo *imgCreateInfo,
                            const VmaAllocationCreateInfo *allocCreateInfo,
                            VkImage *targetImage,
                            DeviceMemoryAllocationHandle *allocationInfo) {
    DeviceMemoryAllocation newAllocationInfo{};
    auto res =
        vmaCreateImage(deviceMemoryAllocator, imgCreateInfo, allocCreateInfo,
                       targetImage, &newAllocationInfo.allocation, nullptr);
    newAllocationInfo.allocatedObject.emplace<1>(*targetImage);

    if (res == VK_SUCCESS) {
        auto newId = generateNewAllocationId();

        allocations.insert(std::make_pair(newId, newAllocationInfo));
        allocationInfo->identifier = newId;

#ifndef NDEBUG
        std::cout << "ID for new image allocation : " << newId.id << std::endl;
#endif
    }

    return res;
}

VkResult Device::mapMemory(DeviceMemoryAllocationHandle *allocationInfo,
                           void **ppData) const {
    if (allocations.find(allocationInfo->identifier) == allocations.end())
        return VK_ERROR_INVALID_EXTERNAL_HANDLE;

    return vmaMapMemory(deviceMemoryAllocator,
                        allocations.at(allocationInfo->identifier).allocation,
                        ppData);
}

VkResult
Device::unmapMemory(DeviceMemoryAllocationHandle *allocationInfo) const {
    if (allocations.find(allocationInfo->identifier) == allocations.end())
        return VK_ERROR_INVALID_EXTERNAL_HANDLE;

    vmaUnmapMemory(deviceMemoryAllocator,
                   allocations.at(allocationInfo->identifier).allocation);
    return VK_SUCCESS;
}

VkResult Device::freeAllocationMemoryOnDemand(
    DeviceMemoryAllocationHandle *allocationInfo) {
    if (allocations.find(allocationInfo->identifier) == allocations.end())
        throw std::runtime_error("Attempted to free unallocated memory!");

#ifndef NDEBUG
    std::cout << "Allocation ID to free : " << allocationInfo->identifier.id
              << std::endl;
#endif

    freeAllocation(&allocations.at(allocationInfo->identifier));
    allocations.erase(allocationInfo->identifier);

    return VK_SUCCESS;
}

VkImageView Device::createImageView(VkImage image, VkFormat format,
                                    VkImageAspectFlags aspectFlags) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

uint32_t Device::findMemoryType(uint32_t typeFilter,
                                VkMemoryPropertyFlags properties) const {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(*getPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) ==
                properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}
