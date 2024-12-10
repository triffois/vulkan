#include "DescriptorLayout.h"
#include "Pipeline.h"
#include "PipelineManager.h"
#include <stdexcept>

void PipelineManager::init(Device *device) { this->device = device; }

void PipelineManager::cleanup() {
    for (auto &pipeline : pipelines) {
        pipeline.second->cleanup();
    }
    pipelines.clear();
}

PipelineID PipelineManager::createPipeline(DescriptorLayout descriptorLayout,
                                           VkFormat colorFormat,
                                           VkFormat depthFormat,
                                           uint32_t maxFramesInFlight,
                                           PipelineSettings &settings) {
    if (!device) {
        throw std::runtime_error(
            "PipelineManager not initialized with device!");
    }

    // Create a unique ID from the shader paths
    PipelineID id =
        settings.getVertexShaderPath() + ":" + settings.getFragmentShaderPath();

    // Check if pipeline already exists
    if (pipelines.find(id) != pipelines.end()) {
        return id;
    }

    // Create new pipeline and move it into the map
    pipelines.emplace(id, std::make_unique<Pipeline>(
                              device, std::move(descriptorLayout), colorFormat,
                              depthFormat, maxFramesInFlight, settings));

    return id;
}

Pipeline &PipelineManager::getPipeline(const PipelineID &id) const {
    auto it = pipelines.find(id);
    if (it == pipelines.end()) {
        throw std::runtime_error("Pipeline not found!");
    }
    return *it->second;
}
