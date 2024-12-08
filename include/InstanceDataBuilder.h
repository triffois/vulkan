#pragma once

#include "Instance.h"
#include "InstanceData.h"
#include <vector>

// Converts high-level Instance objects into GPU-ready InstanceData
std::vector<InstanceData>
buildInstanceData(const std::vector<Instance> &instances);
