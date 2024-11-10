#pragma once

#include "INeedCleanUp.h"
#include "commonstructs.h"
#include "AppInstance.h"
#include "AppWindow.h"

class Device :  public INeedCleanUp {

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

private:
    void pickPhysicalDevice();
    void createLogicalDevice();

    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

private:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    const AppWindow *appWindow;
    const AppInstance *appInstance;
};