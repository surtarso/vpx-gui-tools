#include "launcher/table_view.h"
#include <filesystem>
#include <sstream>
#include <vector>

TableView::TableView(TableManager* tm, IConfigProvider& config) : tableManager(tm), config(config), selectedTable(-1) {}

void TableView::drawTable(std::vector<TableEntry>& tables) {
    if (ImGui::BeginTable("Tables", 13, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | 
                          ImGuiTableFlags_ScrollX | ImGuiTableFlags_Sortable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Year", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("Author", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed, 35.0f);
        ImGui::TableSetupColumn("Extra Files", ImGuiTableColumnFlags_WidthFixed, 90.0f);
        ImGui::TableSetupColumn("ROM", ImGuiTableColumnFlags_WidthFixed, 65.0f);
        ImGui::TableSetupColumn("UltraDMD", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 15.0f);
        ImGui::TableSetupColumn("AltSound", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 15.0f);
        ImGui::TableSetupColumn("AltColor", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 15.0f);
        ImGui::TableSetupColumn("PUPMedia", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 15.0f);
        ImGui::TableSetupColumn("Music", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 15.0f);
        ImGui::TableSetupColumn("Images", ImGuiTableColumnFlags_WidthFixed, 175.0f);
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
                    if (selectedTable == static_cast<int>(i)) selectedTable = -1;
                    else selectedTable = static_cast<int>(i);
                }
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && selectedTable >= 0) {
                    std::string folder = std::filesystem::path(tables[selectedTable].filepath).parent_path().string();
                    std::string cmd = "xdg-open \"" + folder + "\"";
                    int result = system(cmd.c_str());
                    if (result != 0) {
                        LOG_DEBUG("Failed to open table folder: " << folder << " (command: " << cmd << ")");
                    }
                }
                if (isSelected && ImGui::IsKeyPressed(ImGuiKey_Enter) && selectedTable >= 0) {
                    std::string cmd = config.getStartArgs() + " \"" + config.getCommandToRun() + "\" " + config.getPlaySubCmd() + " \"" + tables[selectedTable].filepath + "\" " + config.getEndArgs();
                    int result = system(cmd.c_str());
                    if (result != 0) {
                        LOG_DEBUG("Failed to open: " << tables[selectedTable].filepath << " (command: " << cmd << ")");
                    }
                }
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", tables[i].brand.c_str());
                ImGui::TableSetColumnIndex(2); ImGui::Text("%s", tables[i].name.c_str());
                ImGui::TableSetColumnIndex(3); ImGui::Text("%s", tables[i].version.c_str());
                ImGui::TableSetColumnIndex(4); {
                    if (tables[i].extraFiles.find("INI") != std::string::npos) {
                        if (tables[i].iniModified) ImGui::TextColored(ImVec4(1, 1, 0, 1), "INI ");
                        else ImGui::Text("INI ");
                        ImGui::SameLine(0, 0);
                    }
                    if (tables[i].extraFiles.find("VBS") != std::string::npos) {
                        if (tables[i].vbsModified) ImGui::TextColored(ImVec4(1, 1, 0, 1), "VBS ");
                        else ImGui::Text("VBS ");
                        ImGui::SameLine(0, 0);
                    }
                    if (tables[i].extraFiles.find("B2S") != std::string::npos) ImGui::Text("B2S");
                }
                ImGui::TableSetColumnIndex(5); ImGui::Text("%s", tables[i].rom.c_str());
                ImGui::TableSetColumnIndex(6); { // uDMD column
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 20.0f);
                    if (!tables[i].udmd.empty()) {
                        ImGui::TextColored(ImVec4(0.5f, 0, 0.7f, 0.90f), "%s", tables[i].udmd.c_str()); // Light purple
                    }
                    ImGui::PopTextWrapPos();
                }
                ImGui::TableSetColumnIndex(7); { // AltS column
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 20.0f);
                    if (!tables[i].alts.empty()) {
                        ImGui::TextColored(ImVec4(0.4f, 0.6f, 0.9f, 0.85f), "%s", tables[i].alts.c_str()); // Light blue
                    }
                    ImGui::PopTextWrapPos();
                }
                ImGui::TableSetColumnIndex(8); { // AltC column
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 20.0f);
                    if (!tables[i].altc.empty()) {
                        ImGui::TextColored(ImVec4(0.7f, 0.4f, 0, 0.90f), "%s", tables[i].altc.c_str()); // Light orange
                    }
                    ImGui::PopTextWrapPos();
                }
                ImGui::TableSetColumnIndex(9); { // PUP column
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 20.0f);
                    if (!tables[i].pup.empty()) {
                        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0, 0.90f), "%s", tables[i].pup.c_str()); // Light yellow
                    }
                    ImGui::PopTextWrapPos();
                }
                ImGui::TableSetColumnIndex(10); { // Music column
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 20.0f);
                    if (!tables[i].music.empty()) {
                        ImGui::TextColored(ImVec4(0.4f, 0.6f, 0.9f, 0.85f), "%s", tables[i].music.c_str()); // Light blue
                    }
                    ImGui::PopTextWrapPos();
                }
                ImGui::TableSetColumnIndex(11); { // Images column
                    std::string tableDir = std::filesystem::path(tables[i].filepath).parent_path().string();
                    std::istringstream iss(tables[i].images);
                    std::string word;
                    bool first = true;
                    while (iss >> word) {
                        if (!first) ImGui::SameLine(0, 0);
                        ImGui::TextColored(
                            checkFilePresence(tableDir, 
                                word == "Wheel" ? config.getWheelImage() :
                                word == "Table" ? config.getTableImage() :
                                word == "B2S" ? config.getBackglassImage() :
                                word == "Marquee" ? config.getMarqueeImage() : "") ? 
                            ImVec4(0, 0.7f, 0, 0.90f) : ImVec4(0.7f, 0, 0, 0.90f), "%s", word.c_str());
                        first = false;
                    }
                    // Fill in missing parts
                    if (tables[i].images.find("Wheel") == std::string::npos) {
                        if (!first) ImGui::SameLine(0, 0);
                        ImGui::TextColored(checkFilePresence(tableDir, config.getWheelImage()) ? ImVec4(0, 0.7f, 0, 0.90f) : ImVec4(0.7f, 0, 0, 0.90f), "Wheel");
                    }
                    if (tables[i].images.find("Table") == std::string::npos) {
                        if (!first) ImGui::SameLine(0, 0);
                        ImGui::TextColored(checkFilePresence(tableDir, config.getTableImage()) ? ImVec4(0, 0.7f, 0, 0.90f) : ImVec4(0.7f, 0, 0, 0.90f), "Table");
                    }
                    if (tables[i].images.find("B2S") == std::string::npos) {
                        if (!first) ImGui::SameLine(0, 0);
                        ImGui::TextColored(checkFilePresence(tableDir, config.getBackglassImage()) ? ImVec4(0, 0.7f, 0, 0.90f) : ImVec4(0.7f, 0, 0, 0.90f), "B2S");
                    }
                    if (tables[i].images.find("Marquee") == std::string::npos) {
                        if (!first) ImGui::SameLine(0, 0);
                        ImGui::TextColored(checkFilePresence(tableDir, config.getMarqueeImage()) ? ImVec4(0, 0.7f, 0, 0.90f) : ImVec4(0.7f, 0, 0, 0.90f), "Marquee");
                    }
                }
                ImGui::TableSetColumnIndex(12); { // Videos column
                    std::string tableDir = std::filesystem::path(tables[i].filepath).parent_path().string();
                    std::istringstream iss(tables[i].videos);
                    std::string word;
                    bool first = true;
                    while (iss >> word) {
                        if (!first) ImGui::SameLine(0, 0);
                        ImGui::TextColored(
                            checkFilePresence(tableDir, 
                                word == "Table" ? config.getTableVideo() :
                                word == "B2S" ? config.getBackglassVideo() :
                                word == "DMD" ? config.getDmdVideo() : "") ? 
                            ImVec4(0, 0.7f, 0, 0.90f) : ImVec4(0.7f, 0, 0, 0.90f), "%s", word.c_str());
                        first = false;
                    }
                    // Fill in missing parts
                    if (tables[i].videos.find("Table") == std::string::npos) {
                        if (!first) ImGui::SameLine(0, 0);
                        ImGui::TextColored(checkFilePresence(tableDir, config.getTableVideo()) ? ImVec4(0, 0.7f, 0, 0.90f) : ImVec4(0.7f, 0, 0, 0.90f), "Table");
                    }
                    if (tables[i].videos.find("B2S") == std::string::npos) {
                        if (!first) ImGui::SameLine(0, 0);
                        ImGui::TextColored(checkFilePresence(tableDir, config.getBackglassVideo()) ? ImVec4(0, 0.7f, 0, 0.90f) : ImVec4(0.7f, 0, 0, 0.90f), "B2S");
                    }
                    if (tables[i].videos.find("DMD") == std::string::npos) {
                        if (!first) ImGui::SameLine(0, 0);
                        ImGui::TextColored(checkFilePresence(tableDir, config.getDmdVideo()) ? ImVec4(0, 0.7f, 0, 0.90f) : ImVec4(0.7f, 0, 0, 0.90f), "DMD");
                    }
                }
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

bool TableView::checkFilePresence(const std::string& tablePath, const std::string& relativePath) {
    return std::filesystem::exists(tablePath + "/" + relativePath);
}