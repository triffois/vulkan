#pragma once

#include "RenderBatch.h"
#include <cstring>
#include <vector>

class Model {
  public:
    Model() = default;

    void addBatch(RenderBatch &&batch);

    Model merge(Model &other) const;
    void scale(float factor);
    void translate(const glm::vec3 &offset);
    void scatter(const std::vector<glm::vec3> &offsets);

    std::vector<RenderBatch> &getBatches() { return batches; }

  private:
    std::vector<RenderBatch> batches;
};
