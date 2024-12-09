#pragma once

#include "IAttachment.h"
#include "RenderBatch.h"
#include <cstring>
#include <vector>

class Model {
  public:
    Model() = default;

    // TODO: think of somehow tagging the materials before the conversion to a
    // RenderBatch happens - for the purposes of cluttering the callsite less
    // Maybe call that abstraction "Scene", and make it a container for models
    // with one model per material
    void addBatch(RenderBatch &&batch);
    void bind(IAttachment &attachment) { attachments.push_back(attachment); }

    // TODO:
    // Modle merge(const Model &other) const; (or addition)
    // void scale(float factor);
    // void translate(const glm::vec3 &offset);
    // void duplicate(const std::vector<glm::vec3> &positions);
    // void rotate(float angle, const glm::vec3 &axis);
    // void scatter(const std::vector<glm::vec3> &offsets);

    std::vector<RenderBatch> &getBatches() { return batches; }
    std::vector<std::reference_wrapper<IAttachment>> &getAttachments() {
        return attachments;
    }

  private:
    std::vector<RenderBatch> batches;
    std::vector<std::reference_wrapper<IAttachment>> attachments;
};
