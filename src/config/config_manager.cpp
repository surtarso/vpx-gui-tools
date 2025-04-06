#include "config_manager.h"
#include <filesystem>
#include <fstream>
#include <iostream>

// Constructs ConfigManager with a base path, initializing all config subcomponents
ConfigManager::ConfigManager(const std::string& basePath)
    : basePath_(basePath),
      pathsConfig_(std::make_unique<PathsConfig>(basePath_)),
      toolsConfig_(std::make_unique<ToolsConfig>(basePath_)),
      windowConfig_(std::make_unique<WindowConfig>()),
      mediaConfig_(std::make_unique<MediaConfig>(basePath_)) {
    loadSettings();
}

// Loads settings from settings.ini, creating it with defaults if it doesn't exist
void ConfigManager::loadSettings() {
    std::string configFile = basePath_ + "resources/settings.ini";
    if (!std::filesystem::exists(configFile)) {
        save(); // Write defaults if file is missing
    } else {
        pathsConfig_->load(configFile);
        toolsConfig_->load(configFile);
        windowConfig_->load(configFile);
        mediaConfig_->load(configFile);
    }
}

// Saves all config settings to settings.ini, preserving original format
void ConfigManager::save() {
    std::string configFile = basePath_ + "resources/settings.ini";
    std::ofstream outFile(configFile);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open " << configFile << " for writing\n";
        return;
    }

    outFile << "[VPinballX]\n"
            << "FirstRun=" << (pathsConfig_->isFirstRun() ? "true" : "false") << "\n"
            << "TablesDir=" << pathsConfig_->getTablesDir() << "\n"
            << "StartArgs=" << pathsConfig_->getStartArgs() << "\n"
            << "CommandToRun=" << pathsConfig_->getCommandToRun() << "\n"
            << "EndArgs=" << pathsConfig_->getEndArgs() << "\n"
            << "VPinballXIni=" << pathsConfig_->getVPinballXIni() << "\n"
            << "\n";

    outFile << "[LauncherWindow]\n"
            << "EnableDPIAwareness=" << (windowConfig_->getEnableDPIAwareness() ? "true" : "false") << "\n"
            << "DPIScaleFactor=" << windowConfig_->getDPIScaleFactor() << "\n"
            << "WindowWidth=" << windowConfig_->getWindowWidth() << "\n"
            << "WindowHeight=" << windowConfig_->getWindowHeight() << "\n"
            << "\n";

    outFile << "[Images]\n"
            << "WheelImage=" << mediaConfig_->getWheelImage() << "\n"
            << "TableImage=" << mediaConfig_->getTableImage() << "\n"
            << "BackglassImage=" << mediaConfig_->getBackglassImage() << "\n"
            << "MarqueeImage=" << mediaConfig_->getMarqueeImage() << "\n"
            << "\n";

    outFile << "[Videos]\n"
            << "TableVideo=" << mediaConfig_->getTableVideo() << "\n"
            << "BackglassVideo=" << mediaConfig_->getBackglassVideo() << "\n"
            << "DMDVideo=" << mediaConfig_->getDmdVideo() << "\n"
            << "\n";

    outFile << "[ExtraFolders]\n"
            << "ROMPath=" << mediaConfig_->getRomPath() << "\n"
            << "AltSoundPath=" << mediaConfig_->getAltSoundPath() << "\n"
            << "AltColorPath=" << mediaConfig_->getAltColorPath() << "\n"
            << "MusicPath=" << mediaConfig_->getMusicPath() << "\n"
            << "PUPPackPath=" << mediaConfig_->getPupPackPath() << "\n"
            << "\n";

    outFile << "[Tools]\n"
            << "FallbackEditor=" << toolsConfig_->getFallbackEditor() << "\n"
            << "VpxTool=" << toolsConfig_->getVpxTool() << "\n"
            << "\n";

    outFile << "[Internal]\n"
            << "VpxtoolIndexFile=" << toolsConfig_->getVpxtoolIndexFile() << "\n"
            << "IndexerSubCmd=" << toolsConfig_->getIndexerSubCmd() << "\n"
            << "DiffSubCmd=" << toolsConfig_->getDiffSubCmd() << "\n"
            << "RomSubCmd=" << toolsConfig_->getRomSubCmd() << "\n"
            << "VbsSubCmd=" << toolsConfig_->getVbsSubCmd() << "\n"
            << "PlaySubCmd=" << toolsConfig_->getPlaySubCmd() << "\n"
            << "ImGuiConf=" << toolsConfig_->getImGuiConf() << "\n";

    outFile.close();
}