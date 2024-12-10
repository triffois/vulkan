#include "Renderable.h"
#include <algorithm>

Renderable::Renderable(GlobalResources *resources, Model &model,
                       PipelineSettings &settings)
    : resources(resources) {
    auto device = resources->getDevice();
    DescriptorLayout descriptorLayout(*device->getDevice(),
                                      settings.getAttachments());

    // TODO: move things like this into the resources class
    auto maxFramesInFlight = resources->getDevice()->getMaxFramesInFlight();

    VkFormat colorFormat = resources->getSwapChain().getImageFormat();
    VkFormat depthFormat = resources->getSwapChain().findDepthFormat();

    auto pipelineId = resources->getPipelineManager().createPipeline(
        descriptorLayout, colorFormat, depthFormat, maxFramesInFlight,
        settings);

    for (auto &batch : model.getBatches()) {
        passes.emplace_back(resources, batch, pipelineId, maxFramesInFlight);
    }
}

Renderable::~Renderable() {
    std::for_each(passes.begin(), passes.end(),
                  [](auto &pass) { pass.cleanUp(); });
}
