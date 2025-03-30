#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>

class ConfigManager {
public:
    ConfigManager(const std::string& basePath);
    void loadSettings();

    // Configuration settings (public for simplicity; could use getters)
    std::string tablesDir;
    std::string startArgs;
    std::string commandToRun;
    std::string endArgs;
    std::string vpinballXIni;
    std::string fallbackEditor;
    std::string vpxTool;
    std::string vbsSubCmd;  // New: for "extractvbs"
    int windowWidth = 1024;
    int windowHeight = 768;
    std::string wheelImage;
    std::string tableImage;
    std::string backglassImage;
    std::string marqueeImage;
    std::string tableVideo;
    std::string backglassVideo;
    std::string dmdVideo;
    std::string romPath;
    std::string altSoundPath;
    std::string altColorPath;
    std::string musicPath;
    std::string pupPackPath;

private:
    std::string basePath;
    std::string prependBasePath(const std::string& relativePath) const;
};

#endif // CONFIG_MANAGER_H