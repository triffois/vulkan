#include "Model.h"
#include <algorithm>

void Model::addBatch(RenderBatch &&batch) {
    batches.push_back(std::move(batch));
}

Model Model::merge(Model &other) const {
    Model result = *this;
    for (const auto &batch : other.getBatches()) {
        // First, try to find an existing batch with the same mesh ID
        auto existingBatch =
            std::find_if(result.batches.begin(), result.batches.end(),
                         [&batch](const RenderBatch &b) {
                             return b.meshId == batch.meshId;
                         });
        if (existingBatch != result.batches.end()) {
            // If we found an existing batch, append the instances to it
            existingBatch->instances.insert(existingBatch->instances.end(),
                                            batch.instances.begin(),
                                            batch.instances.end());
            continue;
        }
        // If we didn't find an existing batch, create a new one
        else
            result.addBatch(RenderBatch{batch.meshId, batch.instances});
    }
    return result;
}

void Model::scale(float factor) {
    for (auto &batch : batches) {
        for (auto &instance : batch.instances) {
            instance.position *= factor;
            instance.scale *= factor;
        }
    }
}

void Model::translate(const glm::vec3 &offset) {
    for (auto &batch : batches) {
        for (auto &instance : batch.instances) {
            instance.position += offset;
        }
    }
}

void Model::scatter(const std::vector<glm::vec3> &offsets) {
    // Replace the instances with offsets * instances copies
    std::vector<RenderBatch> newBatches;
    for (const auto &offset : offsets) {
        for (const auto &batch : batches) {
            std::vector<Instance> newInstances;
            for (const auto &instance : batch.instances) {
                newInstances.push_back(instance);
                newInstances.back().position += offset;
            }
            newBatches.push_back(RenderBatch{batch.meshId, newInstances});
        }
    }
    batches = newBatches;
}
