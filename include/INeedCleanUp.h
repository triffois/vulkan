#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "commonstructs.h"

class INeedCleanUp {
  public:
    virtual void cleanUp(const AppContext &context) = 0;
    virtual ~INeedCleanUp() = default;
};
