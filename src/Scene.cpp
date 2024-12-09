#include "Scene.h"

void Scene::addBatch(RenderBatch &&batch) {
    batches.push_back(std::move(batch));
}

void Scene::prepareForRendering(Camera &camera) {
    // TODO: move things like this into the resources class
    auto maxFramesInFlight = resources->getDevice()->getMaxFramesInFlight();
    for (auto &batch : batches) {
        passes.emplace_back(resources, batch, maxFramesInFlight, camera);
    }
}
