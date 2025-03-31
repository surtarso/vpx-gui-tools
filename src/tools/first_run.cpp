#include "first_run.h"
#include <filesystem>
#include <cstring>

FirstRunDialog::FirstRunDialog(IConfigProvider& config) 
    : config(config), 
      tablePath(config.getTablesDir()),
      vpxExecutable(config.getCommandToRun()),
      vpinballXIni(config.getVPinballXIni()),
      pathsValid(false) {
    validatePaths();
}

bool FirstRunDialog::validatePaths() {
    // Validate table folder: must exist, be a directory, and contain at least one .vpx file
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

    // Validate VPinballX executable: must exist and be executable
    bool vpxExecutableValid = std::filesystem::exists(vpxExecutable) && 
                             (std::filesystem::status(vpxExecutable).permissions() & std::filesystem::perms::owner_exec) != std::filesystem::perms::none;

    // Validate VPinballX.ini: must exist
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

    if (ImGui::BeginPopupModal("First Run Setup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Welcome to VPX GUI Tools!");
        ImGui::TextWrapped("Please set the required paths to get started.");
        ImGui::Separator();

        // Table Path
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
        if (!tablePathValid) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::Text("Table path is invalid or contains no .vpx files!");
            ImGui::PopStyleColor();
        }
        // Use a char buffer for ImGui::InputText
        char tablePathBuf[1024];
        strncpy(tablePathBuf, tablePath.c_str(), sizeof(tablePathBuf));
        tablePathBuf[sizeof(tablePathBuf) - 1] = '\0'; // Ensure null termination
        ImGui::InputText("Table Root Folder", tablePathBuf, sizeof(tablePathBuf), ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("Browse##TablePath")) {
            const char* path = tinyfd_selectFolderDialog("Select Table Root Folder", "");
            if (path) {
                tablePath = path;
                validatePaths();
            }
        }

        // VPinballX Executable
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
            // No filter patterns for executable, so pass nullptr
            const char* path = tinyfd_openFileDialog("Select VPinballX Executable", "", 0, nullptr, nullptr, 0);
            if (path) {
                vpxExecutable = path;
                validatePaths();
            }
        }

        // VPinballX.ini
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
            // Define filter patterns as a static array to avoid temporary array issues
            static const char* filterPatterns[] = {"*.ini", nullptr};
            const char* path = tinyfd_openFileDialog("Select VPinballX.ini", "", 1, filterPatterns, "INI files", 0);
            if (path) {
                vpinballXIni = path;
                validatePaths();
            }
        }

        ImGui::Separator();

        // Save button (enabled only if paths are valid)
        if (!pathsValid) {
            ImGui::BeginDisabled();
        }
        if (ImGui::Button("Save and Continue")) {
            // Update the config with the new paths
            config.setTablesDir(tablePath);
            config.setCommandToRun(vpxExecutable);
            config.setVPinballXIni(vpinballXIni);
            config.setFirstRun(false); // Mark first run as complete
            config.save();
            dialogActive = false; // Close the dialog
        }
        if (!pathsValid) {
            ImGui::EndDisabled();
        }

        ImGui::SameLine();
        if (ImGui::Button("Exit")) {
            dialogActive = false;
            exit(0); // Exit the application if the user doesn't want to configure
        }

        ImGui::EndPopup();
    }

    return dialogActive;
}