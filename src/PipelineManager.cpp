#include "PipelineManager.h"
#include "Pipeline.h"
#include <functional>
#include <stdexcept>

void PipelineManager::init(Device *device) { this->device = device; }

void PipelineManager::cleanup() { pipelines.clear(); }

PipelineID PipelineManager::createPipeline(
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
    pipelines.emplace(id, std::make_unique<Pipeline>(vertPath, fragPath,
                                                     std::move(attachments)));

    return id;
}

void PipelineManager::prepareResources(VkFormat colorFormat,
                                       VkFormat depthFormat) {
    for (auto &pair : pipelines) {
        pair.second->init(device, colorFormat, depthFormat,
                          device->getMaxFramesInFlight());
    }
}

Pipeline &PipelineManager::getPipeline(const PipelineID &id) const {
    auto it = pipelines.find(id);
    if (it == pipelines.end()) {
        throw std::runtime_error("Pipeline not found!");
    }
    return *it->second;
}
