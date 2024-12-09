#pragma once

#include "Device.h"
#include "MeshManager.h"
#include "PipelineManager.h"
#include "SwapChain.h"

class GlobalResources {
  public:
    GlobalResources() = default;
    ~GlobalResources() = default;

    void init(Device *device, AppWindow *appWindow);

    void cleanup();

    SwapChain &getSwapChain() { return *swapChain; }
    PipelineManager &getPipelineManager() { return pipelineManager; }
    MeshManager &getMeshManager() { return meshManager; }
    Device *getDevice() { return device; }

  private:
    Device *device = nullptr;
    std::unique_ptr<SwapChain> swapChain;
    PipelineManager pipelineManager;
    MeshManager meshManager;
};
