#pragma once

#include "Instance.h"
#include "MeshManager.h"
#include "PipelineManager.h"
#include <vector>

struct RenderBatch {
    MeshID meshId;
    PipelineID pipelineId;
    std::vector<Instance> instances;
};
