#include "GlobalResources.h"

void GlobalResources::init(Device *device) {
    this->device = device;
    pipelineManager.init(device);
    meshManager.init(device);
    textureManager.init(device);
}

void GlobalResources::cleanup() {
    textureManager.cleanup();
    meshManager.cleanup();
    pipelineManager.cleanup();
}

void GlobalResources::prepareResources(VkFormat colorFormat,
                                       VkFormat depthFormat) {
    textureManager.prepareResources();
    pipelineManager.prepareResources(colorFormat, depthFormat);
}
