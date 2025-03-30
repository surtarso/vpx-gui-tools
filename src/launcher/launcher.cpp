#include "launcher/launcher.h"
#include "imgui.h"
#include <filesystem>
#include <cstdlib>
#include <algorithm>

Launcher::Launcher(IConfigProvider& config, TableManager* tm)
    : config(config), tableManager(tm), selectedIniPath(config.getVPinballXIni()) {}

void Launcher::draw(std::vector<TableEntry>& tables, bool& editingIni, bool& editingSettings, bool& quitRequested, bool& showCreateIniPrompt) {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
    ImGui::Begin("VPX GUI Tools", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

    char tablesFoundText[32];
    snprintf(tablesFoundText, sizeof(tablesFoundText), "Table(s) found: %zu", tables.size());
    ImGui::Text("%s", tablesFoundText);

    float headerHeight = ImGui::GetCursorPosY();
    float buttonHeight = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y * 2;
    float availableHeight = ImGui::GetIO().DisplaySize.y - headerHeight - buttonHeight;

    ImGui::BeginChild("TableContainer", ImVec2(0, availableHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
    if (ImGui::BeginTable("Tables", 12, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX | 
                          ImGuiTableFlags_Sortable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Year", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Brand", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::TableSetupColumn("Extra Files", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("ROM", ImGuiTableColumnFlags_WidthFixed, 40.0f);
        ImGui::TableSetupColumn("uDMD", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("AltS", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("AltC", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("PUP", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("Music", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("Images", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Videos", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs()) {
            if (sortSpecs->SpecsDirty) {
                const ImGuiTableColumnSortSpecs* spec = &sortSpecs->Specs[0];
                int columnIdx = spec->ColumnIndex;
                bool isAscending = spec->SortDirection == ImGuiSortDirection_Ascending;
                tableManager->setSortSpecs(columnIdx, isAscending);
                sortSpecs->SpecsDirty = false;
            }
        }

        for (size_t i = 0; i < tables.size(); ++i) {
            ImGui::TableNextRow();
            ImGui::PushID(static_cast<int>(i));
            bool isSelected = (selectedTable == static_cast<int>(i));
            if (ImGui::TableSetColumnIndex(0)) {
                char rowLabel[1024];
                snprintf(rowLabel, sizeof(rowLabel), "%s##%zu", tables[i].year.c_str(), i);
                bool wasClicked = ImGui::Selectable(rowLabel, &isSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap);
                if (wasClicked) {
                    if (selectedTable == static_cast<int>(i)) {
                        selectedTable = -1;
                    } else {
                        selectedTable = static_cast<int>(i);
                    }
                }
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && selectedTable >= 0) {
                    openFolder(tables[selectedTable].filepath);
                }
                if (isSelected && ImGui::IsKeyPressed(ImGuiKey_Enter) && selectedTable >= 0) {
                    launchTable(tables[selectedTable].filepath);
                }
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", tables[i].brand.c_str());
                ImGui::TableSetColumnIndex(2); ImGui::Text("%s", tables[i].name.c_str());
                ImGui::TableSetColumnIndex(3); ImGui::Text("%s", tables[i].extraFiles.c_str());
                ImGui::TableSetColumnIndex(4); ImGui::Text("%s", tables[i].rom.c_str());
                ImGui::TableSetColumnIndex(5); ImGui::Text("%s", tables[i].udmd.c_str());
                ImGui::TableSetColumnIndex(6); ImGui::Text("%s", tables[i].alts.c_str());
                ImGui::TableSetColumnIndex(7); ImGui::Text("%s", tables[i].altc.c_str());
                ImGui::TableSetColumnIndex(8); ImGui::Text("%s", tables[i].pup.c_str());
                ImGui::TableSetColumnIndex(9); ImGui::Text("%s", tables[i].music.c_str());
                ImGui::TableSetColumnIndex(10); ImGui::Text("%s", tables[i].images.c_str());
                ImGui::TableSetColumnIndex(11); ImGui::Text("%s", tables[i].videos.c_str());
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();

    if (ImGui::Button("⛭")) {
        editingSettings = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("INI Editor")) {
        if (selectedTable >= 0) {
            std::string iniFile = tables[selectedTable].filepath;
            iniFile = iniFile.substr(0, iniFile.find_last_of('.')) + ".ini";
            selectedIniPath = iniFile;
            if (std::filesystem::exists(iniFile)) {
                editingIni = true;
            } else {
                showCreateIniPrompt = true;
            }
        } else {
            selectedIniPath = config.getVPinballXIni();
            editingIni = true;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Extract VBS")) {
        if (selectedTable >= 0) {
            std::string vbsFile = tables[selectedTable].filepath;
            vbsFile = vbsFile.substr(0, vbsFile.find_last_of('.')) + ".vbs";
            if (std::filesystem::exists(vbsFile)) {
                openInExternalEditor(vbsFile);
            } else {
                extractVBS(tables[selectedTable].filepath);
                if (std::filesystem::exists(vbsFile)) {
                    openInExternalEditor(vbsFile);
                }
            }
        } else {
            ImGui::OpenPopup("No Table Selected");
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Open Folder")) {
        openFolder(selectedTable >= 0 ? tables[selectedTable].filepath : config.getTablesDir());
    }
    ImGui::SameLine();
    float playButtonPosX = ImGui::GetCursorPosX();
    float playButtonWidth = ImGui::CalcTextSize("▶ Play").x + ImGui::GetStyle().FramePadding.x * 2;
    if (ImGui::Button("▶ Play") && selectedTable >= 0) {
        launchTable(tables[selectedTable].filepath);
    }
    ImGui::SameLine();
    float padding = ImGui::GetStyle().ItemSpacing.x;
    ImGui::SetCursorPosX(playButtonPosX + playButtonWidth + padding);
    float searchBarWidth = 350.0f;
    char searchBuf[300];
    strncpy(searchBuf, searchQuery.c_str(), sizeof(searchBuf) - 1);
    searchBuf[sizeof(searchBuf) - 1] = '\0';
    ImGui::PushItemWidth(searchBarWidth);
    if (ImGui::InputTextWithHint("##Search", "Search", searchBuf, sizeof(searchBuf))) {
        searchQuery = searchBuf;
    }
    ImGui::PopItemWidth();

    ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize("✖ Quit").x - ImGui::GetStyle().ItemSpacing.x * 2);
    if (ImGui::Button("✖ Quit")) {
        quitRequested = true;
    }

    ImGui::End();
}

void Launcher::launchTable(const std::string& filepath) {
    std::string cmd = config.getStartArgs() + " \"" + config.getCommandToRun() + "\" " + config.getPlaySubCmd() + " \"" + filepath + "\" " + config.getEndArgs();
    system(cmd.c_str());
}

void Launcher::extractVBS(const std::string& filepath) {
    std::string cmd = "\"" + config.getVpxTool() + "\" " + config.getVbsSubCmd() + " \"" + filepath + "\"";
    system(cmd.c_str());
}

bool Launcher::openInExternalEditor(const std::string& filepath) {
    std::string cmd = "xdg-open \"" + filepath + "\"";
    if (system(cmd.c_str()) != 0) {
        if (!config.getFallbackEditor().empty()) {
            cmd = "\"" + config.getFallbackEditor() + "\" \"" + filepath + "\"";
            return system(cmd.c_str()) == 0;
        }
        return false;
    }
    return true;
}

void Launcher::openFolder(const std::string& filepath) {
    std::string folder = filepath.empty() ? config.getTablesDir() : filepath.substr(0, filepath.find_last_of('/'));
    std::string cmd = "xdg-open \"" + folder + "\"";
    system(cmd.c_str());
}