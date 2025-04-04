#include "launcher/table_view.h"
#include <filesystem>
#include <sstream>
#include <vector>

TableView::TableView(TableManager* tm, IConfigProvider& config) : tableManager(tm), config(config), selectedTable(-1) {}

void TableView::drawTable(std::vector<TableEntry>& tables) {
    float dpiScale = ImGui::GetIO().FontGlobalScale;
    if (dpiScale <= 0.0f) dpiScale = 1.0f;

    // Adjust padding for a compact look
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.0f * dpiScale, 1.0f * dpiScale));

    const char* tableName = "Tables";

    // Calculate total fixed width and scale if necessary to fit window
    float baseFixedWidth = 30.0f + 60.0f + 35.0f + 90.0f + 65.0f + (15.0f * 5) + 200.0f + 120.0f; // Sum of fixed widths
    float totalFixedWidth = baseFixedWidth * dpiScale;
    float windowWidth = ImGui::GetWindowWidth();
    float widthScale = (totalFixedWidth > windowWidth) ? (windowWidth * 0.8f / totalFixedWidth) : 1.0f;
    float adjustedDpiScale = dpiScale * widthScale;

    if (ImGui::BeginTable(tableName, 13, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | 
                          ImGuiTableFlags_ScrollX | ImGuiTableFlags_Sortable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Resizable)) {
        // Use adjusted scaling to prevent overflow
        ImGui::TableSetupColumn("Year", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 30.0f * adjustedDpiScale);
        ImGui::TableSetupColumn("Author", ImGuiTableColumnFlags_WidthFixed, 60.0f * adjustedDpiScale);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 200.0f * adjustedDpiScale);
        ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed, 35.0f * adjustedDpiScale);
        ImGui::TableSetupColumn("Extra Files", ImGuiTableColumnFlags_WidthFixed, 90.0f * adjustedDpiScale);
        ImGui::TableSetupColumn("ROM", ImGuiTableColumnFlags_WidthFixed, 65.0f * adjustedDpiScale);
        ImGui::TableSetupColumn("UltraDMD", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 15.0f * adjustedDpiScale);
        ImGui::TableSetupColumn("AltSound", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 15.0f * adjustedDpiScale);
        ImGui::TableSetupColumn("AltColor", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 15.0f * adjustedDpiScale);
        ImGui::TableSetupColumn("PUPMedia", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 15.0f * adjustedDpiScale);
        ImGui::TableSetupColumn("Music", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 15.0f * adjustedDpiScale);
        ImGui::TableSetupColumn("Images", ImGuiTableColumnFlags_WidthFixed, 200.0f * adjustedDpiScale); // Increased to fit "Wheel Table B2S Marquee"
        ImGui::TableSetupColumn("Videos", ImGuiTableColumnFlags_WidthFixed, 120.0f * adjustedDpiScale); // Increased to fit "Table B2S DMD"
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
            ImGui::TableNextRow(ImGuiTableRowFlags_None, 20.0f * dpiScale); // Reduced for compact look
            ImGui::PushID(static_cast<int>(i));
            bool isSelected = (selectedTable == static_cast<int>(i));
            if (ImGui::TableSetColumnIndex(0)) {
                char rowLabel[1024];
                snprintf(rowLabel, sizeof(rowLabel), "%s##%zu", tables[i].year.c_str(), i);
                bool wasClicked = ImGui::Selectable(rowLabel, &isSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap);
                if (wasClicked) {
                    selectedTable = (selectedTable == static_cast<int>(i)) ? -1 : static_cast<int>(i);
                }
                // Add tooltip when hovering over the row
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("Name: %s", tables[i].name.c_str());
                    ImGui::Text("Filename: %s", tables[i].filename.c_str());
                    ImGui::EndTooltip();
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
                    // Always show "INI VBS B2S" with color coding
                    // Colors: Grey (not found), White (found, unmodified), Yellow (found, modified)
                    ImVec4 greyColor(0.5f, 0.5f, 0.5f, 1.0f);   // Grey for not found
                    ImVec4 whiteColor(1.0f, 1.0f, 1.0f, 1.0f);   // White for found, unmodified
                    ImVec4 yellowColor(1.0f, 1.0f, 0.0f, 1.0f);  // Yellow for found, modified

                    // Check INI status
                    bool iniExists = tables[i].extraFiles.find("INI") != std::string::npos;
                    bool iniModified = tables[i].iniModified;
                    ImVec4 iniColor = iniExists ? (iniModified ? yellowColor : whiteColor) : greyColor;
                    ImGui::TextColored(iniColor, "INI");

                    ImGui::SameLine();
                    // Check VBS status
                    bool vbsExists = tables[i].extraFiles.find("VBS") != std::string::npos;
                    bool vbsModified = tables[i].vbsModified;
                    ImVec4 vbsColor = vbsExists ? (vbsModified ? yellowColor : whiteColor) : greyColor;
                    ImGui::TextColored(vbsColor, "VBS");

                    ImGui::SameLine();
                    // Check B2S status (no modification check for B2S)
                    bool b2sExists = tables[i].extraFiles.find("B2S") != std::string::npos;
                    ImVec4 b2sColor = b2sExists ? whiteColor : greyColor;
                    ImGui::TextColored(b2sColor, "B2S");
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
                    // Always show "Wheel Table B2S Marquee" with color coding
                    // Colors: Green (found), Red (not found)
                    ImVec4 greenColor(0.0f, 1.0f, 0.0f, 1.0f);  // Green for found
                    ImVec4 redColor(1.0f, 0.0f, 0.0f, 1.0f);    // Red for not found

                    std::string tableDir = std::filesystem::path(tables[i].filepath).parent_path().string();
                    std::string images = tables[i].images;

                    // Check Wheel image
                    bool wheelExists = images.find("Wheel") != std::string::npos || checkFilePresence(tableDir, config.getWheelImage());
                    ImGui::TextColored(wheelExists ? greenColor : redColor, "Wheel");

                    ImGui::SameLine();
                    // Check Table image
                    bool tableImageExists = images.find("Table") != std::string::npos || checkFilePresence(tableDir, config.getTableImage());
                    ImGui::TextColored(tableImageExists ? greenColor : redColor, "Table");

                    ImGui::SameLine();
                    // Check B2S image
                    bool b2sImageExists = images.find("B2S") != std::string::npos || checkFilePresence(tableDir, config.getBackglassImage());
                    ImGui::TextColored(b2sImageExists ? greenColor : redColor, "B2S");

                    ImGui::SameLine();
                    // Check Marquee image
                    bool marqueeExists = images.find("Marquee") != std::string::npos || checkFilePresence(tableDir, config.getMarqueeImage());
                    ImGui::TextColored(marqueeExists ? greenColor : redColor, "Marquee");
                }
                ImGui::TableSetColumnIndex(12); {
                    // Always show "Table B2S DMD" with color coding
                    // Colors: Green (found), Red (not found)
                    ImVec4 greenColor(0.0f, 1.0f, 0.0f, 1.0f);  // Green for found
                    ImVec4 redColor(1.0f, 0.0f, 0.0f, 1.0f);    // Red for not found

                    std::string tableDir = std::filesystem::path(tables[i].filepath).parent_path().string();
                    std::string videos = tables[i].videos;

                    // Check Table video
                    bool tableVideoExists = videos.find("Table") != std::string::npos || checkFilePresence(tableDir, config.getTableVideo());
                    ImGui::TextColored(tableVideoExists ? greenColor : redColor, "Table");

                    ImGui::SameLine();
                    // Check B2S video
                    bool b2sVideoExists = videos.find("B2S") != std::string::npos || checkFilePresence(tableDir, config.getBackglassVideo());
                    ImGui::TextColored(b2sVideoExists ? greenColor : redColor, "B2S");

                    ImGui::SameLine();
                    // Check DMD video
                    bool dmdVideoExists = videos.find("DMD") != std::string::npos || checkFilePresence(tableDir, config.getDmdVideo());
                    ImGui::TextColored(dmdVideoExists ? greenColor : redColor, "DMD");
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