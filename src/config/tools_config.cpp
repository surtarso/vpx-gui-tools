#include "tools_config.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

// Constructs ToolsConfig with default values
ToolsConfig::ToolsConfig(const std::string& basePath)
    : basePath_(basePath),
      fallbackEditor_("code"),
      vpxTool_(prependBasePath("resources/vpxtool")),
      vbsSubCmd_("extractvbs"),
      playSubCmd_("-Play"),
      vpxtoolIndexFile_("vpxtool_index.json"),
      indexerSubCmd_("index -r"),
      diffSubCmd_("diff"),
      romSubCmd_("romname"),
      imGuiConf_(prependBasePath("resources/imgui.ini")) {
    std::filesystem::create_directories(basePath_ + "resources"); // Ensure resources dir exists
}

// Prepends basePath_ to relative paths, leaving absolute paths unchanged
std::string ToolsConfig::prependBasePath(const std::string& relativePath) const {
    if (relativePath.empty() || std::filesystem::path(relativePath).is_absolute()) {
        return relativePath;
    }
    std::string fullPath = basePath_ + relativePath;
    while (fullPath.find("//") != std::string::npos) {
        fullPath.replace(fullPath.find("//"), 2, "/");
    }
    return fullPath;
}

// Loads tool-related settings from the config file
void ToolsConfig::load(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "ToolsConfig: Could not open " << configFile << ", using defaults\n";
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

        if (currentSection == "Tools") {
            if (key == "FallbackEditor") fallbackEditor_ = value;
            else if (key == "VpxTool") vpxTool_ = prependBasePath(value);
        }
        else if (currentSection == "Internal") {
            if (key == "VpxtoolIndexFile") vpxtoolIndexFile_ = value;
            else if (key == "IndexerSubCmd") indexerSubCmd_ = value;
            else if (key == "DiffSubCmd") diffSubCmd_ = value;
            else if (key == "RomSubCmd") romSubCmd_ = value;
            else if (key == "VbsSubCmd") vbsSubCmd_ = value;
            else if (key == "PlaySubCmd") playSubCmd_ = value;
            else if (key == "ImGuiConf") imGuiConf_ = prependBasePath(value);
        }
    }
    file.close();
}