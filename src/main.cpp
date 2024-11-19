#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Engine.h"
#include "Model.h"
#include "ModelLoader.h"

int main() {
    Engine engine;
    std::vector<Model> models = ModelLoader::loadFromGLTF("assets/dingus.glb");

    // Create pipeline(s) outside the engine
    std::vector<Pipeline> pipelines;
    for (const auto &model : models) {
        pipelines.push_back(engine.createPipeline("shaders/vert.spv",
                                                  "shaders/frag.spv", model));
    }

    try {
        while (engine.running()) {
            auto render = engine.startRender();

            for (const auto &pipeline : pipelines) {
                render.submit(pipeline);
            }

            engine.finishRender(render);
        }

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
