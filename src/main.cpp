#include "UniformAttachment.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Engine.h"
#include "ModelLoader.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_gltf_file>"
                  << std::endl;
        return EXIT_FAILURE;
    }

    try {
        Engine engine;

        // Load the model into a scene
        auto model =
            ModelLoader::loadFromGLTF(argv[1], engine.getGlobalResources());

        // Bind all the resources
        UniformAttachment uniformAttachment(&engine.getGlobalResources(),
                                            *engine.getCamera(), 0);

        // TODO nonglobal texture managers - this one is certainly possible
        TextureAttachment textureAttachment = engine.getGlobalResources()
                                                  .getTextureManager()
                                                  .getTextureAttachment();

        model.bind(uniformAttachment);
        model.bind(textureAttachment);

        auto renderable =
            engine.shaded(model, "shaders/vert.spv", "shaders/frag.spv");

        // Main render loop
        while (engine.running()) {
            auto render = engine.startRender();
            render.submit(renderable);
            engine.finishRender(render);
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
