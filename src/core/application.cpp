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
      renderer(nullptr),
      dpiScale(1.0f) {
    // Enable DPI awareness
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); // Linear filtering for sharpness

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        LOG_DEBUG("SDL_Init Error: " << SDL_GetError());
        throw std::runtime_error("Failed to initialize SDL");
    }

    // Create window with high-DPI support
    window = SDL_CreateWindow("VPX GUI Tools", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              config.getWindowWidth(), config.getWindowHeight(),
                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
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

    // Get DPI scaling factor
    float dpi = 96.0f;
    int displayIndex = SDL_GetWindowDisplayIndex(window);
    if (SDL_GetDisplayDPI(displayIndex, &dpi, nullptr, nullptr) != 0) {
        std::cerr << "Failed to get display DPI: " << SDL_GetError() << std::endl;
        dpiScale = 1.0f;
    } else {
        dpiScale = dpi / 96.0f;
        if (dpiScale <= 0.0f) dpiScale = 1.0f;
    }
    LOG_DEBUG("DPI Scale: " << dpiScale);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Scale ImGui globally
    io.FontGlobalScale = dpiScale;
    float baseFontSize = 16.0f;
    float scaledFontSize = baseFontSize * dpiScale;

    static const ImWchar glyphRanges[] = {
        0x0020, 0x007F, 0x2600, 0x26FF, 0x25A0, 0x25FF, 0x2700, 0x27BF, 0, 0
    };

    std::string symbolaPath = basePath + "resources/Symbola.ttf";
    ImFont* emojiFont = nullptr;
    if (std::filesystem::exists(symbolaPath)) {
        emojiFont = io.Fonts->AddFontFromFileTTF(symbolaPath.c_str(), scaledFontSize, nullptr, glyphRanges);
    }
    const char* fallbackFontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
    if (!emojiFont && std::filesystem::exists(fallbackFontPath)) {
        io.Fonts->AddFontFromFileTTF(fallbackFontPath, scaledFontSize, nullptr, io.Fonts->GetGlyphRangesDefault());
    }
    if (io.Fonts->Fonts.empty()) {
        io.Fonts->AddFontDefault();
    }
    io.Fonts->Build();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(dpiScale);

    imguiIniPath = config.getImGuiConf();
    std::string resourcesDir = config.getBasePath() + "resources";
    if (!std::filesystem::exists(resourcesDir)) {
        std::filesystem::create_directory(resourcesDir);
    }
    io.IniFilename = imguiIniPath.c_str();
    LOG_DEBUG("INI file path: " << io.IniFilename);

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    if (config.isFirstRun() || !config.arePathsValid()) {
        showFirstRunDialog = true;
        deferInitialLoad = true;
    } else {
        loadTables();
    }
}

Application::~Application() {
    if (loadingThread.joinable()) {
        loadingThread.join();
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
    loadingThread = std::thread([this]() {
        tableManager.loadTables();
        LOG_DEBUG("Loaded " << tableManager.getTables().size() << " tables.");
        loadingComplete = true;
    });
}

void Application::drawLoadingScreen() {
    ImVec2 textSize = ImGui::CalcTextSize("Indexing tables...");
    ImVec2 windowSize = ImGui::GetIO().DisplaySize;
    ImVec2 textPos = ImVec2((windowSize.x - textSize.x) * 0.5f, (windowSize.y - textSize.y) * 0.5f);

    ImGui::SetNextWindowPos(textPos, ImGuiCond_Always);
    ImGui::Begin("Loading", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
    ImGui::Text("Indexing tables...");
    ImGui::End();
}

void Application::run() {
    std::string lastIniPath = config.getVPinballXIni();

    while (!exitRequested) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) exitRequested = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                // Update renderer viewport on resize
                SDL_RenderSetViewport(renderer, nullptr);
            }
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (showFirstRunDialog) {
            showFirstRunDialog = firstRunDialog.show();
            if (!showFirstRunDialog && config.arePathsValid()) {
                loadTables();
            } else if (!showFirstRunDialog) {
                exitRequested = true;
            }
        } else if (loadingTables) {
            drawLoadingScreen();
            if (loadingComplete) {
                loadingTables = false;
                if (loadingThread.joinable()) {
                    loadingThread.join();
                }
            }
        } else if (editingIni) {
            std::string currentIniPath = launcher.getSelectedIniPath();
            if (currentIniPath != lastIniPath) {
                iniEditor.loadIniFile(currentIniPath);
                lastIniPath = currentIniPath;
            }
            iniEditor.draw(editingIni);
            if (ImGui::IsKeyPressed(ImGuiKey_Escape)) editingIni = false;
        } else if (editingSettings) {
            configEditor.draw(editingSettings);
            if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                editingSettings = false;
                config.loadSettings();
            }
        } else {
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