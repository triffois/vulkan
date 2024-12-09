#pragma once

#include "Device.h"
#include "UniformAttachment.h"
#include "commonstructs.h"

class SceneLighting {
  public:
    SceneLighting(Device &deviceToUse, size_t attachmentBindingIndex);

    SceneLighting(const SceneLighting &other) = delete;

    SceneLighting(SceneLighting &&other) = delete;

    const SceneLighting &operator=(const SceneLighting &other) = delete;

    const SceneLighting &operator=(SceneLighting &&other) = delete;

    UniformAttachment<SimpleLightSource[3]> &getLightingBuffer();

  private:
    UniformAttachment<SimpleLightSource[3]> lightingAttachment;
};
