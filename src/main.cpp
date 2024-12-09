#include "TextureManager.h"
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

        TextureManager textures(engine.getDevice());

        // Load the model into a scene
        auto model = ModelLoader::loadFromGLTF(
            argv[1], engine.getGlobalResources(), textures);

        // Create the uniform attachment with a lambda for updates
        auto uniformUpdator = [&engine, &textures](UniformBufferObject &ubo) {
            auto camera = engine.getCamera();
            auto &swapChain = engine.getGlobalResources().getSwapChain();
            VkExtent2D swapChainExtent = swapChain.getExtent();

            ubo.view = camera->GetViewMatrix();
            ubo.proj = glm::perspective(glm::radians(camera->getZoom()),
                                        swapChainExtent.width /
                                            (float)swapChainExtent.height,
                                        0.1f, 1000.0f);
            ubo.proj[1][1] *= -1;

            // Get texture resolutions and fill the array
            auto resolutions = textures.getTextureResolutions();

            // Fill remaining slots with zero
            for (size_t i = resolutions.size(); i < 256; i++) {
                ubo.textureResolutions[i] = glm::vec4(0.0f);
            }
            for (size_t i = 0; i < resolutions.size(); i++) {
                ubo.textureResolutions[i] = resolutions[i];
            }
        };

        UniformAttachment<UniformBufferObject> uniformAttachment(
            engine.getDevice(), uniformUpdator, 0);

        TextureAttachment textureAttachment = textures.getTextureAttachment(1);

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
