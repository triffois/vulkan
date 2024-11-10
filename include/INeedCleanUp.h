#pragma once

#include <vulkan/vulkan.h>

struct CleanUpContex{
    VkInstance *appInstance;
    VkDevice *appDevice;
};

class INeedCleanUp {
public:
    virtual void cleanUp(const CleanUpContex &context) = 0;
    virtual ~INeedCleanUp() = default;
};
