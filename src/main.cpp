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

        // Load the model into a scene
        PipelineID pipelineID =
            engine.createPipeline("shaders/vert.spv", "shaders/frag.spv");
        Scene scene = ModelLoader::loadFromGLTF(
            argv[1], engine.getGlobalResources(), pipelineID);

        // Bind all the resources
        UniformAttachment uniformAttachment(&engine.getGlobalResources(),
                                            *engine.getCamera(), 0);
        uniformAttachment.init(engine.getDevice()->getMaxFramesInFlight());

        TextureAttachment textureAttachment = engine.getGlobalResources()
                                                  .getTextureManager()
                                                  .getTextureAttachment();

        engine.bind(pipelineID, uniformAttachment);
        engine.bind(pipelineID, textureAttachment);

        // Prepare scene resources before rendering
        // TODO: consider initing pipelines here more explicitly
        // along the lines of modelLoader returning a PipelineID, and here being
        // engine.setMaterial(pipelineID, "shaders/vert.spv",
        // "shaders/frag.spv", {uniformAttachment, textureAttachment});
        engine.prepareResources();

        // TODO: consider returning RenderPasses or Renderables or whatever here
        // Since the scene still separates rendering by pipeline (obviously),
        // bonus points: combine with the previous step into something along the
        // lines of Renderable renderable = scene.using("shaders/frag.spv",
        // "shaders/vert.spv", {uniformAttachment, textureAttachment});
        // (and then potentially rename Scene to Model - we going in circles
        // yippee)
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
