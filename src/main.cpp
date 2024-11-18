#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Engine.h"
#include "Model.h"
#include "ModelLoader.h"

int main() {
    Engine app;
    std::cout << "Engine created" << std::endl;

    Model model = ModelLoader::loadFromGLTF("assets/dingus.glb");
    app.addPipeline("shaders/vert.spv", "shaders/frag.spv", model);
    std::cout << "Pipeline added" << std::endl;

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
