#include "ControlSystem.h"
#include "PipelineSettings.h"
#include "SceneLighting.h"
#include "TextureManager.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "ControlSystem.h"
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
        ControlSystem controlSystem(engine);
        TextureManager textures(engine.getDevice());

        // Load the model into a scene
        auto model = ModelLoader::loadFromGLTF(
            argv[1], engine.getGlobalResources(), textures);

        // std::vector<glm::vec3> offsets;
        // for (int i = -500; i < 500; i += 100) {
        //     for (int j = -500; j < 500; j += 100) {
        //         offsets.push_back(glm::vec3{i, 0, j});
        //     }
        // }
        // model.scatter(offsets);
        // Create the uniform attachment with a lambda for updates

        auto uniformAttachment = controlSystem.getUniformAttachment(0);

        auto resolutionsAttachment = textures.getResolutionsAttachment<256>(1);
        auto textureAttachment = textures.getTextureAttachment(2);

        SceneLighting staticLighting{*engine.getDevice(), 3};

        PipelineSettings shading("shaders/vert.spv", "shaders/frag.spv");
        shading.bind(uniformAttachment);
        shading.bind(resolutionsAttachment);
        shading.bind(textureAttachment);
        shading.bind(staticLighting.getLightingBuffer());

        auto renderable = engine.shaded(model, shading);

        // Main render loop
        while (engine.running()) {
            controlSystem.update();
            auto render = engine.startRender();
            render.submit(renderable);
            engine.finishRender(render);
        }

        engine.initializeEngineTeardown();
        // it is needed here to do some initial clean-up before destructors of
        // objects created above kick in
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
