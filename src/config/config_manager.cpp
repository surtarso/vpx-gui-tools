#include "config_manager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

ConfigManager::ConfigManager(const std::string& basePath) : basePath(basePath), firstRun(true) {
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
            << "FirstRun=true\n"
            << "TablesDir=tables\n"
            << "StartArgs=\n"
            << "CommandToRun=vpinballx\n"
            << "EndArgs=\n"
            << "VPinballXIni=" << defaultIniPath << "\n"
            << "\n[LauncherWindow]\n"
            << "EnableDPIAwareness=true\n" // Default DPI awareness on
            << "DPIScaleFactor=1.0\n"     // Default no scaling
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
            << "PlaySubCmd=-Play\n"
            << "ImGuiConf=resources/imgui.ini\n";
        out.close();
    }

    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "Could not open " << configFile << std::endl;
        firstRun = true;
        tablesDir = prependBasePath("tables");
        startArgs = "";
        commandToRun = "vpinballx";
        endArgs = "";
        vpinballXIni = defaultIniPath;
        enableDPIAwareness = true; // Default if file fails
        dpiScaleFactor = 1.0f;
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
        imGuiConf = prependBasePath("resources/imgui.ini");
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
            if (key == "FirstRun") firstRun = (value == "true");
            else if (key == "TablesDir") tablesDir = prependBasePath(value);
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
            else if (key == "EnableDPIAwareness") enableDPIAwareness = (value == "true");
            else if (key == "DPIScaleFactor") dpiScaleFactor = std::stof(value);
        } 
        else if (currentSection == "Images") {
            if (key == "WheelImage") wheelImage = value;
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
            else if (key == "ImGuiConf") imGuiConf = prependBasePath(value);
        }
    }
    file.close();
}

void ConfigManager::save() {
    std::string configFile = prependBasePath("resources/settings.ini");
    std::ifstream inFile(configFile);
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    inFile.close();

    std::string content = buffer.str();
    std::string newContent;
    bool inVPinballXSection = false;
    bool inLauncherWindowSection = false;
    bool firstRunWritten = false;
    bool dpiAwarenessWritten = false;
    bool dpiScaleWritten = false;
    bool windowWidthWritten = false;
    bool windowHeightWritten = false;

    std::istringstream iss(content);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.empty() || line[0] == ';') {
            newContent += line + "\n";
            continue;
        }
        if (line.front() == '[' && line.back() == ']') {
            // Before leaving a section, write any missing keys
            if (inVPinballXSection) {
                if (!firstRunWritten) {
                    newContent += "FirstRun=" + std::string(firstRun ? "true" : "false") + "\n";
                }
            }
            if (inLauncherWindowSection) {
                if (!dpiAwarenessWritten) {
                    newContent += "EnableDPIAwareness=" + std::string(enableDPIAwareness ? "true" : "false") + "\n";
                }
                if (!dpiScaleWritten) {
                    newContent += "DPIScaleFactor=" + std::to_string(dpiScaleFactor) + "\n";
                }
                if (!windowWidthWritten) {
                    newContent += "WindowWidth=" + std::to_string(windowWidth) + "\n";
                }
                if (!windowHeightWritten) {
                    newContent += "WindowHeight=" + std::to_string(windowHeight) + "\n";
                }
            }
            inVPinballXSection = (line == "[VPinballX]");
            inLauncherWindowSection = (line == "[LauncherWindow]");
            newContent += line + "\n";
            continue;
        }
        if (inVPinballXSection) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                key.erase(key.find_last_not_of(" \t") + 1);
                if (key == "FirstRun") {
                    newContent += "FirstRun=" + std::string(firstRun ? "true" : "false") + "\n";
                    firstRunWritten = true;
                    continue;
                }
                else if (key == "TablesDir") {
                    newContent += "TablesDir=" + tablesDir + "\n";
                    continue;
                }
                else if (key == "CommandToRun") {
                    newContent += "CommandToRun=" + commandToRun + "\n";
                    continue;
                }
                else if (key == "VPinballXIni") {
                    newContent += "VPinballXIni=" + vpinballXIni + "\n";
                    continue;
                }
            }
        }
        if (inLauncherWindowSection) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                key.erase(key.find_last_not_of(" \t") + 1);
                if (key == "EnableDPIAwareness") {
                    newContent += "EnableDPIAwareness=" + std::string(enableDPIAwareness ? "true" : "false") + "\n";
                    dpiAwarenessWritten = true;
                    continue;
                }
                else if (key == "DPIScaleFactor") {
                    newContent += "DPIScaleFactor=" + std::to_string(dpiScaleFactor) + "\n";
                    dpiScaleWritten = true;
                    continue;
                }
                else if (key == "WindowWidth") {
                    newContent += "WindowWidth=" + std::to_string(windowWidth) + "\n";
                    windowWidthWritten = true;
                    continue;
                }
                else if (key == "WindowHeight") {
                    newContent += "WindowHeight=" + std::to_string(windowHeight) + "\n";
                    windowHeightWritten = true;
                    continue;
                }
            }
        }
        newContent += line + "\n";
    }

    // Append missing keys for the last section
    if (inVPinballXSection && !firstRunWritten) {
        newContent += "FirstRun=" + std::string(firstRun ? "true" : "false") + "\n";
    }
    if (inLauncherWindowSection) {
        if (!dpiAwarenessWritten) {
            newContent += "EnableDPIAwareness=" + std::string(enableDPIAwareness ? "true" : "false") + "\n";
        }
        if (!dpiScaleWritten) {
            newContent += "DPIScaleFactor=" + std::to_string(dpiScaleFactor) + "\n";
        }
        if (!windowWidthWritten) {
            newContent += "WindowWidth=" + std::to_string(windowWidth) + "\n";
        }
        if (!windowHeightWritten) {
            newContent += "WindowHeight=" + std::to_string(windowHeight) + "\n";
        }
    }

    std::ofstream outFile(configFile);
    outFile << newContent;
    outFile.close();
}

void ConfigManager::setTablesDir(const std::string& path) {
    tablesDir = path;
}

void ConfigManager::setCommandToRun(const std::string& path) {
    commandToRun = path;
}

void ConfigManager::setVPinballXIni(const std::string& path) {
    vpinballXIni = path;
}

void ConfigManager::setFirstRun(bool value) {
    firstRun = value;
}

bool ConfigManager::isFirstRun() const {
    return firstRun;
}

bool ConfigManager::arePathsValid() const {
    bool tablePathValid = std::filesystem::exists(tablesDir) && std::filesystem::is_directory(tablesDir);
    if (tablePathValid) {
        bool hasVpxFile = false;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(tablesDir)) {
            if (entry.path().extension() == ".vpx") {
                hasVpxFile = true;
                break;
            }
        }
        tablePathValid = hasVpxFile;
    }

    bool vpxExecutableValid = std::filesystem::exists(commandToRun) && 
                             (std::filesystem::status(commandToRun).permissions() & std::filesystem::perms::owner_exec) != std::filesystem::perms::none;

    bool vpinballXIniValid = std::filesystem::exists(vpinballXIni);

    return tablePathValid && vpxExecutableValid && vpinballXIniValid;
}