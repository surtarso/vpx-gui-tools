#include "media_config.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

// Constructs MediaConfig with default values
MediaConfig::MediaConfig(const std::string& basePath)
    : basePath_(basePath),
      romPath_("/pinmame/roms"),
      altSoundPath_("/pinmame/altsound"),
      altColorPath_("/pinmame/AltColor"),
      musicPath_("/music"),
      pupPackPath_("/pupvideos"),
      wheelImage_(prependBasePath("/images/wheel.png")),
      tableImage_(prependBasePath("/images/table.png")),
      backglassImage_(prependBasePath("/images/backglass.png")),
      marqueeImage_(prependBasePath("/images/marquee.png")),
      tableVideo_(prependBasePath("/video/table.mp4")),
      backglassVideo_(prependBasePath("/video/backglass.mp4")),
      dmdVideo_(prependBasePath("/video/dmd.mp4")) {}

// Prepends basePath_ to relative paths, leaving absolute paths unchanged
std::string MediaConfig::prependBasePath(const std::string& relativePath) const {
    if (relativePath.empty() || std::filesystem::path(relativePath).is_absolute()) {
        return relativePath;
    }
    std::string fullPath = basePath_ + relativePath;
    while (fullPath.find("//") != std::string::npos) {
        fullPath.replace(fullPath.find("//"), 2, "/");
    }
    return fullPath;
}

// Loads media and extra folder settings from the config file
void MediaConfig::load(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "MediaConfig: Could not open " << configFile << ", using defaults\n";
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

        if (currentSection == "Images") {
            if (key == "WheelImage") wheelImage_ = value;
            else if (key == "TableImage") tableImage_ = value;
            else if (key == "BackglassImage") backglassImage_ = value;
            else if (key == "MarqueeImage") marqueeImage_ = value;
        }
        else if (currentSection == "Videos") {
            if (key == "TableVideo") tableVideo_ = value;
            else if (key == "BackglassVideo") backglassVideo_ = value;
            else if (key == "DMDVideo") dmdVideo_ = value;
        }
        else if (currentSection == "ExtraFolders") {
            if (key == "ROMPath") romPath_ = value;
            else if (key == "AltSoundPath") altSoundPath_ = value;
            else if (key == "AltColorPath") altColorPath_ = value;
            else if (key == "MusicPath") musicPath_ = value;
            else if (key == "PUPPackPath") pupPackPath_ = value;
        }
    }
    file.close();
}