#include "launcher/table_view.h"
#include <filesystem>
#include <sstream>
#include <vector>

TableView::TableView(TableManager* tm, IConfigProvider& config) : tableManager(tm), config(config), selectedTable(-1) {}

void TableView::drawTable(std::vector<TableEntry>& tables) {
    float dpiScale = ImGui::GetIO().FontGlobalScale;
    if (dpiScale <= 0.0f) dpiScale = 1.0f;

    // Adjust padding for a compact look, scaled by DPI
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.0f * dpiScale, 1.0f * dpiScale));

    const char* tableName = "Tables";

    // Calculate the exact widths of fixed-content columns using ImGui::CalcTextSize()
    // These columns have predictable content, so we can ensure they never clip
    ImVec2 yearSize = ImGui::CalcTextSize("2024"); // Year is always 4 digits
    ImVec2 extraFilesSize = ImGui::CalcTextSize("INI VBS B2S"); // Extra Files is always "INI VBS B2S"
    ImVec2 imagesSize = ImGui::CalcTextSize("Wheel Table B2S Marquee"); // Images is always "Wheel Table B2S Marquee"
    ImVec2 videosSize = ImGui::CalcTextSize("Table B2S DMD"); // Videos is always "Table B2S DMD"

    // Calculate the width of icon columns (UltraDMD, AltSound, AltColor, PUPMedia, Music) based on their symbols
    ImVec2 ultraDmdSize = ImGui::CalcTextSize(u8"✪"); // UltraDMD symbol
    ImVec2 altSoundSize = ImGui::CalcTextSize(u8"♪"); // AltSound symbol
    ImVec2 altColorSize = ImGui::CalcTextSize(u8"☀"); // AltColor symbol
    ImVec2 pupMediaSize = ImGui::CalcTextSize(u8"▣"); // PUPMedia symbol
    ImVec2 musicSize = ImGui::CalcTextSize(u8"♫"); // Music symbol
    // Use the maximum width of the icon columns to ensure they all fit
    float iconWidth = std::max({ultraDmdSize.x, altSoundSize.x, altColorSize.x, pupMediaSize.x, musicSize.x});

    // Add padding to the calculated widths to ensure no clipping (e.g., 20% extra for safety)
    float padding = 1.2f;
    float yearWidth = yearSize.x * padding;
    float extraFilesWidth = extraFilesSize.x * padding;
    float imagesWidth = imagesSize.x * padding;
    float videosWidth = videosSize.x * padding;
    float adjustedIconWidth = iconWidth * padding;

    // Define base widths for other columns (scaled by dpiScale)
    // These are starting points and will be adjusted by widthScale if the table is too wide
    float authorBaseWidth = 60.0f * dpiScale;
    float versionBaseWidth = 35.0f * dpiScale;
    float romBaseWidth = 65.0f * dpiScale;

    // Calculate the total minimum width required for fixed-content columns (not scaled down)
    float totalMinWidth = yearWidth + extraFilesWidth + imagesWidth + videosWidth + (adjustedIconWidth * 5);

    // Calculate the total width of variable-content columns (which can be scaled down)
    float totalVariableWidth = authorBaseWidth + versionBaseWidth + romBaseWidth;

    // Calculate the total fixed width (minimum width + variable width)
    float totalFixedWidth = totalMinWidth + totalVariableWidth;

    // Calculate the scaling factor to fit the table within the window
    float windowWidth = ImGui::GetWindowWidth();
    float widthScale = 1.0f;
    if (totalFixedWidth > windowWidth) {
        // Only scale down the variable-content columns (Author, Version, ROM)
        // Fixed-content columns (Year, Extra Files, Images, Videos, Icons) should not be scaled below their calculated width
        float excessWidth = totalFixedWidth - windowWidth * 0.8f;
        if (excessWidth > 0 && totalVariableWidth > 0) {
            widthScale = (totalVariableWidth - excessWidth) / totalVariableWidth;
            if (widthScale < 0.1f) widthScale = 0.1f; // Prevent excessive shrinking
        }
    }

    // Apply the widthScale only to variable-content columns
    float adjustedAuthorWidth = authorBaseWidth * widthScale;
    float adjustedVersionWidth = versionBaseWidth * widthScale;
    float adjustedRomWidth = romBaseWidth * widthScale;

    // Log the calculated widths for debugging
    LOG_DEBUG("DPI Scale: " << dpiScale << ", Window Width: " << windowWidth);
    LOG_DEBUG("Year Width: " << yearWidth << ", Extra Files Width: " << extraFilesWidth);
    LOG_DEBUG("Images Width: " << imagesWidth << ", Videos Width: " << videosWidth);
    LOG_DEBUG("Icon Width: " << adjustedIconWidth);
    LOG_DEBUG("Author Width: " << adjustedAuthorWidth << ", Version Width: " << adjustedVersionWidth << ", ROM Width: " << adjustedRomWidth);
    LOG_DEBUG("Total Fixed Width: " << totalFixedWidth << ", Width Scale: " << widthScale);

    if (ImGui::BeginTable(tableName, 13, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | 
                          ImGuiTableFlags_ScrollX | ImGuiTableFlags_Sortable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Resizable)) {
        // Set up columns with dynamically calculated widths
        // Remove ImGuiTableColumnFlags_NoResize from icon columns to allow resizing
        ImGui::TableSetupColumn("Year", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, yearWidth);
        ImGui::TableSetupColumn("Author", ImGuiTableColumnFlags_WidthFixed, adjustedAuthorWidth);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch); // Stretch to fill remaining space
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

        // Log column widths after user resizing for debugging
        if (ImGui::GetIO().MouseClicked[0]) { // Log on mouse click to avoid spamming
            LOG_DEBUG("Column Widths After Resizing:");
            for (int i = 0; i < 13; ++i) {
                LOG_DEBUG("Column " << i << " Width: " << ImGui::TableGetColumnWidth(i));
            }
        }

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
                    bool marqueeExists = images.find("Marquee") != std::string::npos || checkFilePresence(tableDir, config

.getMarqueeImage());
                    ImGui::TextColored(marqueeExists ? greenColor : redColor, "Marquee");
                }
                ImGui::TableSetColumnIndex(12); {
                    // Always show "Table B2S DMD" with color coding
                    // Colors: Green - Green (found), Red (not found)
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