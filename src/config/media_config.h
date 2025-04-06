#ifndef MEDIA_CONFIG_H
#define MEDIA_CONFIG_H

#include "iconfig_provider.h"
#include <string>

// Manages media paths and extra folder settings
class MediaConfig : public IConfigProvider {
public:
    explicit MediaConfig(const std::string& basePath);
    void load(const std::string& configFile);

    // Media-related getters
    std::string getRomPath() const override { return romPath_; }
    std::string getAltSoundPath() const override { return altSoundPath_; }
    std::string getAltColorPath() const override { return altColorPath_; }
    std::string getMusicPath() const override { return musicPath_; }
    std::string getPupPackPath() const override { return pupPackPath_; }
    std::string getWheelImage() const override { return wheelImage_; }
    std::string getTableImage() const override { return tableImage_; }
    std::string getBackglassImage() const override { return backglassImage_; }
    std::string getMarqueeImage() const override { return marqueeImage_; }
    std::string getTableVideo() const override { return tableVideo_; }
    std::string getBackglassVideo() const override { return backglassVideo_; }
    std::string getDmdVideo() const override { return dmdVideo_; }

    // Unimplemented IConfigProvider methods (default to empty/zero)
    std::string getBasePath() const override { return ""; }
    std::string getTablesDir() const override { return ""; }
    std::string getStartArgs() const override { return ""; }
    std::string getCommandToRun() const override { return ""; }
    std::string getEndArgs() const override { return ""; }
    std::string getVPinballXIni() const override { return ""; }
    std::string getFallbackEditor() const override { return ""; }
    std::string getVpxTool() const override { return ""; }
    std::string getVbsSubCmd() const override { return ""; }
    std::string getPlaySubCmd() const override { return ""; }
    std::string getVpxtoolIndexFile() const override { return ""; }
    std::string getIndexerSubCmd() const override { return ""; }
    std::string getDiffSubCmd() const override { return ""; }
    std::string getRomSubCmd() const override { return ""; }
    std::string getImGuiConf() const override { return ""; }
    int getWindowWidth() const override { return 0; }
    int getWindowHeight() const override { return 0; }
    bool getEnableDPIAwareness() const override { return false; }
    float getDPIScaleFactor() const override { return 0.0f; }
    void setTablesDir(const std::string&) override {}
    void setCommandToRun(const std::string&) override {}
    void setVPinballXIni(const std::string&) override {}
    void setFirstRun(bool) override {}
    bool isFirstRun() const override { return false; }
    bool arePathsValid() const override { return false; }
    void save() override {}

private:
    std::string basePath_; // Root directory for relative paths
    std::string romPath_; // Path to ROM files
    std::string altSoundPath_; // Path to alternate sound files
    std::string altColorPath_; // Path to alternate color files
    std::string musicPath_; // Path to music files
    std::string pupPackPath_; // Path to PUP pack files
    std::string wheelImage_; // Path to wheel image
    std::string tableImage_; // Path to table image
    std::string backglassImage_; // Path to backglass image
    std::string marqueeImage_; // Path to marquee image
    std::string tableVideo_; // Path to table video
    std::string backglassVideo_; // Path to backglass video
    std::string dmdVideo_; // Path to DMD video

    std::string prependBasePath(const std::string& relativePath) const; // Prepends basePath_ to relative paths
};

#endif // MEDIA_CONFIG_H