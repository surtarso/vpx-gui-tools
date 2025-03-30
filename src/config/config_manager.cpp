#include "config/config_manager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

ConfigManager::ConfigManager(const std::string& basePath) : basePath(basePath) {
    loadSettings();
}

std::string ConfigManager::prependBasePath(const std::string& relativePath) const {
    if (relativePath.empty() || std::filesystem::path(relativePath).is_absolute()) {
        return relativePath;
    }
    return basePath + relativePath;
}

void ConfigManager::loadSettings() {
    const char* homeDir = std::getenv("HOME");
    std::string defaultIniPath = std::string(homeDir ? homeDir : "") + "/.vpinball/VPinballX.ini";
    std::string configFile = prependBasePath("resources/settings.ini");

    if (!std::filesystem::exists(configFile)) {
        std::ofstream out(configFile);
        out << "[VPinballX]\n"
            << "TablesDir=tables\n"
            << "StartArgs=\n"
            << "CommandToRun=vpinballx\n"
            << "EndArgs=\n"
            << "VPinballXIni=" << defaultIniPath << "\n"
            << "\n[LauncherWindow]\n"
            << "WindowWidth=1024\n"
            << "WindowHeight=768\n"
            << "\n[Images]\n"
            << "WheelImage=/images/wheel.png\n"
            << "TableImage=/images/table.png\n"
            << "BackglassImage=/images/backglass.png\n"
            << "MarqueeImage=/images/marquee.png\n"
            << "\n[Videos]\n"
            << "TableVideo=/video/table.mp4\n"
            << "BackglassVideo=/video/backglass.mp4\n"
            << "DMDVideo=/video/dmd.mp4\n"
            << "\n[ExtraFolders]\n"
            << "ROMPath=/pinmame/roms\n"
            << "AltSoundPath=/pinmame/altsound\n"
            << "AltColorPath=/pinmame/AltColor\n"
            << "MusicPath=/music\n"
            << "PUPPackPath=/pupvideos\n"
            << "\n[Tools]\n"
            << "FallbackEditor=code\n"
            << "VpxTool=resources/vpxtool\n"
            << "\n[Internal]\n"
            << "VpxtoolIndexFile=vpxtool_index.json\n"
            << "IndexerSubCmd=index -r\n"
            << "DiffSubCmd=diff\n"
            << "RomSubCmd=romname\n"
            << "VbsSubCmd=extractvbs\n"
            << "PlaySubCmd=-Play\n";
        out.close();
    }

    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "Could not open " << configFile << std::endl;
        tablesDir = prependBasePath("tables");
        startArgs = "";
        commandToRun = "vpinballx";
        endArgs = "";
        vpinballXIni = defaultIniPath;
        fallbackEditor = "code";
        vpxTool = prependBasePath("resources/vpxtool");
        vbsSubCmd = "extractvbs";
        playSubCmd = "-Play";
        windowWidth = 1024;
        windowHeight = 768;
        wheelImage = prependBasePath("/images/wheel.png");
        tableImage = prependBasePath("/images/table.png");
        backglassImage = prependBasePath("/images/backglass.png");
        marqueeImage = prependBasePath("/images/marquee.png");
        tableVideo = prependBasePath("/video/table.mp4");
        backglassVideo = prependBasePath("/video/backglass.mp4");
        dmdVideo = prependBasePath("/video/dmd.mp4");
        romPath = "/pinmame/roms";
        altSoundPath = "/pinmame/altsound";
        altColorPath = "/pinmame/AltColor";
        musicPath = "/music";
        pupPackPath = "/pupvideos";
        vpxtoolIndexFile = "vpxtool_index.json";
        indexerSubCmd = "index -r";
        diffSubCmd = "diff";
        romSubCmd = "romname";
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
            if (key == "TablesDir") tablesDir = prependBasePath(value);
            else if (key == "StartArgs") startArgs = value;
            else if (key == "CommandToRun") commandToRun = prependBasePath(value);
            else if (key == "EndArgs") endArgs = value;
            else if (key == "VPinballXIni") vpinballXIni = prependBasePath(value);
        } 
        else if (currentSection == "Tools") {
            if (key == "FallbackEditor") fallbackEditor = value;
            else if (key == "VpxTool") vpxTool = prependBasePath(value);
        } 
        else if (currentSection == "LauncherWindow") {
            if (key == "WindowWidth") windowWidth = std::stoi(value);
            else if (key == "WindowHeight") windowHeight = std::stoi(value);
        } 
        else if (currentSection == "Images") {
            if (key == "WheelImage") wheelImage = value; // Relative to table folder
            else if (key == "TableImage") tableImage = value;
            else if (key == "BackglassImage") backglassImage = value;
            else if (key == "MarqueeImage") marqueeImage = value;
        } 
        else if (currentSection == "Videos") {
            if (key == "TableVideo") tableVideo = value;
            else if (key == "BackglassVideo") backglassVideo = value;
            else if (key == "DMDVideo") dmdVideo = value;
        } 
        else if (currentSection == "ExtraFolders") {
            if (key == "ROMPath") romPath = value;
            else if (key == "AltSoundPath") altSoundPath = value;
            else if (key == "AltColorPath") altColorPath = value;
            else if (key == "MusicPath") musicPath = value;
            else if (key == "PUPPackPath") pupPackPath = value;
        }
        else if (currentSection == "Internal") {
            if (key == "VpxtoolIndexFile") vpxtoolIndexFile = value;
            else if (key == "IndexerSubCmd") indexerSubCmd = value;
            else if (key == "DiffSubCmd") diffSubCmd = value;
            else if (key == "RomSubCmd") romSubCmd = value;
            else if (key == "VbsSubCmd") vbsSubCmd = value;
            else if (key == "PlaySubCmd") playSubCmd = value;
        }
    }
    file.close();
}