#include "core/application.h"
#include "utils/logging.h"
#include "version.h"
#include <SDL.h>
#include <filesystem>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string(argv[1]) == "--version") {
        std::cout << "VPXGUITools version " << PROJECT_VERSION << std::endl;
        return 0;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        LOG_DEBUG("SDL_Init Error: " << SDL_GetError());
        return 1;
    }
    char* basePathC = SDL_GetBasePath();
    std::string basePath = basePathC ? basePathC : std::filesystem::current_path().string();
    SDL_free(basePathC);
    SDL_Quit();
    if (!basePath.empty() && basePath.back() != '/' && basePath.back() != '\\') {
        basePath += "/";
    }
    Application app(basePath);
    app.run();
    return 0;
}