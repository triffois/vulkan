#include "McNavigationSystem.h"
#include "PipelineSettings.h"
#include "SceneLighting.h"
#include "TextureManager.h"

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

        McNavigationSystem controlSystem(engine);
        TextureManager textures(engine.getDevice());

        // Load the GLTF model
        auto model = ModelLoader::loadFromGLTF(
            argv[1], engine.getGlobalResources(), textures);
        model.scale(2);

        auto cubeModel = ModelLoader::loadFromGLTF(
            "assets/cube.glb", engine.getGlobalResources(), textures);
        cubeModel.scale(2);
        cubeModel.translate({2, 2.125, 0});

        auto uniformAttachment = controlSystem.getUniformAttachment(0);
        auto resolutionsAttachment = textures.getResolutionsAttachment<256>(1);
        auto textureAttachment = textures.getTextureAttachment(2);
        SceneLighting staticLighting{*engine.getDevice(), 3};

        PipelineSettings shading("shaders/vert.spv", "shaders/frag.spv");
        shading.bind(uniformAttachment);
        shading.bind(resolutionsAttachment);
        shading.bind(textureAttachment);
        shading.bind(staticLighting.getLightingBuffer());

        auto uniformAttachmentr = controlSystem.getUniformAttachment(0);
        SceneLighting staticLightingr{*engine.getDevice(), 1};

        PipelineSettings shadingr("shaders/raymarching_vert.spv",
                                  "shaders/raymarching_frag.spv");
        shadingr.bind(uniformAttachmentr);
        shadingr.bind(staticLightingr.getLightingBuffer());

        // Create two renderables
        auto gltfRenderable = engine.shaded(model, shading);
        auto cubeRenderable = engine.shaded(cubeModel, shadingr);

        // Main render loop
        while (engine.running()) {
            controlSystem.update();
            auto render = engine.startRender();
            render.submit(gltfRenderable);
            render.submit(cubeRenderable);
            engine.finishRender(render);
        }

        engine.initializeEngineTeardown();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
