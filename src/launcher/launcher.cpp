#include "launcher/launcher.h"
#include "imgui.h"
#include <filesystem>
#include <cstdlib>
#include <algorithm>

Launcher::Launcher(const std::string& tablesDir, const std::string& startArgs, const std::string& commandToRun,
                   const std::string& endArgs, const std::string& vpinballXIni, TableManager* tm)
    : tablesDir(tablesDir), startArgs(startArgs), commandToRun(commandToRun), endArgs(endArgs), vpinballXIni(vpinballXIni), tableManager(tm) {}

void Launcher::draw(std::vector<TableEntry>& tables, bool& editingIni, bool& editingSettings, bool& quitRequested) {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
    ImGui::Begin("VPX GUI Tools", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

    ImGui::Text("Table(s) found: %zu", tables.size());
    char searchBuf[256];
    strncpy(searchBuf, searchQuery.c_str(), sizeof(searchBuf) - 1);
    searchBuf[sizeof(searchBuf) - 1] = '\0';
    if (ImGui::InputText("Search", searchBuf, sizeof(searchBuf))) {
        searchQuery = searchBuf;
    }

    float headerHeight = ImGui::GetCursorPosY();
    float buttonHeight = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y * 2;
    float availableHeight = ImGui::GetIO().DisplaySize.y - headerHeight - buttonHeight;

    // Enable horizontal scrolling with ScrollX
    ImGui::BeginChild("TableContainer", ImVec2(0, availableHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
    if (ImGui::BeginTable("Tables", 12, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX | 
                          ImGuiTableFlags_Sortable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Resizable)) {
        // Set column widths (icon columns narrower)
        ImGui::TableSetupColumn("Year", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Brand", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 200.0f);  // Stretch for readability
        ImGui::TableSetupColumn("Extra Files", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("ROM", ImGuiTableColumnFlags_WidthFixed, 40.0f);
        ImGui::TableSetupColumn("uDMD", ImGuiTableColumnFlags_WidthFixed, 30.0f);    // Icon-only, narrow
        ImGui::TableSetupColumn("AltS", ImGuiTableColumnFlags_WidthFixed, 30.0f);    // Icon-only
        ImGui::TableSetupColumn("AltC", ImGuiTableColumnFlags_WidthFixed, 30.0f);    // Icon-only
        ImGui::TableSetupColumn("PUP", ImGuiTableColumnFlags_WidthFixed, 30.0f);     // Icon-only
        ImGui::TableSetupColumn("Music", ImGuiTableColumnFlags_WidthFixed, 30.0f);   // Icon-only
        ImGui::TableSetupColumn("Images", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Videos", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupScrollFreeze(0, 1);  // Freeze header row
        ImGui::TableHeadersRow();

        // Handle sorting
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
                
                if (ImGui::Selectable(rowLabel, &isSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap)) {
                    selectedTable = static_cast<int>(i);
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
        editingIni = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Extract VBS") && selectedTable >= 0) {
        extractVBS(tables[selectedTable].filepath);
    }
    ImGui::SameLine();
    if (ImGui::Button("Open Folder")) {
        openFolder(selectedTable >= 0 ? tables[selectedTable].filepath : tablesDir);
    }
    ImGui::SameLine();
    if (ImGui::Button("▶") && selectedTable >= 0) {
        launchTable(tables[selectedTable].filepath);
    }

    ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize("✖").x - ImGui::GetStyle().ItemSpacing.x * 2 - ImGui::GetStyle().WindowPadding.x);
    if (ImGui::Button("✖")) {
        quitRequested = true;
    }

    ImGui::End();
}

void Launcher::launchTable(const std::string& filepath) {
    std::string cmd = startArgs + " \"" + commandToRun + "\" -play \"" + filepath + "\" " + endArgs;
    system(cmd.c_str());
}

void Launcher::extractVBS(const std::string& filepath) {
    std::string cmd = "\"" + commandToRun + "\" -ExtractVBS \"" + filepath + "\"";
    system(cmd.c_str());
}

void Launcher::openFolder(const std::string& filepath) {
    std::string folder = filepath.empty() ? tablesDir : filepath.substr(0, filepath.find_last_of('/'));
    std::string cmd = "xdg-open \"" + folder + "\"";
    system(cmd.c_str());
}