#pragma once

#include "RenderBatch.h"
#include <cstring>
#include <vector>

class Model {
  public:
    Model() = default;

    void addBatch(RenderBatch &&batch);

    // TODO:
    // Modle merge(const Model &other) const; (or addition)
    // void scale(float factor);
    // void translate(const glm::vec3 &offset);
    // void duplicate(const std::vector<glm::vec3> &positions);
    // void rotate(float angle, const glm::vec3 &axis);
    // void scatter(const std::vector<glm::vec3> &offsets);

    std::vector<RenderBatch> &getBatches() { return batches; }

  private:
    std::vector<RenderBatch> batches;
};
