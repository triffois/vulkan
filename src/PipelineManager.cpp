#include "PipelineManager.h"
#include "DescriptorLayout.h"
#include "Pipeline.h"
#include <stdexcept>

void PipelineManager::init(Device *device) { this->device = device; }

void PipelineManager::cleanup() {
    for (auto &pipeline : pipelines) {
        pipeline.second->cleanup();
    }
    pipelines.clear();
}

PipelineID PipelineManager::createPipeline(
    DescriptorLayout descriptorLayout, VkFormat colorFormat,
    VkFormat depthFormat, uint32_t maxFramesInFlight,
    const std::string &vertPath, const std::string &fragPath,
    std::vector<std::reference_wrapper<IAttachment>> attachments) {
    if (!device) {
        throw std::runtime_error(
            "PipelineManager not initialized with device!");
    }

    // Create a unique ID from the shader paths
    PipelineID id = vertPath + ":" + fragPath;

    // Check if pipeline already exists
    if (pipelines.find(id) != pipelines.end()) {
        return id;
    }

    // Create new pipeline and move it into the map
    pipelines.emplace(id, std::make_unique<Pipeline>(
                              device, std::move(descriptorLayout), colorFormat,
                              depthFormat, maxFramesInFlight, vertPath,
                              fragPath, attachments));

    return id;
}

Pipeline &PipelineManager::getPipeline(const PipelineID &id) const {
    auto it = pipelines.find(id);
    if (it == pipelines.end()) {
        throw std::runtime_error("Pipeline not found!");
    }
    return *it->second;
}
