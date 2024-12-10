#pragma once

#include "Device.h"
#include "MeshManager.h"
#include "PipelineManager.h"
#include "SwapChain.h"

class GlobalResources {
  public:
    GlobalResources() = default;

    ~GlobalResources();

    void init(Device *device, AppWindow *appWindow);

    SwapChain &getSwapChain() { return *swapChain; }
    PipelineManager &getPipelineManager() { return pipelineManager; }
    MeshManager &getMeshManager() { return meshManager; }
    Device *getDevice() { return device; }

  private:
    Device *device = nullptr;
    PipelineManager pipelineManager;
    MeshManager meshManager;
    std::unique_ptr<SwapChain> swapChain;
};
