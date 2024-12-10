#include "GlobalResources.h"

GlobalResources::~GlobalResources() {
    meshManager.cleanup();
    pipelineManager.cleanup();
    swapChain->cleanup();
}

void GlobalResources::init(Device *device, AppWindow *appWindow) {
    this->device = device;

    SwapChainSupportDetails swapChainSupport =
        device->querySwapChainSupportCurrent();
    swapChain = std::make_unique<SwapChain>(device, appWindow);

    pipelineManager.init(device);
    meshManager.init(device);
}
