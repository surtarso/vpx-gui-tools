#include "core/application.h"
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <iostream>
#include <filesystem>

Application::Application(const std::string& basePath)
    : basePath(basePath),
      config(basePath),
      tableManager(config),
      iniEditor(config.getVPinballXIni(), false),
      configEditor(basePath + "resources/settings.ini", true),
      launcher(config, &tableManager),
      firstRunDialog(config),
      showFirstRunDialog(false),
      deferInitialLoad(false),
      loadingTables(false),
      loadingComplete(false),
      editingIni(false),
      editingSettings(false),
      exitRequested(false),
      showCreateIniPrompt(false),
      showNoTablePopup(false),
      window(nullptr),
      renderer(nullptr) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        LOG_DEBUG("SDL_Init Error: " << SDL_GetError());
        throw std::runtime_error("Failed to initialize SDL");
    }

    window = SDL_CreateWindow("VPX GUI Tools", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              config.getWindowWidth(), config.getWindowHeight(), SDL_WINDOW_RESIZABLE);
    if (!window) {
        LOG_DEBUG("SDL_CreateWindow Error: " << SDL_GetError());
        SDL_Quit();
        throw std::runtime_error("Failed to create SDL window");
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        LOG_DEBUG("SDL_CreateRenderer Error: " << SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("Failed to create SDL renderer");
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    // Get ImGuiConf path from config and prepend base path
    imguiIniPath = config.getImGuiConf();
    
    // Ensure the directory exists (e.g., resources/)
    std::string resourcesDir = config.getBasePath() + "resources";
    if (!std::filesystem::exists(resourcesDir)) {
        std::filesystem::create_directory(resourcesDir);
    }
    
    io.IniFilename = imguiIniPath.c_str();
    LOG_DEBUG("INI file path: " << io.IniFilename);

    static const ImWchar glyphRanges[] = {
        0x0020, 0x007F, 0x2600, 0x26FF, 0x25A0, 0x25FF, 0x2700, 0x27BF, 0, 0
    };

    std::string symbolaPath = basePath + "resources/Symbola.ttf";
    ImFont* emojiFont = nullptr;
    if (std::filesystem::exists(symbolaPath)) {
        emojiFont = io.Fonts->AddFontFromFileTTF(symbolaPath.c_str(), 16.0f, nullptr, glyphRanges);
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

    // Check if we need to show the first-run dialog
    if (config.isFirstRun() || !config.arePathsValid()) {
        showFirstRunDialog = true;
        deferInitialLoad = true; // Defer initial table loading until after the first-run dialog
    } else {
        loadTables(); // Load tables immediately if no first-run dialog is needed
    }
}

Application::~Application() {
    if (loadingThread.joinable()) {
        loadingThread.join(); // Ensure the loading thread is finished before destruction
    }
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

void Application::loadTables() {
    loadingTables = true;
    loadingComplete = false;
    // Start the loading process in a separate thread
    loadingThread = std::thread([this]() {
        tableManager.loadTables();
        LOG_DEBUG("Loaded " << tableManager.getTables().size() << " tables.");
        loadingComplete = true;
    });
}

void Application::drawLoadingScreen() {
    // Calculate the position to center the text
    const char* loadingText = "Indexing tables...";
    ImVec2 textSize = ImGui::CalcTextSize(loadingText);
    ImVec2 windowSize = ImGui::GetIO().DisplaySize;
    ImVec2 textPos = ImVec2((windowSize.x - textSize.x) * 0.5f, (windowSize.y - textSize.y) * 0.5f);

    // Draw the text directly without a window
    ImGui::SetNextWindowPos(textPos, ImGuiCond_Always);
    ImGui::Begin("Loading", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
    ImGui::Text("%s", loadingText);
    ImGui::End();
}

void Application::run() {
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

        if (showFirstRunDialog) {
            showFirstRunDialog = firstRunDialog.show();
            if (!showFirstRunDialog) {
                // Dialog closed, check if paths are valid before proceeding
                if (config.arePathsValid()) {
                    loadTables(); // Start the table loading process
                } else {
                    exitRequested = true; // Exit if the user didn't set valid paths
                }
            }
        }
        else if (loadingTables) {
            drawLoadingScreen(); // Show the loading screen while tables are being loaded
            if (loadingComplete) {
                loadingTables = false; // Loading is done, move to the main interface
                if (loadingThread.joinable()) {
                    loadingThread.join(); // Clean up the thread
                }
            }
        }
        else if (editingIni) {
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
}