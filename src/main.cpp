#include "Engine.h"

int main() {
    Engine app;

    app.addPipeline("shaders/vert.spv", "shaders/frag.spv");

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
