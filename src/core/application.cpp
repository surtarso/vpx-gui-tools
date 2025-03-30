#include "core/application.h"
#include <imgui.h>
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <iostream>
#include <filesystem>

Application::Application(const std::string& basePath)
    : basePath(basePath),
      config(basePath),
      tableManager(config),
      iniEditor(config.getVPinballXIni(), false),
      configEditor(basePath + "resources/settings.ini", true),
      launcher(config, &tableManager) {}

Application::~Application() {}

void Application::run() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }

    window = SDL_CreateWindow("VPX GUI Tools", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              config.getWindowWidth(), config.getWindowHeight(), SDL_WINDOW_RESIZABLE);
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

    static const ImWchar glyphRanges[] = {
        0x0020, 0x007F, 0x2600, 0x26FF, 0x25A0, 0x25FF, 0x2700, 0x27BF, 0, 0
    };

    std::string symbolaPath = basePath + "resources/Symbola.ttf";
    ImFont* emojiFont = nullptr;
    if (std::filesystem::exists(symbolaPath)) {
        emojiFont = io.Fonts->AddFontFromFileTTF(symbolaPath.c_str(), 14.0f, nullptr, glyphRanges);
    }
    const char* fallbackFontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
    if (!emojiFont && std::filesystem::exists(fallbackFontPath)) {
        io.Fonts->AddFontFromFileTTF(fallbackFontPath, 16.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
    }
    if (io.Fonts->Fonts.empty()) {
        io.Fonts->AddFontDefault();
    }
    io.Fonts->Build();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    std::string lastIniPath = config.getVPinballXIni();

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
            std::string currentIniPath = launcher.getSelectedIniPath();
            if (currentIniPath != lastIniPath) {
                iniEditor.loadIniFile(currentIniPath);
                lastIniPath = currentIniPath;
            }
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
            tableManager.filterTables(launcher.getSearchQuery());
            launcher.draw(tableManager.getTables(), editingIni, editingSettings, exitRequested, showCreateIniPrompt, showNoTablePopup);
        }

        if (showCreateIniPrompt) {
            ImGui::OpenPopup("Create INI File?");
            if (ImGui::BeginPopupModal("Create INI File?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("No INI file found for this table.\nWould you like to create one?");
                if (ImGui::Button("Yes")) {
                    std::string newIniPath = launcher.getSelectedIniPath();
                    std::filesystem::copy(config.getVPinballXIni(), newIniPath, std::filesystem::copy_options::skip_existing);
                    iniEditor.loadIniFile(newIniPath);
                    lastIniPath = newIniPath;
                    editingIni = true;
                    showCreateIniPrompt = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("No")) {
                    showCreateIniPrompt = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        if (showNoTablePopup) {
            ImGui::OpenPopup("No Table Selected");
            if (ImGui::BeginPopupModal("No Table Selected", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("No table selected.\nPlease select a table first.");
                if (ImGui::Button("OK")) {
                    showNoTablePopup = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
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