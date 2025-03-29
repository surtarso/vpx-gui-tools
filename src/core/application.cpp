#include "core/application.h"
#include <imgui.h>
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <iostream>
#include <filesystem>

Application::Application()
    : tableManager(config.tablesDir, config.romPath, config.altSoundPath, config.altColorPath, config.musicPath, config.pupPackPath,
                   config.wheelImage, config.tableImage, config.backglassImage, config.marqueeImage, config.tableVideo,
                   config.backglassVideo, config.dmdVideo),
      iniEditor(config.vpinballXIni, false),
      configEditor("resources/settings.ini", true),
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
    ImGuiIO& io = ImGui::GetIO();

    // Define glyph ranges for Unicode symbols (16-bit, within 0xFFFF)
    static const ImWchar glyphRanges[] = {
        0x0020, 0x007F,  // Basic Latin
        0x2600, 0x26FF,  // Miscellaneous Symbols (☀, ♫, ♪)
        0x25A0, 0x25FF,  // Geometric Shapes (■)
        0x2700, 0x27BF,  // Dingbats (✪)
        0, 0             // Terminator
    };

    // Load Symbola
    const char* symbolaPath = "resources/Symbola.ttf";
    ImFont* emojiFont = nullptr;
    if (std::filesystem::exists(symbolaPath)) {
        std::cout << "Found Symbola.ttf at " << symbolaPath << std::endl;
        emojiFont = io.Fonts->AddFontFromFileTTF(symbolaPath, 14.0f, nullptr, glyphRanges);
        if (emojiFont) {
            std::cout << "Successfully loaded Symbola.ttf with glyph ranges" << std::endl;
        } else {
            std::cerr << "Failed to load Symbola.ttf from " << symbolaPath << " (returned null)" << std::endl;
        }
    } else {
        std::cerr << "Symbola.ttf not found at " << symbolaPath << std::endl;
    }

    // Fallback to DejaVuSans
    const char* fallbackFontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
    if (!emojiFont && std::filesystem::exists(fallbackFontPath)) {
        std::cout << "Falling back to DejaVuSans.ttf at " << fallbackFontPath << std::endl;
        ImFont* fallbackFont = io.Fonts->AddFontFromFileTTF(fallbackFontPath, 16.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
        if (fallbackFont) {
            std::cout << "Successfully loaded DejaVuSans.ttf" << std::endl;
        } else {
            std::cerr << "Failed to load DejaVuSans.ttf from " << fallbackFontPath << " (returned null)" << std::endl;
        }
    }

    // Final fallback to default font
    if (io.Fonts->Fonts.empty()) {
        std::cout << "All custom font loading failed. Using ImGui default font." << std::endl;
        io.Fonts->AddFontDefault();
    }

    // Build font atlas
    bool fontAtlasBuilt = io.Fonts->Build();
    if (!fontAtlasBuilt) {
        std::cerr << "Failed to build font atlas. Clearing fonts and using default." << std::endl;
        io.Fonts->Clear();
        io.Fonts->AddFontDefault();
    }

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
            // Use the launcher's search query to filter tables
            tableManager.filterTables(launcher.getSearchQuery());
            launcher.draw(tableManager.getTables(), editingIni, editingSettings, exitRequested);
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