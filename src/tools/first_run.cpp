#include "first_run.h"
#include <filesystem>
#include <cstring>
#include <cstdlib> // For getenv()

// Constructor for FirstRunDialog, initializes paths from the config provider
FirstRunDialog::FirstRunDialog(IConfigProvider& config) 
    : config(config), 
      tablePath(config.getTablesDir()),        // Path to the directory containing .vpx table files
      vpxExecutable(config.getCommandToRun()), // Path to the VPinballX executable
      vpinballXIni(config.getVPinballXIni()),  // Path to the VPinballX.ini configuration file
      pathsValid(false) {                      // Flag to track if all paths are valid
    validatePaths(); // Validate paths on initialization
}

// Validates the paths for the table directory, VPinballX executable, and VPinballX.ini
// Returns true if all paths are valid, false otherwise
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

    // Log the validation results for debugging
    LOG_DEBUG("Table path valid: " << tablePathValid);
    LOG_DEBUG("VPX executable valid: " << vpxExecutableValid);
    LOG_DEBUG("VPinballX.ini valid: " << vpinballXIniValid);

    // Update the pathsValid flag based on the validation results
    pathsValid = tablePathValid && vpxExecutableValid && vpinballXIniValid;
    return pathsValid;
}

// Displays the first-run dialog to configure required paths
// Returns true if the dialog is still active, false if it should be closed
bool FirstRunDialog::show() {
    // Open the first-run dialog as a modal popup
    ImGui::OpenPopup("First Run Setup");
    bool dialogActive = true;

    // Begin the modal popup with auto-resize enabled
    if (ImGui::BeginPopupModal("First Run Setup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        // Display a welcome message in green
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Welcome to VPX GUI Tools!");
        ImGui::TextWrapped("Please set the required paths to get started.");
        ImGui::Separator();

        // Helper function to get a safe initial path for tinyfiledialogs
        // Returns the input path if valid, otherwise falls back to the user's home directory or a system default
        auto getSafeInitialPath = [](const std::string& path) -> std::string {
            // Check if the path exists and is valid
            if (!path.empty() && std::filesystem::exists(path)) {
                return path;
            }
            // Fallback to the user's home directory
            const char* home = getenv("HOME"); // Linux/macOS
            if (!home) home = getenv("USERPROFILE"); // Windows
            if (home) return home;
            // Last resort: use a system default
            #ifdef _WIN32
            return "C:\\";
            #else
            return "/";
            #endif
        };

        // --- Table Path Section ---
        // Validate the table path and display an error if invalid
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
        // Display the table path in a read-only input field
        char tablePathBuf[1024];
        strncpy(tablePathBuf, tablePath.c_str(), sizeof(tablePathBuf));
        tablePathBuf[sizeof(tablePathBuf) - 1] = '\0'; // Ensure null termination
        ImGui::InputText("Table Root Folder", tablePathBuf, sizeof(tablePathBuf), ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        // Button to open a folder selection dialog for the table path
        if (ImGui::Button("Browse##TablePath")) {
            std::string initialPath = getSafeInitialPath(tablePath);
            const char* path = tinyfd_selectFolderDialog("Select Table Root Folder", initialPath.c_str());
            if (path) {
                tablePath = path;
                validatePaths();
            }
        }

        // --- VPinballX Executable Section ---
        // Validate the VPinballX executable and display an error if invalid
        bool vpxExecutableValid = std::filesystem::exists(vpxExecutable) && 
                                 (std::filesystem::status(vpxExecutable).permissions() & std::filesystem::perms::owner_exec) != std::filesystem::perms::none;
        if (!vpxExecutableValid) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::Text("VPinballX executable is invalid or not executable!");
            ImGui::PopStyleColor();
        }
        // Display the VPinballX executable path in a read-only input field
        char vpxExecutableBuf[1024];
        strncpy(vpxExecutableBuf, vpxExecutable.c_str(), sizeof(vpxExecutableBuf));
        vpxExecutableBuf[sizeof(vpxExecutableBuf) - 1] = '\0';
        ImGui::InputText("VPinballX Executable", vpxExecutableBuf, sizeof(vpxExecutableBuf), ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        // Button to open a file selection dialog for the VPinballX executable
        if (ImGui::Button("Browse##VpxExecutable")) {
            std::string initialPath = getSafeInitialPath(vpxExecutable);
            // No filter patterns for executable, so pass nullptr
            const char* path = tinyfd_openFileDialog("Select VPinballX Executable", initialPath.c_str(), 0, nullptr, nullptr, 0);
            if (path) {
                vpxExecutable = path;
                validatePaths();
            }
        }

        // --- VPinballX.ini Section ---
        // Validate the VPinballX.ini file and display an error if invalid
        bool vpinballXIniValid = std::filesystem::exists(vpinballXIni);
        if (!vpinballXIniValid) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::Text("VPinballX.ini is invalid!");
            ImGui::PopStyleColor();
        }
        // Display the VPinballX.ini path in a read-only input field
        char vpinballXIniBuf[1024];
        strncpy(vpinballXIniBuf, vpinballXIni.c_str(), sizeof(vpinballXIniBuf));
        vpinballXIniBuf[sizeof(vpinballXIniBuf) - 1] = '\0';
        ImGui::InputText("VPinballX.ini", vpinballXIniBuf, sizeof(vpinballXIniBuf), ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        // Button to open a file selection dialog for the VPinballX.ini file
        if (ImGui::Button("Browse##VPinballXIni")) {
            std::string initialPath = getSafeInitialPath(vpinballXIni);
            // Define filter patterns as a static array to avoid temporary array issues
            static const char* filterPatterns[] = {"*.ini", nullptr};
            const char* path = tinyfd_openFileDialog("Select VPinballX.ini", initialPath.c_str(), 1, filterPatterns, "INI files", 0);
            if (path) {
                vpinballXIni = path;
                validatePaths();
            }
        }

        ImGui::Separator();

        // --- Save and Exit Buttons ---
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
        // Exit button to close the application
        if (ImGui::Button("Exit")) {
            dialogActive = false;
            exit(0); // Exit the application if the user doesn't want to configure
        }

        ImGui::EndPopup();
    }

    return dialogActive;
}