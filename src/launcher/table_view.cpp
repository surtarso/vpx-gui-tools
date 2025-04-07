#include "launcher/table_view.h"
#include <filesystem>
#include <sstream>
#include <vector>

TableView::TableView(TableManager* tm, IConfigProvider& config) : tableManager(tm), config(config), selectedTable(-1) {}

void TableView::drawTable(std::vector<TableEntry>& tables) {
    float dpiScale = ImGui::GetIO().FontGlobalScale;
    if (dpiScale <= 0.0f) dpiScale = 1.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.0f * dpiScale, 1.0f * dpiScale));

    const char* tableName = "Tables";

    ImVec2 yearSize = ImGui::CalcTextSize("2024");
    ImVec2 extraFilesSize = ImGui::CalcTextSize("INI VBS B2S");
    ImVec2 imagesSize = ImGui::CalcTextSize("Wheel Table B2S Marquee");
    ImVec2 videosSize = ImGui::CalcTextSize("Table B2S DMD");

    ImVec2 ultraDmdSize = ImGui::CalcTextSize(u8"✪");
    ImVec2 altSoundSize = ImGui::CalcTextSize(u8"♪");
    ImVec2 altColorSize = ImGui::CalcTextSize(u8"☀");
    ImVec2 pupMediaSize = ImGui::CalcTextSize(u8"▣");
    ImVec2 musicSize = ImGui::CalcTextSize(u8"♫");
    float iconWidth = std::max({ultraDmdSize.x, altSoundSize.x, altColorSize.x, pupMediaSize.x, musicSize.x});

    float padding = 1.2f;
    float yearWidth = yearSize.x * padding;
    float extraFilesWidth = extraFilesSize.x * padding;
    float imagesWidth = imagesSize.x * padding;
    float videosWidth = videosSize.x * padding;
    float adjustedIconWidth = iconWidth * padding;

    float authorBaseWidth = 60.0f * dpiScale;
    float versionBaseWidth = 35.0f * dpiScale;
    float romBaseWidth = 65.0f * dpiScale;

    float totalMinWidth = yearWidth + extraFilesWidth + imagesWidth + videosWidth + (adjustedIconWidth * 5);
    float totalVariableWidth = authorBaseWidth + versionBaseWidth + romBaseWidth;
    float totalFixedWidth = totalMinWidth + totalVariableWidth;

    float windowWidth = ImGui::GetWindowWidth();
    float widthScale = 1.0f;
    if (totalFixedWidth > windowWidth) {
        float excessWidth = totalFixedWidth - windowWidth * 0.8f;
        if (excessWidth > 0 && totalVariableWidth > 0) {
            widthScale = (totalVariableWidth - excessWidth) / totalVariableWidth;
            if (widthScale < 0.1f) widthScale = 0.1f;
        }
    }

    float adjustedAuthorWidth = authorBaseWidth * widthScale;
    float adjustedVersionWidth = versionBaseWidth * widthScale;
    float adjustedRomWidth = romBaseWidth * widthScale;

    if (ImGui::BeginTable(tableName, 13, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | 
                          ImGuiTableFlags_ScrollX | ImGuiTableFlags_Sortable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Year", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, yearWidth);
        ImGui::TableSetupColumn("Author", ImGuiTableColumnFlags_WidthFixed, adjustedAuthorWidth);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed, adjustedVersionWidth);
        ImGui::TableSetupColumn("Extra Files", ImGuiTableColumnFlags_WidthFixed, extraFilesWidth);
        ImGui::TableSetupColumn("ROM", ImGuiTableColumnFlags_WidthFixed, adjustedRomWidth);
        ImGui::TableSetupColumn("UltraDMD", ImGuiTableColumnFlags_WidthFixed, adjustedIconWidth);
        ImGui::TableSetupColumn("AltSound", ImGuiTableColumnFlags_WidthFixed, adjustedIconWidth);
        ImGui::TableSetupColumn("AltColor", ImGuiTableColumnFlags_WidthFixed, adjustedIconWidth);
        ImGui::TableSetupColumn("PUPMedia", ImGuiTableColumnFlags_WidthFixed, adjustedIconWidth);
        ImGui::TableSetupColumn("Music", ImGuiTableColumnFlags_WidthFixed, adjustedIconWidth);
        ImGui::TableSetupColumn("Images", ImGuiTableColumnFlags_WidthFixed, imagesWidth);
        ImGui::TableSetupColumn("Videos", ImGuiTableColumnFlags_WidthFixed, videosWidth);
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
            ImGui::TableNextRow(ImGuiTableRowFlags_None, 20.0f * dpiScale);
            ImGui::PushID(static_cast<int>(i));
            bool isSelected = (selectedTable == static_cast<int>(i));
            if (ImGui::TableSetColumnIndex(0)) {
                char rowLabel[1024];
                snprintf(rowLabel, sizeof(rowLabel), "%s##%zu", tables[i].year.c_str(), i);
                bool wasClicked = ImGui::Selectable(rowLabel, &isSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap);
                if (wasClicked) {
                    selectedTable = (selectedTable == static_cast<int>(i)) ? -1 : static_cast<int>(i);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    // ImGui::Text("Name: %s", tables[i].name.c_str());
                    ImGui::Text("Filename: %s", tables[i].filename.c_str());
                    if (tables[i].lastRun == "failed") {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Failed to launch table.");
                    }
                    if (tables[i].playCount > 0) {
                        ImGui::Text("Played: %d time%s", tables[i].playCount, tables[i].playCount == 1 ? "" : "s");
                    }
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
                    std::string filepath = tables[selectedTable].filepath;
                    std::string cmd = config.getStartArgs() + " \"" + config.getCommandToRun() + "\" " + config.getPlaySubCmd() + " \"" + filepath + "\" " + config.getEndArgs();
                    int result = system(cmd.c_str());
                    std::string status = (result == 0) ? "success" : "failed";
                    tableManager->updateTableLastRun(filepath, status);
                    if (result != 0) {
                        LOG_DEBUG("Failed to open: " << filepath << " (command: " << cmd << ")");
                    }
                }
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", tables[i].author.c_str());
                ImGui::TableSetColumnIndex(2); {
                    if (tables[i].lastRun == "failed") {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", tables[i].name.c_str());
                    } else {
                        ImGui::Text("%s", tables[i].name.c_str());
                    }
                }
                ImGui::TableSetColumnIndex(3); ImGui::Text("%s", tables[i].version.c_str());
                ImGui::TableSetColumnIndex(4); {
                    ImVec4 greyColor(0.5f, 0.5f, 0.5f, 1.0f);
                    ImVec4 whiteColor(1.0f, 1.0f, 1.0f, 1.0f);
                    ImVec4 yellowColor(1.0f, 1.0f, 0.0f, 1.0f);

                    bool iniExists = tables[i].extraFiles.find("INI") != std::string::npos;
                    bool iniModified = tables[i].iniModified;
                    ImVec4 iniColor = iniExists ? (iniModified ? yellowColor : whiteColor) : greyColor;
                    ImGui::TextColored(iniColor, "INI");

                    ImGui::SameLine();
                    bool vbsExists = tables[i].extraFiles.find("VBS") != std::string::npos;
                    bool vbsModified = tables[i].vbsModified;
                    ImVec4 vbsColor = vbsExists ? (vbsModified ? yellowColor : whiteColor) : greyColor;
                    ImGui::TextColored(vbsColor, "VBS");

                    ImGui::SameLine();
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
                    ImVec4 greenColor(0.0f, 1.0f, 0.0f, 1.0f);
                    ImVec4 redColor(1.0f, 0.0f, 0.0f, 1.0f);

                    std::string tableDir = std::filesystem::path(tables[i].filepath).parent_path().string();
                    std::string images = tables[i].images;

                    bool wheelExists = images.find("Wheel") != std::string::npos || checkFilePresence(tableDir, config.getWheelImage());
                    ImGui::TextColored(wheelExists ? greenColor : redColor, "Wheel");

                    ImGui::SameLine();
                    bool tableImageExists = images.find("Table") != std::string::npos || checkFilePresence(tableDir, config.getTableImage());
                    ImGui::TextColored(tableImageExists ? greenColor : redColor, "Table");

                    ImGui::SameLine();
                    bool b2sImageExists = images.find("B2S") != std::string::npos || checkFilePresence(tableDir, config.getBackglassImage());
                    ImGui::TextColored(b2sImageExists ? greenColor : redColor, "B2S");

                    ImGui::SameLine();
                    bool marqueeExists = images.find("Marquee") != std::string::npos || checkFilePresence(tableDir, config.getMarqueeImage());
                    ImGui::TextColored(marqueeExists ? greenColor : redColor, "Marquee");
                }
                ImGui::TableSetColumnIndex(12); {
                    ImVec4 greenColor(0.0f, 1.0f, 0.0f, 1.0f);
                    ImVec4 redColor(1.0f, 0.0f, 0.0f, 1.0f);

                    std::string tableDir = std::filesystem::path(tables[i].filepath).parent_path().string();
                    std::string videos = tables[i].videos;

                    bool tableVideoExists = videos.find("Table") != std::string::npos || checkFilePresence(tableDir, config.getTableVideo());
                    ImGui::TextColored(tableVideoExists ? greenColor : redColor, "Table");

                    ImGui::SameLine();
                    bool b2sVideoExists = videos.find("B2S") != std::string::npos || checkFilePresence(tableDir, config.getBackglassVideo());
                    ImGui::TextColored(b2sVideoExists ? greenColor : redColor, "B2S");

                    ImGui::SameLine();
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