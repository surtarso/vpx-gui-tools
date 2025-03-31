#include "launcher/launcher.h"
#include "imgui.h"
#include <filesystem>

Launcher::Launcher(IConfigProvider& config, TableManager* tm)
    : config(config), tableManager(tm), tableView(tm, config), tableActions(config), createIniConfirmed(false), selectedIniPath(config.getVPinballXIni()) {}

void Launcher::draw(std::vector<TableEntry>& tables, bool& editingIni, bool& editingSettings, bool& quitRequested, bool& showCreateIniPrompt, bool& showNoTablePopup) {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
    ImGui::Begin("VPX GUI Tools", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

    if (tableManager->isLoading()) {
        ImGui::Text("Loading tables...");
    } else {
        ImGuiIO& io = ImGui::GetIO();
        bool shouldFocusSearch = false;
        if (!io.WantTextInput && ImGui::IsKeyPressed(ImGuiKey_Space, false)) { // Focus on spacebar press
            shouldFocusSearch = true;
        }

        char tablesFoundText[32];
        snprintf(tablesFoundText, sizeof(tablesFoundText), "Table(s) found: %zu", tables.size());
        ImGui::Text("%s", tablesFoundText);

        float headerHeight = ImGui::GetCursorPosY();
        float buttonHeight = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y * 2;
        float availableHeight = ImGui::GetIO().DisplaySize.y - headerHeight - buttonHeight;

        ImGui::BeginChild("TableContainer", ImVec2(0, availableHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
        tableView.drawTable(tables);
        ImGui::EndChild();

        if (ImGui::Button("⛭")) editingSettings = true;
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
        ImGui::SameLine();
        if (ImGui::Button("Extract VBS")) {
            int selectedTable = tableView.getSelectedTable();
            if (selectedTable >= 0) {
                std::string vbsFile = tables[selectedTable].filepath;
                vbsFile = vbsFile.substr(0, vbsFile.find_last_of('.')) + ".vbs";
                if (std::filesystem::exists(vbsFile)) tableActions.openInExternalEditor(vbsFile);
                else {
                    tableActions.extractVBS(tables[selectedTable].filepath);
                    if (std::filesystem::exists(vbsFile)) tableActions.openInExternalEditor(vbsFile);
                }
            } else showNoTablePopup = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Open Folder")) {
            int selectedTable = tableView.getSelectedTable();
            tableActions.openFolder(selectedTable >= 0 ? tables[selectedTable].filepath : config.getTablesDir());
        }
        ImGui::SameLine();
        float playButtonPosX = ImGui::GetCursorPosX();
        float playButtonWidth = ImGui::CalcTextSize("▶ Play").x + ImGui::GetStyle().FramePadding.x * 2;
        if (ImGui::Button("▶ Play")) {
            int selectedTable = tableView.getSelectedTable();
            if (selectedTable >= 0) tableActions.launchTable(tables[selectedTable].filepath);
            else showNoTablePopup = true;
        }
        ImGui::SameLine();
        float padding = ImGui::GetStyle().ItemSpacing.x;
        ImGui::SetCursorPosX(playButtonPosX + playButtonWidth + padding);
        float searchBarWidth = 350.0f;
        char searchBuf[300];
        strncpy(searchBuf, searchQuery.c_str(), sizeof(searchBuf) - 1);
        searchBuf[sizeof(searchBuf) - 1] = '\0';
        ImGui::PushItemWidth(searchBarWidth);

        // Focus the search bar if flagged from the previous frame
        if (shouldFocusSearch) {
            ImGui::SetKeyboardFocusHere();
        }

        // Render the search bar
        if (ImGui::InputTextWithHint("##Search", "Search", searchBuf, sizeof(searchBuf))) {
            searchQuery = searchBuf;
        }

        // Add 'x' button to clear the search bar
        ImGui::SameLine();
        if (ImGui::Button("X")) {
            searchQuery.clear(); // Clear the search query
        }

        // Clear search bar with ESC
        if (ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            searchQuery.clear(); // Clear the search query
        }

        ImGui::PopItemWidth();

        ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize("✖ Quit").x - ImGui::GetStyle().ItemSpacing.x * 2);
        if (ImGui::Button("✖ Quit")) quitRequested = true;
    }
    ImGui::End();
}