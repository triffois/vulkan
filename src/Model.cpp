#include "Model.h"

void Model::addBatch(RenderBatch &&batch) {
    batches.push_back(std::move(batch));
}
