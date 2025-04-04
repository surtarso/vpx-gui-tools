#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "utils/logging.h"
#include "iconfig_provider.h"
#include <string>

class ConfigManager : public IConfigProvider {
public:
    ConfigManager(const std::string& basePath);
    void loadSettings();
    void save() override;

    std::string getBasePath() const override { return basePath; }
    std::string getTablesDir() const override { return tablesDir; }
    std::string getStartArgs() const override { return startArgs; }
    std::string getCommandToRun() const override { return commandToRun; }
    std::string getEndArgs() const override { return endArgs; }
    std::string getVPinballXIni() const override { return vpinballXIni; }
    std::string getFallbackEditor() const override { return fallbackEditor; }
    std::string getVpxTool() const override { return vpxTool; }
    std::string getVbsSubCmd() const override { return vbsSubCmd; }
    std::string getPlaySubCmd() const override { return playSubCmd; }
    int getWindowWidth() const override { return windowWidth; }
    int getWindowHeight() const override { return windowHeight; }
    std::string getRomPath() const override { return romPath; }
    std::string getAltSoundPath() const override { return altSoundPath; }
    std::string getAltColorPath() const override { return altColorPath; }
    std::string getMusicPath() const override { return musicPath; }
    std::string getPupPackPath() const override { return pupPackPath; }
    std::string getWheelImage() const override { return wheelImage; }
    std::string getTableImage() const override { return tableImage; }
    std::string getBackglassImage() const override { return backglassImage; }
    std::string getMarqueeImage() const override { return marqueeImage; }
    std::string getTableVideo() const override { return tableVideo; }
    std::string getBackglassVideo() const override { return backglassVideo; }
    std::string getDmdVideo() const override { return dmdVideo; }
    std::string getVpxtoolIndexFile() const override { return vpxtoolIndexFile; }
    std::string getIndexerSubCmd() const override { return indexerSubCmd; }
    std::string getDiffSubCmd() const override { return diffSubCmd; }
    std::string getRomSubCmd() const override { return romSubCmd; }
    std::string getImGuiConf() const override { return imGuiConf; }
    // New DPI-related getters
    bool getEnableDPIAwareness() const { return enableDPIAwareness; }
    float getDPIScaleFactor() const { return dpiScaleFactor; }

    // New methods for first-run setup
    void setTablesDir(const std::string& path) override;
    void setCommandToRun(const std::string& path) override;
    void setVPinballXIni(const std::string& path) override;
    void setFirstRun(bool value) override;
    bool isFirstRun() const override;
    bool arePathsValid() const override;

private:
    std::string basePath;
    std::string prependBasePath(const std::string& relativePath) const;

    bool firstRun;
    std::string tablesDir;
    std::string startArgs;
    std::string commandToRun;
    std::string endArgs;
    std::string vpinballXIni;
    std::string fallbackEditor;
    std::string vpxTool;
    std::string vbsSubCmd;
    std::string playSubCmd;
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
    std::string vpxtoolIndexFile;
    std::string indexerSubCmd;
    std::string diffSubCmd;
    std::string romSubCmd;
    std::string imGuiConf;
    // New DPI-related members
    bool enableDPIAwareness = true; // Default to true
    float dpiScaleFactor = 1.0f; // Default to no scaling
};

#endif // CONFIG_MANAGER_H