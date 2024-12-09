#pragma once

#include <list>
#include <unordered_map>

#include "AppInstance.h"
#include "AppWindow.h"
#include "CommandPool.h"
#include "DeviceMemoryAllocation.h"
#include "INeedCleanUp.h"
#include "commonstructs.h"

class Device : public INeedCleanUp {
  friend struct DeviceMemoryAllocationHandle;

public:
  void init(const AppWindow *appWindow, const AppInstance *appInstance);

  Device() = default;

  ~Device() = default;

  Device(const Device &) = delete;

  Device(Device &&) = delete;

  Device &operator=(const Device &) = delete;

  Device &operator=(Device &&) = delete;

  void cleanUp(const AppContext &context) override;

  const VkDevice *getDevice() const;

  const VkPhysicalDevice *getPhysicalDevice() const;

  QueueFamilyIndices findQueueFamiliesCurrent();

  SwapChainSupportDetails querySwapChainSupportCurrent();

  VkResult submitToAvailableGraphicsQueue(const VkSubmitInfo *info,
                                          VkFence submitFence,
                                          bool ifWaitIdle = false) const;

  VkResult submitToAvailablePresentQueue(const VkPresentInfoKHR *info) const;

  // TODO: move out this bunch into a separate abstraction
  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties) const;

  VkImageView createImageView(VkImage image, VkFormat format,
                              VkImageAspectFlags aspectFlags);

  // to abstract user from any particular memory allocation algorithm, the
  // 'descriptor' returned to user in the pointer to AllocationInfoCache cast
  // to void *
  VkResult
  allocateBufferMemory(const VkBufferCreateInfo *bufCreateInfo,
                       const VmaAllocationCreateInfo *allocCreateInfo,
                       VkBuffer *targetBuf,
                       DeviceMemoryAllocationHandle *allocationInfo);

  VkResult allocateImageMemory(const VkImageCreateInfo *imgCreateInfo,
                               const VmaAllocationCreateInfo *allocCreateInfo,
                               VkImage *targetImage,
                               DeviceMemoryAllocationHandle *allocationInfo);

  VkResult
  freeAllocationMemoryOnDemand(DeviceMemoryAllocationHandle *allocationInfo);

  VkResult mapMemory(DeviceMemoryAllocationHandle *allocationInfo,
                     void **ppData) const;

  VkResult unmapMemory(DeviceMemoryAllocationHandle *allocationInfo) const;

  CommandPool *getGraphicsCommandPool() { return graphicsCommandPool; }

  uint32_t getMaxFramesInFlight() const { return MAX_FRAMES_IN_FLIGHT; }

private:
  void pickPhysicalDevice();

  void createLogicalDevice();

  void createAllocator();

  bool isDeviceSuitable(VkPhysicalDevice device);

  bool checkDeviceExtensionSupport(VkPhysicalDevice device);

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

  void freeAllocation(DeviceMemoryAllocation *allocationToFree);

  AllocationIdentifier generateNewAllocationId();

private:
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  VmaAllocator deviceMemoryAllocator;

  const AppWindow *appWindow;
  const AppInstance *appInstance;

  CommandPool *graphicsCommandPool;

  std::unordered_map<AllocationIdentifier, DeviceMemoryAllocation, AllocationIdentifier, AllocationIdentifier>
  allocations;
  unsigned long int allocationIdCounter{0}; //will overflow someday

  static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
};
