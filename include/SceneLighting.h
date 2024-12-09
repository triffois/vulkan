#pragma once

#include "Device.h"
#include "UniformAttachment.h"
#include "commonstructs.h"

#define NUM_FRAG_SHADER_LIGHTS 3

struct LightsArray {
    SimpleLightSource lights[NUM_FRAG_SHADER_LIGHTS];
};

class SceneLighting {
  public:
    SceneLighting(Device &deviceToUse, size_t attachmentBindingIndex);

    SceneLighting(const SceneLighting &other) = delete;

    SceneLighting(SceneLighting &&other) = delete;

    const SceneLighting &operator=(const SceneLighting &other) = delete;

    const SceneLighting &operator=(SceneLighting &&other) = delete;

    UniformAttachment<LightsArray> &getLightingBuffer();

  private:
    UniformAttachment<LightsArray> lightingAttachment;
};
