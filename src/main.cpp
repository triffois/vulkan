#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Engine.h"
#include "Model.h"
#include "ModelLoader.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_gltf_file>"
                  << std::endl;
        return EXIT_FAILURE;
    }

    Engine engine;
    std::vector<Model> models = ModelLoader::loadFromGLTF(argv[1]);

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
