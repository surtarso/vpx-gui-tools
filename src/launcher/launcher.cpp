#include "launcher/launcher.h"
#include <filesystem>

Launcher::Launcher(IConfigProvider& config, TableManager* tm, SDL_Renderer* renderer)
    : config(config), 
      tableManager(tm), 
      renderer(renderer),
      tableView(tm, config), 
      tableActions(config), 
      createIniConfirmed(false), 
      selectedIniPath(config.getVPinballXIni()),
      feedbackMessage(""),
      feedbackMessageTimer(0.0f),
      pendingExtractVBS(false),
      pendingPlay(false),
      pendingTableIndex(static_cast<size_t>(-1)),
      delayTimer(0.0f) {}

bool Launcher::isShiftKeyDown() const {
    return ImGui::GetIO().KeyShift;
}

void Launcher::handlePendingOperations(std::vector<TableEntry>& tables) {
    if (pendingExtractVBS || pendingPlay) {
        // Wait for the delay to complete before starting the operation
        if (delayTimer > 0.0f) {
            delayTimer -= ImGui::GetIO().DeltaTime;
            if (delayTimer <= 0.0f) {
                // Delay is complete, execute the operation
                if (pendingExtractVBS && pendingTableIndex != static_cast<size_t>(-1) && pendingTableIndex < tables.size()) {
                    std::string vbsFile = tables[pendingTableIndex].filepath;
                    LOG_DEBUG("Original filepath: " << vbsFile);
                    // Normalize the path to remove any trailing slashes
                    vbsFile = std::filesystem::path(vbsFile).string();
                    vbsFile = vbsFile.substr(0, vbsFile.find_last_of('.')) + ".vbs";
                    LOG_DEBUG("Constructed vbsFile path: " << vbsFile);
                    if (std::filesystem::exists(vbsFile)) {
                        tableActions.openInExternalEditor(vbsFile);
                    } else {
                        tableActions.extractVBS(tables[pendingTableIndex].filepath);
                        if (std::filesystem::exists(vbsFile)) tableActions.openInExternalEditor(vbsFile);
                    }
                    pendingExtractVBS = false;
                    pendingTableIndex = static_cast<size_t>(-1);
                }

                if (pendingPlay && pendingTableIndex != static_cast<size_t>(-1) && pendingTableIndex < tables.size()) {
                    std::string filepath = tables[pendingTableIndex].filepath;
                    bool success = tableActions.launchTable(filepath);
                    LOG_DEBUG("Launched table at index " << pendingTableIndex << ": " << filepath << ", success=" << success);
                    tableManager->updateTableLastRun(filepath, success ? "success" : "failed");
                    pendingPlay = false;
                    pendingTableIndex = static_cast<size_t>(-1);
                }
            }
        }
    }
}

void Launcher::draw(std::vector<TableEntry>& tables, bool& editingIni, bool& editingSettings, bool& quitRequested, bool& showCreateIniPrompt, bool& showNoTablePopup) {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
    ImGui::Begin("VPX GUI Tools", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

    float dpiScale = ImGui::GetIO().FontGlobalScale;
    if (dpiScale <= 0.0f) dpiScale = 1.0f;

    if (tableManager->isLoading()) {
        const char* loadingText = "Updating tables...";
        ImVec2 textSize = ImGui::CalcTextSize(loadingText);
        ImVec2 windowSize = ImGui::GetIO().DisplaySize;
        ImGui::SetCursorPos(ImVec2((windowSize.x - textSize.x) * 0.5f, (windowSize.y - textSize.y) * 0.5f));
        ImGui::Text("%s", loadingText);
    } else {
        ImGuiIO& io = ImGui::GetIO();
        bool shouldFocusSearch = (!io.WantTextInput && ImGui::IsKeyPressed(ImGuiKey_Space, false));

        char tablesFoundText[32];
        snprintf(tablesFoundText, sizeof(tablesFoundText), "Table(s) found: %zu", tables.size());
        ImGui::Text("%s", tablesFoundText);

        float headerHeight = ImGui::GetCursorPosY();
        float buttonHeight = ImGui::GetFrameHeight() * dpiScale + ImGui::GetStyle().ItemSpacing.y * 2 * dpiScale;
        float availableHeight = ImGui::GetIO().DisplaySize.y - headerHeight - buttonHeight;

        ImGui::BeginChild("TableContainer", ImVec2(0, availableHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
        tableView.drawTable(tables);
        ImGui::EndChild();

        if (ImGui::Button("⛭")) editingSettings = true;
        buttonTooltips.renderTooltip("⛭");
        ImGui::SameLine();
        if (ImGui::Button("INI Editor")) {
            int selectedTable = tableView.getSelectedTable();
            if (selectedTable >= 0) {
                std::string iniFile = tables[selectedTable].filepath;
                iniFile = iniFile.substr(0, iniFile.find_last_of('.')) + ".ini";
                selectedIniPath = iniFile;
                if (std::filesystem::exists(iniFile)) editingIni = true;
                else showCreateIniPrompt = true;
            } else {
                selectedIniPath = config.getVPinballXIni();
                editingIni = true;
            }
        }
        buttonTooltips.renderTooltip("INI Editor");
        ImGui::SameLine();
        if (ImGui::Button("Extract VBS")) {
            int selectedTable = tableView.getSelectedTable();
            if (selectedTable >= 0) {
                // Check if the .vbs file exists before setting the message
                std::string vbsFile = tables[selectedTable].filepath;
                vbsFile = std::filesystem::path(vbsFile).string();
                vbsFile = vbsFile.substr(0, vbsFile.find_last_of('.')) + ".vbs";
                if (std::filesystem::exists(vbsFile)) {
                    feedbackMessage = "Opening VBS...";
                } else {
                    feedbackMessage = "Extracting VBS...";
                }
                feedbackMessageTimer = FEEDBACK_MESSAGE_DURATION;
                pendingExtractVBS = true;
                pendingTableIndex = static_cast<size_t>(selectedTable);
                delayTimer = DELAY_DURATION;
            } else {
                showNoTablePopup = true;
            }
        }
        buttonTooltips.renderTooltip("Extract VBS");
        ImGui::SameLine();
        if (ImGui::Button("Open Folder")) {
            int selectedTable = tableView.getSelectedTable();
            tableActions.openFolder(selectedTable >= 0 ? tables[selectedTable].filepath : config.getTablesDir());
        }
        buttonTooltips.renderTooltip("Open Folder");
        ImGui::SameLine();
        if (ImGui::Button("Refresh")) {
            bool forceFullRefresh = isShiftKeyDown();
            tableManager->refreshTables(forceFullRefresh);
        }
        buttonTooltips.renderTooltip("Refresh");
        ImGui::SameLine();
        float playButtonPosX = ImGui::GetCursorPosX();
        float playButtonWidth = ImGui::CalcTextSize("▶ Play").x + ImGui::GetStyle().FramePadding.x * 2 * dpiScale;
        if (ImGui::Button("▶ Play")) {
            int selectedTable = tableView.getSelectedTable();
            if (selectedTable >= 0) {
                feedbackMessage = "VPX is launching...";
                feedbackMessageTimer = FEEDBACK_MESSAGE_DURATION;
                pendingPlay = true;
                pendingTableIndex = static_cast<size_t>(selectedTable);
                delayTimer = DELAY_DURATION;
            } else {
                showNoTablePopup = true;
            }
        }
        buttonTooltips.renderTooltip("▶ Play");
        ImGui::SameLine();
        float padding = ImGui::GetStyle().ItemSpacing.x * dpiScale;
        ImGui::SetCursorPosX(playButtonPosX + playButtonWidth + padding);
        float searchBarWidth = 350.0f * dpiScale;
        char searchBuf[300];
        strncpy(searchBuf, searchQuery.c_str(), sizeof(searchBuf) - 1);
        searchBuf[sizeof(searchBuf) - 1] = '\0';
        ImGui::PushItemWidth(searchBarWidth);

        if (shouldFocusSearch) {
            ImGui::SetKeyboardFocusHere();
        }

        if (ImGui::InputTextWithHint("##Search", "Search", searchBuf, sizeof(searchBuf))) {
            searchQuery = searchBuf;
        }

        ImGui::SameLine();
        if (ImGui::Button("X")) {
            searchQuery.clear();
        }

        if (ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            searchQuery.clear();
        }

        ImGui::PopItemWidth();
        buttonTooltips.renderTooltip("X");
        ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize("✖ Quit").x - ImGui::GetStyle().ItemSpacing.x * 2 * dpiScale);
        if (ImGui::Button("✖ Quit")) quitRequested = true;

        if (feedbackMessageTimer > 0.0f) {
            feedbackMessageTimer -= ImGui::GetIO().DeltaTime;
            if (feedbackMessageTimer < 0.0f) feedbackMessageTimer = 0.0f;

            float alpha = feedbackMessageTimer / FEEDBACK_MESSAGE_DURATION;
            ImGui::SetNextWindowBgAlpha(alpha);

            ImVec2 windowSize = ImGui::GetIO().DisplaySize;
            ImVec2 textSize = ImGui::CalcTextSize(feedbackMessage.c_str());
            // Center the modal both horizontally and vertically
            ImGui::SetNextWindowPos(ImVec2((windowSize.x - textSize.x - 20.0f) * 0.5f, (windowSize.y - textSize.y - 20.0f) * 0.5f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(textSize.x + 20.0f, textSize.y + 20.0f), ImGuiCond_Always);

            ImGui::Begin("FeedbackPopup", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, alpha), "%s", feedbackMessage.c_str());
            ImGui::End();
        }
        buttonTooltips.renderTooltip("✖ Quit");
    }
    ImGui::End();

    handlePendingOperations(tables);
}