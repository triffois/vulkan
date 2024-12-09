#pragma once

#include "UniformAttachment.h"
#include "commonstructs.h"
#include "Device.h"

#include <vector>
#include <array>

class SceneLighting
{
public:
    SceneLighting(Device & deviceToUse);

    SceneLighting (const SceneLighting &other) = delete;
    SceneLighting (SceneLighting &&other) = delete;

    const SceneLighting &operator=(const SceneLighting &other) = delete;
    const SceneLighting &operator=(SceneLighting &&other) = delete;

    UniformAttachment<SimpleLightSource[3]> &getLightingBuffer();

private:
    UniformAttachment<SimpleLightSource[3]> lightingAttachment;
};
