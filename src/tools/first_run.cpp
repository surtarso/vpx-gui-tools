#include "tools/first_run.h"
#include <ImGuiFileDialog.h>
#include <filesystem>
#include <cstring>
#include <cstdlib>
#include <iostream>

FirstRunDialog::FirstRunDialog(IConfigProvider& config) 
    : config(config), 
      tablePath(config.getTablesDir()),
      vpxExecutable(config.getCommandToRun()),
      vpinballXIni(config.getVPinballXIni()),
      pathsValid(false) {
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByFullName, "((VPinballX.*))", ImVec4(0.0f, 1.0f, 0.0f, 0.9f));
    validatePaths();
}

bool FirstRunDialog::validatePaths() {
    bool tablePathValid = std::filesystem::exists(tablePath) && std::filesystem::is_directory(tablePath);
    if (tablePathValid) {
        bool hasVpxFile = false;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(tablePath)) {
            if (entry.path().extension() == ".vpx") {
                hasVpxFile = true;
                break;
            }
        }
        tablePathValid = hasVpxFile;
    }

    bool vpxExecutableValid = std::filesystem::exists(vpxExecutable) && 
                             (std::filesystem::status(vpxExecutable).permissions() & std::filesystem::perms::owner_exec) != std::filesystem::perms::none;

    bool vpinballXIniValid = std::filesystem::exists(vpinballXIni);

    LOG_DEBUG("Table path valid: " << tablePathValid);
    LOG_DEBUG("VPX executable valid: " << vpxExecutableValid);
    LOG_DEBUG("VPinballX.ini valid: " << vpinballXIniValid);

    pathsValid = tablePathValid && vpxExecutableValid && vpinballXIniValid;
    return pathsValid;
}

bool FirstRunDialog::show() {
    ImGui::OpenPopup("First Run Setup");
    bool dialogActive = true;

    float dpiScale = ImGui::GetIO().FontGlobalScale;
    if (dpiScale <= 0.0f) dpiScale = 1.0f;

    if (ImGui::BeginPopupModal("First Run Setup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Welcome to VPX GUI Tools!");
        ImGui::TextWrapped("Please set the required paths to get started.");
        ImGui::Separator();

        auto getSafeInitialPath = [](const std::string& path) -> std::string {
            if (!path.empty() && std::filesystem::exists(path)) return path;
            const char* home = getenv("HOME");
            if (!home) home = getenv("USERPROFILE");
            if (home) return home;
            #ifdef _WIN32
            return "C:\\";
            #else
            return "/";
            #endif
        };

        ImVec2 displaySize = ImGui::GetIO().DisplaySize;
        ImVec2 baseSize = ImVec2(782.0f, 505.0f);
        ImVec2 desiredSize = ImVec2(baseSize.x * dpiScale, baseSize.y * dpiScale);
        desiredSize.x = std::min(desiredSize.x, displaySize.x * 0.8f);
        desiredSize.y = std::min(desiredSize.y, displaySize.y * 0.8f);
        ImVec2 maxSize = displaySize;

        bool tablePathValid = std::filesystem::exists(tablePath) && std::filesystem::is_directory(tablePath) && std::filesystem::directory_iterator(tablePath) != std::filesystem::directory_iterator();
        if (!tablePathValid) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::Text("Table path is invalid or contains no .vpx files!");
            ImGui::PopStyleColor();
        }
        char tablePathBuf[1024];
        strncpy(tablePathBuf, tablePath.c_str(), sizeof(tablePathBuf));
        tablePathBuf[sizeof(tablePathBuf) - 1] = '\0';
        ImGui::InputText("Table Root Folder", tablePathBuf, sizeof(tablePathBuf), ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("Browse##TablePath")) {
            std::string initialPath = getSafeInitialPath(tablePath);
            IGFD::FileDialogConfig config;
            config.path = initialPath;
            config.flags = ImGuiFileDialogFlags_Modal;
            ImGuiFileDialog::Instance()->OpenDialog("ChooseTableDirDlg", "Select Table Root Folder", nullptr, config);
        }
        if (ImGuiFileDialog::Instance()->Display("ChooseTableDirDlg", ImGuiWindowFlags_NoCollapse, desiredSize, maxSize)) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                tablePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                validatePaths();
            }
            ImGuiFileDialog::Instance()->Close();
        }

        bool vpxExecutableValid = std::filesystem::exists(vpxExecutable) && 
                                (std::filesystem::status(vpxExecutable).permissions() & std::filesystem::perms::owner_exec) != std::filesystem::perms::none;
        if (!vpxExecutableValid) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::Text("VPinballX executable is invalid or not executable!");
            ImGui::PopStyleColor();
        }
        char vpxExecutableBuf[1024];
        strncpy(vpxExecutableBuf, vpxExecutable.c_str(), sizeof(vpxExecutableBuf));
        vpxExecutableBuf[sizeof(vpxExecutableBuf) - 1] = '\0';
        ImGui::InputText("VPinballX Executable", vpxExecutableBuf, sizeof(vpxExecutableBuf), ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("Browse##VpxExecutable")) {
            std::string initialPath = getSafeInitialPath(vpxExecutable);
            IGFD::FileDialogConfig config;
            config.path = initialPath;
            config.flags = ImGuiFileDialogFlags_Modal;
            ImGuiFileDialog::Instance()->OpenDialog("ChooseVpxExeDlg", "Select VPinballX Executable", "((.*))", config);
        }
        if (ImGuiFileDialog::Instance()->Display("ChooseVpxExeDlg", ImGuiWindowFlags_NoCollapse, desiredSize, maxSize)) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                vpxExecutable = ImGuiFileDialog::Instance()->GetFilePathName();
                validatePaths();
            }
            ImGuiFileDialog::Instance()->Close();
        }

        bool vpinballXIniValid = std::filesystem::exists(vpinballXIni);
        if (!vpinballXIniValid) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::Text("VPinballX.ini is invalid!");
            ImGui::PopStyleColor();
        }
        char vpinballXIniBuf[1024];
        strncpy(vpinballXIniBuf, vpinballXIni.c_str(), sizeof(vpinballXIniBuf));
        vpinballXIniBuf[sizeof(vpinballXIniBuf) - 1] = '\0';
        ImGui::InputText("VPinballX.ini", vpinballXIniBuf, sizeof(vpinballXIniBuf), ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("Browse##VPinballXIni")) {
            std::string initialPath = getSafeInitialPath(vpinballXIni);
            IGFD::FileDialogConfig config;
            config.path = initialPath;
            config.flags = ImGuiFileDialogFlags_Modal;
            ImGuiFileDialog::Instance()->OpenDialog("ChooseIniDlg", "Select VPinballX.ini", ".ini", config);
        }
        if (ImGuiFileDialog::Instance()->Display("ChooseIniDlg", ImGuiWindowFlags_NoCollapse, desiredSize, maxSize)) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                vpinballXIni = ImGuiFileDialog::Instance()->GetFilePathName();
                validatePaths();
            }
            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::Separator();

        if (!pathsValid) ImGui::BeginDisabled();
        if (ImGui::Button("Save and Continue")) {
            config.setTablesDir(tablePath);
            config.setCommandToRun(vpxExecutable);
            config.setVPinballXIni(vpinballXIni);
            config.setFirstRun(false);
            config.save();
            dialogActive = false;
        }
        if (!pathsValid) ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Exit")) {
            dialogActive = false;
            exit(0);
        }

        ImGui::EndPopup();
    }

    return dialogActive;
}