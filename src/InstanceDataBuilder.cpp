#include "InstanceDataBuilder.h"

std::vector<InstanceData>
buildInstanceData(const std::vector<Instance> &instances) {
    std::vector<InstanceData> instanceData;
    instanceData.reserve(instances.size());

    for (const auto &instance : instances) {
        InstanceData data;

        // Get transform matrix
        data.transform = instance.getTransformMatrix();

        // Copy texture indices directly from material
        data.textureIndices = instance.material.textureIds;

        instanceData.push_back(data);
    }

    return instanceData;
}
