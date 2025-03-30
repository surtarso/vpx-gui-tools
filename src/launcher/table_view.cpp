#include "launcher/table_view.h"
#include <filesystem>

TableView::TableView(TableManager* tm, IConfigProvider& config) : tableManager(tm), config(config), selectedTable(-1) {}

void TableView::drawTable(std::vector<TableEntry>& tables) {
    if (ImGui::BeginTable("Tables", 13, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX | 
                          ImGuiTableFlags_Sortable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Year", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Author", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Extra", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("ROM", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("uDMD", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 20.0f);
        ImGui::TableSetupColumn("AltS", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 20.0f);
        ImGui::TableSetupColumn("AltC", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 20.0f);
        ImGui::TableSetupColumn("PUP", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 20.0f);
        ImGui::TableSetupColumn("Music", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 20.0f);
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
                    if (selectedTable == static_cast<int>(i)) selectedTable = -1;
                    else selectedTable = static_cast<int>(i);
                }
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && selectedTable >= 0) {
                    std::string folder = std::filesystem::path(tables[selectedTable].filepath).parent_path().string();
                    std::string cmd = "xdg-open \"" + folder + "\"";
                    system(cmd.c_str());
                }
                if (isSelected && ImGui::IsKeyPressed(ImGuiKey_Enter) && selectedTable >= 0) {
                    std::string cmd = config.getStartArgs() + " \"" + config.getCommandToRun() + "\" " + config.getPlaySubCmd() + " \"" + tables[selectedTable].filepath + "\" " + config.getEndArgs();
                    system(cmd.c_str());
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
                ImGui::TableSetColumnIndex(6); {
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 20.0f);
                    ImGui::Text("%s", tables[i].udmd.c_str());
                    ImGui::PopTextWrapPos();
                }
                ImGui::TableSetColumnIndex(7); {
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 20.0f);
                    ImGui::Text("%s", tables[i].alts.c_str());
                    ImGui::PopTextWrapPos();
                }
                ImGui::TableSetColumnIndex(8); {
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 20.0f);
                    ImGui::Text("%s", tables[i].altc.c_str());
                    ImGui::PopTextWrapPos();
                }
                ImGui::TableSetColumnIndex(9); {
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 20.0f);
                    ImGui::Text("%s", tables[i].pup.c_str());
                    ImGui::PopTextWrapPos();
                }
                ImGui::TableSetColumnIndex(10); {
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 20.0f);
                    ImGui::Text("%s", tables[i].music.c_str());
                    ImGui::PopTextWrapPos();
                }
                ImGui::TableSetColumnIndex(11); ImGui::Text("%s", tables[i].images.c_str());
                ImGui::TableSetColumnIndex(12); ImGui::Text("%s", tables[i].videos.c_str());
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}