#include "Renderable.h"

Renderable::Renderable(GlobalResources *resources, Model &model,
                       std::string &vertPath, std::string &fragPath)
    : resources(resources) {
    auto device = resources->getDevice();
    DescriptorLayout descriptorLayout(*device->getDevice(),
                                      model.getAttachments());

    // TODO: move things like this into the resources class
    auto maxFramesInFlight = resources->getDevice()->getMaxFramesInFlight();

    VkFormat colorFormat = resources->getSwapChain().getImageFormat();
    VkFormat depthFormat = resources->getSwapChain().findDepthFormat();

    auto pipelineId = resources->getPipelineManager().createPipeline(
        descriptorLayout, colorFormat, depthFormat, maxFramesInFlight, vertPath,
        fragPath, model.getAttachments());

    for (auto &batch : model.getBatches()) {
        passes.emplace_back(resources, batch, pipelineId, maxFramesInFlight);
    }
}
