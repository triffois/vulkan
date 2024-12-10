#pragma once

#include "Device.h"

#include <variant>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

// I don't remember why I separated this out - probably some circular dependency
// issue; it's almost 2 am again leave me alone pls :p

struct AllocationIdentifier {
    friend class Device;

  public:
    size_t operator()(const AllocationIdentifier &id) const { return id.id; }

    bool operator()(const AllocationIdentifier &id1,
                    const AllocationIdentifier &id2) const {
        return id1.id == id2.id;
    }

  private:
    unsigned long int id{0};
};

struct DeviceMemoryAllocationHandle {
    friend class Device;

  private:
    AllocationIdentifier identifier{};
};

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
