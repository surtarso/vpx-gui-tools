#include "launcher/table_view.h"
#include <filesystem>
#include <sstream>
#include <vector>

TableView::TableView(TableManager* tm, IConfigProvider& config) : tableManager(tm), config(config), selectedTable(-1) {}

void TableView::drawTable(std::vector<TableEntry>& tables) {
    float dpiScale = ImGui::GetIO().FontGlobalScale;
    if (dpiScale <= 0.0f) dpiScale = 1.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.0f * dpiScale, 2.0f * dpiScale));

    if (ImGui::BeginTable("Tables", 13, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | 
                          ImGuiTableFlags_ScrollX | ImGuiTableFlags_Sortable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Year", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 30.0f * dpiScale);
        ImGui::TableSetupColumn("Author", ImGuiTableColumnFlags_WidthFixed, 60.0f * dpiScale);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 200.0f * dpiScale);
        ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed, 35.0f * dpiScale);
        ImGui::TableSetupColumn("Extra Files", ImGuiTableColumnFlags_WidthFixed, 90.0f * dpiScale);
        ImGui::TableSetupColumn("ROM", ImGuiTableColumnFlags_WidthFixed, 65.0f * dpiScale);
        ImGui::TableSetupColumn("UltraDMD", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 15.0f * dpiScale);
        ImGui::TableSetupColumn("AltSound", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 15.0f * dpiScale);
        ImGui::TableSetupColumn("AltColor", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 15.0f * dpiScale);
        ImGui::TableSetupColumn("PUPMedia", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 15.0f * dpiScale);
        ImGui::TableSetupColumn("Music", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 15.0f * dpiScale);
        ImGui::TableSetupColumn("Images", ImGuiTableColumnFlags_WidthFixed, 175.0f * dpiScale);
        ImGui::TableSetupColumn("Videos", ImGuiTableColumnFlags_WidthFixed, 100.0f * dpiScale);
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
            ImGui::TableNextRow(ImGuiTableRowFlags_None, 30.0f * dpiScale);
            ImGui::PushID(static_cast<int>(i));
            bool isSelected = (selectedTable == static_cast<int>(i));
            if (ImGui::TableSetColumnIndex(0)) {
                char rowLabel[1024];
                snprintf(rowLabel, sizeof(rowLabel), "%s##%zu", tables[i].year.c_str(), i);
                bool wasClicked = ImGui::Selectable(rowLabel, &isSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap);
                if (wasClicked) {
                    selectedTable = (selectedTable == static_cast<int>(i)) ? -1 : static_cast<int>(i);
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
                    std::string extraFiles;
                    if (tables[i].extraFiles.find("INI") != std::string::npos) {
                        extraFiles += tables[i].iniModified ? "[INI] " : "INI ";
                    }
                    if (tables[i].extraFiles.find("VBS") != std::string::npos) {
                        extraFiles += tables[i].vbsModified ? "[VBS] " : "VBS ";
                    }
                    if (tables[i].extraFiles.find("B2S") != std::string::npos) extraFiles += "B2S";
                    ImGui::Text("%s", extraFiles.c_str());
                }
                ImGui::TableSetColumnIndex(5); ImGui::Text("%s", tables[i].rom.c_str());
                ImGui::TableSetColumnIndex(6); {
                    if (!tables[i].udmd.empty()) ImGui::TextColored(ImVec4(0.5f, 0, 0.7f, 0.90f), "%s", tables[i].udmd.c_str());
                }
                ImGui::TableSetColumnIndex(7); {
                    if (!tables[i].alts.empty()) ImGui::TextColored(ImVec4(0.4f, 0.6f, 0.9f, 0.85f), "%s", tables[i].alts.c_str());
                }
                ImGui::TableSetColumnIndex(8); {
                    if (!tables[i].altc.empty()) ImGui::TextColored(ImVec4(0.7f, 0.4f, 0, 0.90f), "%s", tables[i].altc.c_str());
                }
                ImGui::TableSetColumnIndex(9); {
                    if (!tables[i].pup.empty()) ImGui::TextColored(ImVec4(0.7f, 0.7f, 0, 0.90f), "%s", tables[i].pup.c_str());
                }
                ImGui::TableSetColumnIndex(10); {
                    if (!tables[i].music.empty()) ImGui::TextColored(ImVec4(0.4f, 0.6f, 0.9f, 0.85f), "%s", tables[i].music.c_str());
                }
                ImGui::TableSetColumnIndex(11); {
                    std::string tableDir = std::filesystem::path(tables[i].filepath).parent_path().string();
                    std::string images = tables[i].images;
                    if (images.find("Wheel") == std::string::npos && checkFilePresence(tableDir, config.getWheelImage())) images += " Wheel";
                    if (images.find("Table") == std::string::npos && checkFilePresence(tableDir, config.getTableImage())) images += " Table";
                    if (images.find("B2S") == std::string::npos && checkFilePresence(tableDir, config.getBackglassImage())) images += " B2S";
                    if (images.find("Marquee") == std::string::npos && checkFilePresence(tableDir, config.getMarqueeImage())) images += " Marquee";
                    ImGui::Text("%s", images.c_str());
                }
                ImGui::TableSetColumnIndex(12); {
                    std::string tableDir = std::filesystem::path(tables[i].filepath).parent_path().string();
                    std::string videos = tables[i].videos;
                    if (videos.find("Table") == std::string::npos && checkFilePresence(tableDir, config.getTableVideo())) videos += " Table";
                    if (videos.find("B2S") == std::string::npos && checkFilePresence(tableDir, config.getBackglassVideo())) videos += " B2S";
                    if (videos.find("DMD") == std::string::npos && checkFilePresence(tableDir, config.getDmdVideo())) videos += " DMD";
                    ImGui::Text("%s", videos.c_str());
                }
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    ImGui::PopStyleVar();
}

bool TableView::checkFilePresence(const std::string& tablePath, const std::string& relativePath) {
    return std::filesystem::exists(tablePath + "/" + relativePath);
}