#pragma once

#include "Model.h"
#include "PipelineSettings.h"
#include "RenderPass.h"
#include <cstring>
#include <vector>

class Renderable {
  public:
    Renderable(GlobalResources *resources, Model &model,
               PipelineSettings &settings);

    // TODO: potentially merging, for convenience - not everything can be
    // merged as a model, and having different renderables for different
    // materials is a bit crazy :3

    std::vector<RenderPass> &getRenderPasses() { return passes; }

  private:
    GlobalResources *resources;

    std::vector<RenderPass> passes;
};
