#ifndef WINDOW_CONFIG_H
#define WINDOW_CONFIG_H

#include "iconfig_provider.h"

// Manages window and DPI-related settings
class WindowConfig : public IConfigProvider {
public:
    WindowConfig();
    void load(const std::string& configFile);

    // Window-related getters
    int getWindowWidth() const override { return windowWidth_; }
    int getWindowHeight() const override { return windowHeight_; }
    bool getEnableDPIAwareness() const override { return enableDPIAwareness_; }
    float getDPIScaleFactor() const override { return dpiScaleFactor_; }

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
    std::string getRomPath() const override { return ""; }
    std::string getAltSoundPath() const override { return ""; }
    std::string getAltColorPath() const override { return ""; }
    std::string getMusicPath() const override { return ""; }
    std::string getPupPackPath() const override { return ""; }
    std::string getWheelImage() const override { return ""; }
    std::string getTableImage() const override { return ""; }
    std::string getBackglassImage() const override { return ""; }
    std::string getMarqueeImage() const override { return ""; }
    std::string getTableVideo() const override { return ""; }
    std::string getBackglassVideo() const override { return ""; }
    std::string getDmdVideo() const override { return ""; }
    void setTablesDir(const std::string&) override {}
    void setCommandToRun(const std::string&) override {}
    void setVPinballXIni(const std::string&) override {}
    void setFirstRun(bool) override {}
    bool isFirstRun() const override { return false; }
    bool arePathsValid() const override { return false; }
    void save() override {}

private:
    int windowWidth_; // Launcher window width
    int windowHeight_; // Launcher window height
    bool enableDPIAwareness_; // Whether DPI awareness is enabled
    float dpiScaleFactor_; // DPI scaling factor
};

#endif // WINDOW_CONFIG_H