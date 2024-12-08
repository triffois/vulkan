#include "InstanceDataBuilder.h"

std::vector<InstanceData>
buildInstanceData(const std::vector<Instance> &instances) {
    std::vector<InstanceData> instanceData;
    instanceData.reserve(instances.size());

    for (const auto &instance : instances) {
        InstanceData data;

        // Get transform matrix
        data.transform = instance.getTransformMatrix();

        // Pack texture indices into vec4
        data.textureIndices = glm::vec4(
            instance.material.textureIds[0], instance.material.textureIds[1],
            instance.material.textureIds[2], instance.material.textureIds[3]);

        instanceData.push_back(data);
    }

    return instanceData;
}
