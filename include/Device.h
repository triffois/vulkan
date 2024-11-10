#pragma once

#include <list>
#include <variant>

#include "INeedCleanUp.h"
#include "commonstructs.h"
#include "AppInstance.h"
#include "AppWindow.h"
#include "VMA.h"

struct DeviceMemoryAllocationHandle;

class Device :  public INeedCleanUp {
    friend struct DeviceMemoryAllocationHandle;

public:
    void init(const AppWindow *appWindow, const AppInstance *appInstance);

    Device() = default;
    ~Device() = default;

    Device(const Device& ) = delete;
    Device(Device&& ) = delete;

    Device& operator=(const Device& ) = delete;
    Device& operator=(Device&& ) = delete;

    void cleanUp(const AppContext &context) override;
    const VkDevice* getDevice() const;
    const VkPhysicalDevice *getPhysicalDevice() const;

    QueueFamilyIndices findQueueFamiliesCurrent();
    SwapChainSupportDetails querySwapChainSupportCurrent();

    VkResult submitToAvailableGraphicsQueue(const VkSubmitInfo *info, VkFence submitFence, bool ifWaitIdle = false) const;
    VkResult submitToAvailablePresentQueue(const VkPresentInfoKHR *info) const;

    //to abstract user from any particular memory allocation algorithm, the 'descriptor' returned to user in the pointer to AllocationInfoCache cast to void *
    VkResult allocateBufferMemory(const VkBufferCreateInfo *bufCreateInfo, const VmaAllocationCreateInfo *allocCreateInfo, VkBuffer *targetBuf, DeviceMemoryAllocationHandle *allocationInfo);
    VkResult allocateImageMemory(const VkImageCreateInfo *imgCreateInfo, const VmaAllocationCreateInfo *allocCreateInfo, VkImage *targetImage, DeviceMemoryAllocationHandle *allocationInfo);

    VkResult freeAllocationMemoryOnDemand(DeviceMemoryAllocationHandle *allocationInfo);

    VkResult mapMemory(DeviceMemoryAllocationHandle *allocationInfo, void **ppData) const;
    VkResult unmapMemory(DeviceMemoryAllocationHandle *allocationInfo) const;

private:
    struct DeviceMemoryAllocation;

    void pickPhysicalDevice();
    void createLogicalDevice();
    void createAllocator();

    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    void freeAllocation(DeviceMemoryAllocation *allocationToFree);

private:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VmaAllocator deviceMemoryAllocator;

    const AppWindow *appWindow;
    const AppInstance *appInstance;

    struct DeviceMemoryAllocation {
        std::variant<VkBuffer, VkImage> allocatedObject{};
        VmaAllocation allocation{};

        bool operator==(const DeviceMemoryAllocation &other) const {
            if(allocatedObject.index() != other.allocatedObject.index())
                return false;

            if(allocatedObject.index() == 0 && std::get<0>(allocatedObject) == std::get<0>(other.allocatedObject))
                return true;
            else if(allocatedObject.index() == 1 && std::get<1>(allocatedObject) == std::get<1>(other.allocatedObject))
                return true;

            return false;
        }

        bool operator != (const DeviceMemoryAllocation &other) const {
            return !operator==(other);
        }
    };

    std::list<DeviceMemoryAllocation> allocations;
};

struct DeviceMemoryAllocationHandle {
    friend class Device;
private:
    Device::DeviceMemoryAllocation *allocationInfo{};
};