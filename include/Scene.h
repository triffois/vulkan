#pragma once

#include "RenderBatch.h"
#include "RenderPass.h"
#include <cstring>
#include <vector>

class Scene {
  public:
    Scene(GlobalResources *resources) : resources(resources) {}

    void addBatch(RenderBatch &&batch);

    // TODO:
    // Scene merge(const Scene &other) const;
    // void scale(float factor);
    // void translate(const glm::vec3 &offset);
    // void duplicate(const std::vector<glm::vec3> &positions);
    // void rotate(float angle, const glm::vec3 &axis);
    // void scatter(const std::vector<glm::vec3> &offsets);

    // Called once before rendering starts - creates device-local instance
    // buffers
    void prepareForRendering();
    std::vector<RenderPass> &getRenderPasses() { return passes; }

  private:
    GlobalResources *resources;

    std::vector<RenderBatch> batches;
    std::vector<RenderPass> passes;
};
