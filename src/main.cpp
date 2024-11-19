#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Engine.h"
#include "Model.h"
#include "ModelLoader.h"

int main() {
    Engine app;
    std::cout << "Engine created" << std::endl;

    std::vector<Model> models = ModelLoader::loadFromGLTF("assets/dingus.glb");
    std::cout << "Loaded " << models.size() << " models" << std::endl;

    for (const auto &model : models) {
        app.addPipeline("shaders/vert.spv", "shaders/frag.spv", model);
    }
    std::cout << "Pipeline added" << std::endl;

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
