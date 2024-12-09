#include "UniformAttachment.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Engine.h"
#include "ModelLoader.h"
#include "Scene.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_gltf_file>"
                  << std::endl;
        return EXIT_FAILURE;
    }

    try {
        Engine engine;

        UniformAttachment uniformAttachment(&engine.getGlobalResources(),
                                            *engine.getCamera(), 0);
        uniformAttachment.init(engine.getDevice()->getMaxFramesInFlight());
        TextureAttachment &textureAttachment = engine.getGlobalResources()
                                                   .getTextureManager()
                                                   .getTextureAttachment();

        std::vector<std::reference_wrapper<IAttachment>> attachments;
        attachments.push_back(uniformAttachment);
        attachments.push_back(textureAttachment);

        // Load the model into a scene
        PipelineID pipelineID = engine.createPipeline(
            "shaders/vert.spv", "shaders/frag.spv", std::move(attachments));
        Scene scene = ModelLoader::loadFromGLTF(
            argv[1], engine.getGlobalResources(), pipelineID);

        // Prepare scene resources before rendering
        engine.prepareResources();
        scene.prepareForRendering();

        // Main render loop
        while (engine.running()) {
            auto render = engine.startRender();
            render.submit(scene);
            engine.finishRender(render);
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
