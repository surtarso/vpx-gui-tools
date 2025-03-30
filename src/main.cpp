#include "core/application.h"
#include "utils/logging.h"
#include <SDL.h>
#include <filesystem>
#include <iostream>

int main() {
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