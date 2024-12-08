#pragma once

#include <variant>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

// I don't remember why I separated this out - probably some circular dependency
// issue; it's almost 2 am again leave me alone pls :p

struct DeviceMemoryAllocation {
    std::variant<VkBuffer, VkImage> allocatedObject{};
    VmaAllocation allocation{};

    bool operator==(const DeviceMemoryAllocation &other) const {
        if (allocatedObject.index() != other.allocatedObject.index())
            return false;

        if (allocatedObject.index() == 0 &&
            std::get<0>(allocatedObject) == std::get<0>(other.allocatedObject))
            return true;
        else if (allocatedObject.index() == 1 &&
                 std::get<1>(allocatedObject) ==
                     std::get<1>(other.allocatedObject))
            return true;

        return false;
    }

    bool operator!=(const DeviceMemoryAllocation &other) const {
        return !operator==(other);
    }
};

struct DeviceMemoryAllocationHandle {
    friend class Device;

  private:
    DeviceMemoryAllocation *allocationInfo{};
};
