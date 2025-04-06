#include "paths_config.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

// Constructs PathsConfig with default values
PathsConfig::PathsConfig(const std::string& basePath)
    : basePath_(basePath),
      firstRun_(true),
      tablesDir_(prependBasePath("tables")),
      startArgs_(""),
      commandToRun_(prependBasePath("vpinballx")),
      endArgs_(""),
      vpinballXIni_(std::string(std::getenv("HOME") ? std::getenv("HOME") : "") + "/.vpinball/VPinballX.ini") {
    std::filesystem::create_directories(tablesDir_); // Ensure tables dir exists
}

// Prepends basePath_ to relative paths, leaving absolute paths unchanged
std::string PathsConfig::prependBasePath(const std::string& relativePath) const {
    if (relativePath.empty() || std::filesystem::path(relativePath).is_absolute()) {
        return relativePath;
    }
    std::string fullPath = basePath_ + relativePath;
    while (fullPath.find("//") != std::string::npos) {
        fullPath.replace(fullPath.find("//"), 2, "/");
    }
    return fullPath;
}

// Loads VPinballX settings from the config file
void PathsConfig::load(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "PathsConfig: Could not open " << configFile << ", using defaults\n";
        return;
    }

    std::string line, currentSection;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == ';') continue;
        if (line.front() == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));

        if (currentSection == "VPinballX") {
            if (key == "FirstRun") firstRun_ = (value == "true");
            else if (key == "TablesDir") tablesDir_ = prependBasePath(value);
            else if (key == "StartArgs") startArgs_ = value;
            else if (key == "CommandToRun") commandToRun_ = prependBasePath(value);
            else if (key == "EndArgs") endArgs_ = value;
            else if (key == "VPinballXIni") vpinballXIni_ = prependBasePath(value);
        }
    }
    file.close();
    std::filesystem::create_directories(tablesDir_); // Ensure tables dir exists after load
}

// Validates that required paths exist and are usable
bool PathsConfig::arePathsValid() const {
    bool tablePathValid = std::filesystem::exists(tablesDir_) && std::filesystem::is_directory(tablesDir_);
    if (tablePathValid) {
        bool hasVpxFile = false;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(tablesDir_)) {
            if (entry.path().extension() == ".vpx") {
                hasVpxFile = true;
                break;
            }
        }
        tablePathValid = hasVpxFile;
    }

    bool vpxExecutableValid = std::filesystem::exists(commandToRun_) && 
                              (std::filesystem::status(commandToRun_).permissions() & std::filesystem::perms::owner_exec) != std::filesystem::perms::none;

    bool vpinballXIniValid = std::filesystem::exists(vpinballXIni_);

    return tablePathValid && vpxExecutableValid && vpinballXIniValid;
}