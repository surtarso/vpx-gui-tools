#include "launcher/launcher.h"
#include "imgui.h"
#include <filesystem>
#include <cstdlib>

Launcher::Launcher(const std::string& tablesDir, const std::string& startArgs, const std::string& commandToRun,
                   const std::string& endArgs, const std::string& vpinballXIni)
    : tablesDir(tablesDir), startArgs(startArgs), commandToRun(commandToRun), endArgs(endArgs), vpinballXIni(vpinballXIni) {}

void Launcher::draw(std::vector<TableEntry>& tables, bool& editingIni) {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
    ImGui::Begin("VPX GUI Tools", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImGui::Text("Table(s) found: %zu", tables.size());
    char searchBuf[256] = "";
    if (ImGui::InputText("Search", searchBuf, sizeof(searchBuf))) {
        searchQuery = searchBuf;
    }

    if (ImGui::BeginTable("Tables", 12, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Year");
        ImGui::TableSetupColumn("Brand");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Extra Files");
        ImGui::TableSetupColumn("ROM");
        ImGui::TableSetupColumn("uDMD");
        ImGui::TableSetupColumn("AltS");
        ImGui::TableSetupColumn("AltC");
        ImGui::TableSetupColumn("PUP");
        ImGui::TableSetupColumn("Music");
        ImGui::TableSetupColumn("Images");
        ImGui::TableSetupColumn("Videos");
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < tables.size(); ++i) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", tables[i].year.c_str());
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
            if (ImGui::IsItemClicked()) selectedTable = static_cast<int>(i);
        }
        ImGui::EndTable();
    }

    if (ImGui::Button("Settings")) {
        editingIni = true;
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
    if (ImGui::Button("Launch") && selectedTable >= 0) {
        launchTable(tables[selectedTable].filepath);
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