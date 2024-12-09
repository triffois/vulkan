#include "GlobalResources.h"

void GlobalResources::init(Device *device, AppWindow *appWindow) {
    this->device = device;

    SwapChainSupportDetails swapChainSupport =
        device->querySwapChainSupportCurrent();
    swapChain = std::make_unique<SwapChain>(device, appWindow);

    pipelineManager.init(device);
    meshManager.init(device);
    textureManager.init(device);
}

void GlobalResources::cleanup() {
    meshManager.cleanup();
    pipelineManager.cleanup();

    swapChain->cleanup();
}

void GlobalResources::prepareResources() {
    VkFormat colorFormat = swapChain->getImageFormat();
    VkFormat depthFormat = swapChain->findDepthFormat();

    pipelineManager.prepareResources(colorFormat, depthFormat);
}
