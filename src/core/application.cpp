#include "core/application.h"
#include <imgui.h>
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <iostream>

Application::Application()
    : tableManager(config.tablesDir, config.romPath, config.altSoundPath, config.altColorPath, config.musicPath, config.pupPackPath,
                   config.wheelImage, config.tableImage, config.backglassImage, config.marqueeImage, config.tableVideo,
                   config.backglassVideo, config.dmdVideo),
                   iniEditor(config.vpinballXIni, false),  // VPinballX.ini editor
                   configEditor("resources/settings.ini", true),  // settings.ini editor
      launcher(config.tablesDir, config.startArgs, config.commandToRun, config.endArgs, config.vpinballXIni) {}

Application::~Application() {}

void Application::run() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }

    window = SDL_CreateWindow("VPX GUI Tools", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              config.windowWidth, config.windowHeight, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    while (!exitRequested) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) exitRequested = true;
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (editingIni) {
            iniEditor.draw(editingIni);
            if (ImGui::IsKeyPressed(ImGuiKey_Escape)) editingIni = false;
        }
        else if (editingSettings) {
            configEditor.draw(editingSettings);
            if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                editingSettings = false;
                config.loadSettings();
            }
        }
        else {
            tableManager.filterTables(launcher.getSelectedTable() >= 0 ? "" : "");
            launcher.draw(tableManager.getTables(), editingIni, editingSettings, exitRequested); // Pass exitRequested
        }

        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}