#pragma once

#include "Instance.h"
#include "MeshManager.h"
#include <vector>

struct RenderBatch {
    MeshID meshId;
    std::vector<Instance> instances;
};
