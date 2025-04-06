#include "window_config.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Constructs WindowConfig with default values
WindowConfig::WindowConfig()
    : windowWidth_(1024),
      windowHeight_(768),
      enableDPIAwareness_(true),
      dpiScaleFactor_(1.0f) {}

// Loads window and DPI settings from the config file
void WindowConfig::load(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "WindowConfig: Could not open " << configFile << ", using defaults\n";
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

        if (currentSection == "LauncherWindow") {
            if (key == "WindowWidth") windowWidth_ = std::stoi(value);
            else if (key == "WindowHeight") windowHeight_ = std::stoi(value);
            else if (key == "EnableDPIAwareness") enableDPIAwareness_ = (value == "true");
            else if (key == "DPIScaleFactor") dpiScaleFactor_ = std::stof(value);
        }
    }
    file.close();
}